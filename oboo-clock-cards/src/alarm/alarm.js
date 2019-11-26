var configString = '{"config":{"hourMode":24,"dayNightLed":true,"tz":"America/Toronto","location":"","brightnessTime":5,"brightnessInfo":4,"timezone":"EST5EDT,M3.2.0,M11.1.0","autoTimeZone":true},"alarms":{"1525527435840":{"active":false,"time":"23:02","days":{"Sunday":false,"Monday":true,"Tuesday":false,"Wednesday":false,"Thursday":false,"Friday":false,"Saturday":false},"alarmTone":"Singing in the rain","snooz":true},"1525531157332":{"active":false,"time":"04:19","days":{"Sunday":true,"Monday":false,"Tuesday":false,"Wednesday":false,"Thursday":false,"Friday":false,"Saturday":false},"alarmTone":"Singing in the rain","snooz":false},"1525532545182":{"active":true,"time":"18:43","days":{"Sunday":false,"Monday":true,"Tuesday":true,"Wednesday":true,"Thursday":true,"Friday":true,"Saturday":false},"alarmTone":"Singing in the rain","snooz":true},"1525532589262":{"active":true,"time":"23:21","days":{"Sunday":false,"Monday":false,"Tuesday":false,"Wednesday":false,"Thursday":false,"Friday":true,"Saturday":false},"alarmTone":"Singing in the rain","snooz":true},"1527469578941":{"active":false,"time":"22:33","days":{"Sunday":false,"Monday":false,"Tuesday":false,"Wednesday":false,"Thursday":false,"Friday":false,"Saturday":false},"alarmTone":"Singing in the rain","snooz":true},"1527595868233":{"active":false,"time":"02:10","days":{"Sunday":false,"Monday":false,"Tuesday":false,"Wednesday":false,"Thursday":false,"Friday":false,"Saturday":false},"alarmTone":"Singing in the rain","snooz":false}},"cards":{"0":{"name":"maps","id":98765,"tempUnit":"imperial","card":"Weather","location":"Toronto","distanceUnit":"imperial"},"1":{"name":"timer","id":321,"card":"Calendar","iCalUrl":"urlURL 123456"},"2":{"card":"Traffic","distanceUnit":"imperial","trafficFrom":"From 1230","trafficTo":"To 3210"}},"properties":{"name":"","hardware":{"model":"","rev":"","mac":""},"software":{"build":"","version":"","ip":""},"online":false}}';

var weekdayStringArray = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];

var useDebugConfigString = false;
//you can set this to true to test, but to test snooze you need to disable the if check since there won't be a config
var alarmTriggered = false;
var dataReady = false;
var lastSoundPlayedTil = new Date();
var mainRan = false;
var triggeredAlarmConfig = "";
var currentDate = new Date();
var alarmArmed = false;
var expireSound = "/usr/share/oboo/alarm/alarm_classic.mp3";
var snoozeDurationInSeconds = 60;
var defaultAlarmDurationInSeconds = 60*60*24;
var alarmPid = 0;
var snoozeOn = false;

function main() {
    print(currentDate);

    var alarmJson =  JSON.parse(configString);

    var alarms = {};

    if ('alarms' in alarmJson) {
       alarms = alarmJson['alarms'];
    }

    print (JSON.stringify(alarms))


    Object.keys(alarms).forEach(function(key) {
        var alarmConfig = alarms[key];
        print(key, alarmConfig);

        if (checkIfAlarmShouldBeTriggered(key, alarmConfig)) {
            triggerAlarm(alarmConfig);
        }
    });
}

function checkIfAlarmShouldBeTriggered(key, alarmConfig) {
    if (!alarmConfig['active']) {
        print(key + " is not active");
        return false;
    }
    var currentDayString = weekdayStringArray[currentDate.getDay()];
    if (!alarmConfig['days'][currentDayString]) {
        print(key + " is not " + currentDayString);
        return false;
    }
    var alarmHour24 = parseInt(alarmConfig['time'].split(':')[0]);
    var alarmMinute = parseInt(alarmConfig['time'].split(':')[1]);

    print(key + " hour " + alarmHour24 + " minute " + alarmMinute);

    if (currentDate.getHours() != alarmHour24) {
        print(key + " is not at hour " + alarmHour24);
        return false;
    }

    if (currentDate.getMinutes() != alarmMinute) {
        print(key + " is not at minute " + alarmMinute);
        return false;
    }

    return true;
}

function triggerAlarm(alarmConfig) {
    print('alarm triggered!!');
    triggeredAlarmConfig = alarmConfig;
    alarmTriggered = true;
}

function armTimer() {
    alarmArmed = true;
    publish('/audio', JSON.stringify({
      cmd:'source',
      value:'usb'
    }));
}

function playAlarmSound(sound) {
    if (lastSoundPlayedTil - new Date() < 0) {
        if (!alarmArmed) {
            armTimer();
        }
        snoozeOn = false;
        print("playing alarm");
        publish('/audio', JSON.stringify({
            cmd: 'playLoop',
            value: expireSound
        }));
        lastSoundPlayedTil = new Date();
        //since we have the loop function the lastSoundPlayedTil will be a day (basically forever)
        lastSoundPlayedTil.setSeconds(lastSoundPlayedTil.getSeconds() + defaultAlarmDurationInSeconds);
    }
}

function stopCurrentAlarmPlayback() {
    if (alarmPid != 0) {
        print("Stopping alarm: " + alarmPid);
        publish("/audio", JSON.stringify({
            cmd: 'stop',
            value: alarmPid
        }));
        alarmPid = 0;
    }
}

function handleAlarmSleep() {
    if (!triggeredAlarmConfig['snooz']) {
        return;
    }
    //need the variable so repeated gestures don't add additional timeout
    if (!snoozeOn) {
        snoozeOn = true;
        stopCurrentAlarmPlayback();
        lastSoundPlayedTil = new Date();
        lastSoundPlayedTil.setSeconds(lastSoundPlayedTil.getSeconds() + snoozeDurationInSeconds);
    }
}

function handleAlarmCancel() {
    stopCurrentAlarmPlayback();
    alarmTriggered = false;
}

function readObooConfig() {
    readFile('/etc/config.json', '', function(err, data) {
        if (!useDebugConfigString) {
            // will be converted to JSON in main() function
            configString = data;
        }
        //the async and the run loop requires this for the actual main logic to run at the correct timing.
        dataReady = true;
    });
}


function setup() {
    readObooConfig();

    connect('localhost', 1883, null, function () {
        //for detecting swipe gesture
        subscribe('/card', function () {
        });

        //for detecting button press
        subscribe('/button', function () {
        });

        //for getting audio pid for looped playback
        subscribe('/audioResp', function () {
        });

        // for config file updates
        // subscribe('/config/update');
    });
}

function loop() {
    if (dataReady && !mainRan) {
        main();
        mainRan = true;
    }

    if (alarmTriggered) {
        playAlarmSound();
    } else {
        print("Exiting");
        return -1;
    }
}

function onInput(e) {
    if (typeof e.source !== 'undefined' && typeof e.payload !== 'undefined' ) {
        print('input! input source: ' + e.source + ', value: ' + JSON.stringify(e.payload));
        //we assume all onInput events are originated from button presses
        handleAlarmCancel();
    }
}

function onMessage(e) {
    if (typeof e.topic !== 'undefined' && typeof e.payload !== 'undefined' ) {
        print('message! topic: ' + e.topic + ', value: ' + JSON.stringify(e.payload));

        //Need to check for specific topic since there can be new topics that can added later
        switch (e.topic) {
            case "/audioResp":
                //Make sure this is for us, although it is still possible to have race conditions assuming the message is publish to all
                if (e.payload.source === expireSound) {
                    alarmPid = e.payload.pid;
                    print("Got alarmpid: " + alarmPid);
                }
                break;

            case "/card":
                //Detected swipe
                handleAlarmSleep();
                break;

            case "/config/update":
                //Detected configuration update - read alarm config from
                readObooConfig();
                break;

            default:
                break;
        }
    }
}
