var messageQueue = [];
var bConfigUpdate = false;

var dateCheck = new Date();
var secCurrent = dateCheck.getSeconds();

// helper functions
/** Omega -> MCU**/
// perform the sending to mcu
function sendMcuCmd (cmd, payload) {
    print ('sendMcuCmd, cmd.length = ' + cmd.length + ' payload.length = ' + payload.length);
    if (cmd.length === 1 && payload.length >= 1) {
        var data = '~' + cmd + payload + ';';
        print('Sending command to mcu: ' + data);
        serialWrite(String(data));
    }
}

// --- functions to send commands to the mcu --- //
function setMcuTime (payload) {
  var serialCmd = 'T';
  sendMcuCmd(serialCmd, String(payload));
}

function setMcuHourMode (payload) {
  var serialCmd = 'H';
  var serialPayload = parseInt(payload);
  if (serialPayload === 12 || serialPayload === 24 ) {
      sendMcuCmd(serialCmd, String(serialPayload));
  }
}

function setMcuNightLight (payload) {
  var serialCmd = 'L';
  sendMcuCmd(serialCmd, String(parseInt(payload)));
}

function setMcuButton (id, payload) {
  var serialCmd = 'b';
  if (typeof id !== 'undefined') {
      var serialPayload = String(id) + String(parseInt(payload));
      sendMcuCmd(serialCmd, String(serialPayload));
  }
}

function setMcuButtons (value) {
  var serialCmd = 'l';
  var serialPayload = String(value[0]) + ':' + String(value[1]) + ':' + String(value[2]) + ':' + String(value[3]);
  sendMcuCmd(serialCmd, String(serialPayload));
}

function setMcuTimeDisplayBrightness (payload) {
  var serialCmd = 'B';
  sendMcuCmd(serialCmd, String(parseInt(payload)));
}

function setMcuDayNightLed (payload) {
  var serialCmd = 'D';
  var serialPayload = ((payload === true || payload === 'true') ? 1 : 0);
  sendMcuCmd(serialCmd, String(serialPayload));
}

function setMcuInit () {
  var serialCmd = 'B';
  var serialPayload = '1';
  sendMcuCmd(serialCmd, String(serialPayload));
}


// parse mqtt messages
function handleSetProperty (e) {
    if (typeof e.cmd !== 'undefined' && typeof e.value !== 'undefined') {
        // var serialCmd = '';
        // var serialPayload = '';

        switch (e.cmd) {
            case 'rtc':
            case 'time':
                setMcuTime (e.value);
                break;
            case 'hour mode':
            case 'hourMode':
                // serialPayload = parseInt(e.value);
                // if (serialPayload === 12 || serialPayload === 24 ) {
                //     serialCmd = 'H';
                // }
                setMcuHourMode(e.value);
                break;
            case 'nightlight':
                // serialPayload = parseInt(e.value);
                // serialCmd = 'L';
                setMcuNightLight(e.value)
                break;
            case 'button':
                if (typeof e.id !== 'undefined') {
                    // serialCmd = 'b';
                    // serialPayload = String(e.id) + String(parseInt(e.value));
                    setMcuButton(e.id, e.value)
                }
                break;
            case 'buttons':
                if (Array.isArray(e.value) && e.value.length === 4) {
                    print('value IS TOTALLY an array of length 4') // debug
                    // serialCmd = 'l';
                    // serialPayload = String(e.value[0]) + ':' + String(e.value[1]) + ':' + String(e.value[2]) + ':' + String(e.value[3]);
                    setMcuButtons(e.value);
                }
                // debug
                else {
                  print('value is NOT an array of length 4')
                }
                break;
            case 'brightnessTime':
            case 'timeDisplay':
                // serialCmd = 'B';
                // serialPayload = parseInt(e.value);
                setMcuTimeDisplayBrightness(e.value);
                break;
            case 'day-night-indicator':
            case 'dayNightLed':
                // serialCmd = 'D';
                // serialPayload = ((e.value === true || e.value === 'true') ? 1 : 0);
                setMcuDayNightLed(e.value);
                break;
            case 'init':
                // serialCmd = 'I';
                // serialPayload = '1';
                setMcuInit();
                break;
            default:
                break;
        }

        // if (serialPayload === '') {
        //     serialPayload = e.value;
        // }
        //
        // sendMcuCmd(serialCmd, String(serialPayload));
        // TODO: publish ack to set
    }
}

/** MCU -> Omega **/
// decode received button messages
function decodeButtons(inputStr) {
    var buttons = [];
    var payload = parseInt(inputStr.slice(0,2), 16);
    var duration = parseInt(inputStr.slice(2));
    print('button payload is ' + payload + ', pressed for ' + duration + ' milliseconds');

    // check which buttons have been pressed
    if (payload & 0x01) {
        buttons.push(0);
    }
    if (payload & 0x02) {
        buttons.push(1);
    }
    if (payload & 0x04) {
        buttons.push(2);
    }
    if (payload & 0x08) {
        buttons.push(3);
    }

    if (buttons.length > 0) {
      var obj = {
        id: buttons[0],
        action: 'press',
        duration: duration,
        multitouch: false
      }
      // adjustments for multi-touch case
      if (buttons.length > 1) {
        obj['id'] = payload;
        obj['multitouch'] = true;
      }
      print ('button: ' + JSON.stringify(obj));
      publish('/button', JSON.stringify(obj));
    }
}

// decode received gesture messages
function decodeGesture(inputStr) {
    var direction = ''

    // publish to gesture topic
    switch (inputStr) {
        case 'L':
            direction = 'left';
            break;
        case 'R':
            direction = 'right';
            break;
        case 'U':
            direction = 'up';
            break;
        case 'D':
            direction = 'down';
            break;
        case 'X':
        default:
            direction = 'none';
            break;
    }
    publish('/gesture', JSON.stringify({
      direction: direction
    }))

    // implement card switching
    // TODO: this functionality should be moved to the card manager
    switch (inputStr) {
        // more natural input:
        //  gesture left makes card animate leftward
        case 'L':
            direction = 'right';
            break;
        case 'R':
            direction = 'left';
            break;
        default:
            direction = '';
            break;
    }

    if (direction !== '') {
        var msg = {
            cmd: 'select_card',
            action: direction
        };
        print ('gesture: ' + direction)
        publish('/card', JSON.stringify(msg));
    }
}

// calculate battery percentage
var batterySteps = [
    3.65,
    3.7,
    3.78,
    3.9,
    4.15
];

function calculateBatteryPercentage(a) {
    var detectedVoltage = a * 3.3 / 1024.0;
    var batteryVoltage = detectedVoltage * 1.49;

    for (var i = batterySteps.length; i >= 0; i--) {
        if (batteryVoltage >= batterySteps[i]) {
            return i;
        }
    }

    // default is empty
    return 0;
}

// decode battery voltage(
function decodeBattery (input) {
    var payload = {
        cmd: 'update',
        elements: [{
            type: 'battery',
            value: calculateBatteryPercentage(input)
        }]
    };
    publish('/status', JSON.stringify(payload));
}

function wifiStatus (input) {
    var payload = {
        cmd: 'update',
        elements: [{
            type: 'wifi',
            value: input
        }]
    };
    publish('/status', JSON.stringify(payload));
}

// decode time from MCU
function decodeTime (inputTime) {
    if (inputTime.match(/^\d+$/) !== null) {
        print('received date: ' + inputTime);
        updateSystemTime(inputTime);
    }
}


// parse property received from MCU
function handleReceivedProperty(cmd, payload) {
    switch (cmd) {
        case 'V':
            // battery voltage (0-1023)
            decodeBattery(payload);
            break;
        case 'A':
            // ambient light (0-?)
            // TODO: implement this when supported by hw
            break;
        case 't':
            // touch button
            decodeButtons(payload);
            break;
        case 'G':
            // gesture
            decodeGesture(payload);
            break;
        case 'i':
            // time from RTC
            decodeTime(payload);
            break;
        default:
            print ('Unrecognized command');
            break;
    }
}

// parse string of received messages from MCU
function handleReceivedProperties (msg) {
    var props = msg.split(/\r?\n/);
    props.forEach(function(element) {
        if (element) {
            var components = element.match(/^~(.)(.+);/);

            if (components != null && components.length === 3) {
                print ('cmd: ' + components[1] + ', value: ' + components[2]);
                handleReceivedProperty(components[1], components[2]);
            }
        }
    });
}

// config functions to read and implement config changes
function readConfig() {
    readFile('/etc/config.json', '', function (err, data) {
        if (!err) {
            var config;
    	    try {
    	        state = JSON.parse(data);
    	    } catch(e) {
    	        print(e); // error in the above string!
    	        return null;
    	    }

    	    // apply the settings from the config file
          if (state.config.hourMode) setMcuHourMode(state.config.hourMode);
          if (typeof(state.config.dayNightLed) == typeof(true)) setMcuDayNightLed(state.config.dayNightLed);
          if (state.config.brightnessTime) setMcuTimeDisplayBrightness(state.config.brightnessTime);
        }
        print('configuration: hourMode = ' + state.config.hourMode + '; dayNightLed = ' + state.config.dayNightLed + '; brightnessTime = ' + state.config.brightnessTime);
    });
}

function readWifiHelper(){
  var status;
  readFile('/tmp/wifi-active', '', function (err, data) {
        if (!err) {            
    	    try {
    	        status = data;
    	    } catch(e) {
    	        print(e); // error in the above string!
    	        return null;
    	    }

        }
        print('status ' + status);
  });
  if(status.includes("1")){
    wifiStatus(1);
  } else {
    wifiStatus(0);  
  }
}


// base functions
function setup() {
    connect('localhost', 1883, null, function () {
        print('subscribing to topic');
        subscribe('/set');
        subscribe('/config/update');

        print('opening serial port');
        openSerialPort("/dev/ttyS1");

        sendMcuCmd ("I", "0");
    });
    sleep(2);
}

function loop() {
    // handle any config updates
    if (bConfigUpdate) {
      readConfig()
      bConfigUpdate = false;
    }

    // handle any incoming mqtt messages
    if (messageQueue.length > 0) {
        for (var i = 0; i < messageQueue.length; i++) {
            handleSetProperty(messageQueue[i]);
        }
        messageQueue = [];
    }

    // look for new messages from mcu -- this is a blocking call
    var msg = serialRead();
    if (msg !== null && msg.length > 0) {
        print("received serial message of length " + msg.length + " value is " + msg);
        handleReceivedProperties(msg);
    }

    dateLocal = new Date();

    if((dateLocal.getSeconds() % 15) == 0 && secCurrent != dateLocal.getSeconds()){
      print("update wifi status");
      secCurrent = dateLocal.getSeconds();
      readWifiHelper();
//	check if connected
//	send status message

    }
}

function onMessage(e) {
    if (typeof e.topic !== 'undefined' && typeof e.payload !== 'undefined' ) {
        print('message! topic: ' + e.topic + ', value: ' + JSON.stringify(e.payload));
        if (e.topic === '/set') {
            messageQueue.push(e.payload);
        } else if (e.topic === '/config/update') {
            bConfigUpdate = true;
        }
    }
}
