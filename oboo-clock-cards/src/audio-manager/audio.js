var messageQueue = [];
var info = {
  gpio: {
    enable: 3,
    source: 41
  },
  source: {
    bluetooth: 1,
    usb: 0
  },
  serial: {
    port: "/dev/ttyS2",
    baudrate: 115200
  },
  command: {
    getState: new Uint8Array([0xff, 0xfe, 0xe6, 0x1a, 0x00]),
    btPair: new Uint8Array([0xff, 0xfe, 0xe7, 0x19, 0x00]),
    volUp: new Uint8Array([0xff, 0xfe, 0xed, 0x13, 0x00]),
    volDown: new Uint8Array([0xff, 0xfe, 0xec, 0x14, 0x00]),
    playPause: new Uint8Array([0xff, 0xfe, 0xf0, 0x10, 0x00]),
    nextTrack: new Uint8Array([0xff, 0xfe, 0xef, 0x11, 0x00]),
    prevTrack: new Uint8Array([0xff, 0xfe, 0xee, 0x12, 0x00]),
  },
  audioProcessList: {},
  status: {
    source: -1,
    bluetooth: {
      paired: false,
      connected: false,
      playing: false
    }
  },
  files: {
    emptyTrack: '/usr/share/oboo/alarm/silence-1s.mp3'
  }
}


// helper functions
function audioEnable (bEnable) {
  setGpio(info.gpio.enable, bEnable ? 1 : 0);
}

function selectSource (sourceVal) {
  setGpio(info.gpio.source, sourceVal);
}

function selectBtSource () {
  if (info.status.source !== info.source.bluetooth) {
    // switch to bluetooth source
    selectSource(info.source.bluetooth);
    // track that current source is bluetooth
    info.status.source = info.source.bluetooth;
  }
}

function selectUsbSource () {
  if (info.status.source !== info.source.usb) {
    // stop the bluetooth source if it's currently playing
    if (info.status.bluetooth.playing) {
      playPause();
    }
    // switch to bluetooth source
    selectSource(info.source.usb);



    // perform the activities required to initialize usb audio
    sleep(1); // wait for usb device to be recognized
    var ret = popen('mpg123 ' + info.files.emptyTrack)// play a dummy audio file
    // replay if there was an error
    if (ret.indexOf('error') >= 0) {
      var ret = popen('mpg123 ' + info.files.emptyTrack)// play a dummy audio file
    }
    print(ret);
    sleep(1); // wait for amp to accept input

    volumeSet(20);
    // track that current source is usb
    info.status.source = info.source.usb;
  }
}


function sendCommand (data) {
  serialWrite(data);
}

function sendByteArrayCommand (data) {
  serialWriteByteArray(data);
}


function getModuleState () {
  sendByteArrayCommand(info.command.getState);
}

function btPairCommand () {
  // var typedArray = new Uint8Array([0xff, 0xfe, 0xe7, 0x19, 0x00]);
  // sendByteArrayCommand(typedArray);
  sendByteArrayCommand(info.command.btPair);
}

function volumeUp () {
  // sendCommand("\xff\xfe\xed\x13\x00");
  // sendCommand("\\xff\\xfe\\xed\\x13\\x00");

  // var typedArray = new Uint8Array([0xff, 0xfe, 0xed, 0x13, 0x00]);
  // sendByteArrayCommand(typedArray);
  sendByteArrayCommand(info.command.volUp);
}

function volumeDown () {
  // sendCommand('\xff\xfe\xec\x14\x00');

  // var typedArray = new Uint8Array([0xff, 0xfe, 0xec, 0x14, 0x00]);
  // sendByteArrayCommand(typedArray);
  sendByteArrayCommand(info.command.volDown);
}

function volumeSet (vol) {
  // launchProcess("amixer set 'PCM' " + vol + '%');
  launchProcess("amixer set 'PCM',1 " + vol + '%');
}

function playPause () {
  sendByteArrayCommand(info.command.playPause);
}

function nextTrack () {
  sendByteArrayCommand(info.command.nextTrack);
}

function prevTrack () {
  sendByteArrayCommand(info.command.prevTrack);
}

function playAudioFile (filePath) {
  print('playing audio file');
  // play audio
  var ret = launchProcess('mpg123 ' + filePath);
  print('audio playback started, pid: ' + ret);
}

function playAudioFileLoop (filePath) {
  print('playing audio file in a LOOP');
  // start audio in loop
  var pid = launchProcess('mpg123 --loop -1 ' + filePath);
  print('audio loop started, pid: ' + pid);

  // add pid to process list
  info.audioProcessList[pid] = filePath;

  return pid;
}

function stopAudioFileByPid (pid) {
  // stop audio based on pid
  if (pid in info.audioProcessList) {
    print('killing audio process with pid ' + pid);
    // stop the process
    killProcess(pid);
    // remove from the process list
    delete info.audioProcessList[pid];
  }
}


// interaction functions
function setBluetoothStatusBarIcon(bEnabled) {
  //  /status { "cmd":"update", "elements": [ { "type": "bluetooth", "value": 1 } ] }
  publish('/status', JSON.stringify({
    cmd: 'update',
    elements: [
      {
        type: 'bluetooth',
        value: (bEnabled ? 1 : 0)
      }
    ]
  }));
}


// handle received serial data
function handleReceivedData(e) {
  if (typeof e !== 'undefined' && e.constructor === Array && e.length >= 6) {
    // ensure first 5 bytes match expected sequence:
    //  FF FE xx xx 01
    if (e[0] === 0xff &&
        e[1] === 0xfe &&
        e[4] === 0x01   )
    {
      print('received valid data from bt, last byte = ' + e[5]);
      var prevConnectedState = info.status.bluetooth.connected;
      // reset the status
      info.status.bluetooth.paired = false;
      info.status.bluetooth.connected = false;
      info.status.bluetooth.playing = false;
      // decode last byte
      if (e[5] & 0x01) {
        info.status.bluetooth.paired = true
      }
      if (e[5] & 0x02) {
        info.status.bluetooth.connected = true
      }
      if (e[5] & 0x04) {
        info.status.bluetooth.playing = true
      }
      if (e[5] & 0x08) {
        // currently pairing - don't need to track this though
      }
      print(JSON.stringify(info.status.bluetooth));
      // take action based on bluetooth status
      // set bluetooth status icon
      setBluetoothStatusBarIcon(info.status.bluetooth.connected);
      // switch to bluetooth source if playing or just connected
      if (info.status.bluetooth.playing ||
          (!prevConnectedState && info.status.bluetooth.connected)
        )
      {
        selectBtSource();
      }
    }
  }
}

// handle incoming messages
function handleMessage (e) {
  if (typeof e.cmd !== 'undefined') {
    switch (e.cmd) {
      case 'source':
        if (typeof e.value !== 'undefined') {
          print('switching audio source to ' + e.value); //tmp debug
          if (e.value === 'usb') {
            selectUsbSource();
          }
          else if (e.value === 'bt' || e.value === 'bluetooth' ) {
            selectBtSource();
          }
        }
        break;
      case 'play':
        if (typeof e.value !== 'undefined') {
          print('playing audio file ' + e.value); //tmp debug
          playAudioFile(e.value);
        }
        break;
      case 'playLoop':
        if (typeof e.value !== 'undefined') {
          var ret = playAudioFileLoop(e.value);
          // respond with pid
          publish('/audioResp', JSON.stringify({
            action: 'play',
            source: e.value,
            pid: ret
          }))
        }
        break;
      case 'stop':
        if (typeof e.value !== 'undefined') {
          stopAudioFileByPid(e.value);
        }
        break;
      case 'test':
        // TODO: remove this when successfully tested
        if (typeof e.value !== 'undefined') {
          system(e.value);
        }
        break;
      case 'btPair':
        btPairCommand();
        break;
      case 'volUp':
        volumeUp();
        break;
      case 'volDown':
        volumeDown();
        break;
      case 'volSet':
      print('on volSet');
        if (e.value !== 'undefined') {
          print(e.value);
          volumeSet(e.value);
        }
        break;
      case 'playPause':
        playPause();
        break;
      case 'nextTrack':
        nextTrack();
        break;
      case 'prevTrack':
        prevTrack();
        break;
      default:
        print('Received unsupported command');
        break;
    }
  }
}

// base functions
function setup() {
  // set bluetooth audio by default
  selectBtSource();
  // enable audio
  audioEnable(1);

  print('opening serial port');
  openSerialPort(info.serial.port, info.serial.baudrate);

  // connect to mqtt broker
  connect('localhost', 1883, null, function () {
    print('subscribing to topic');
    subscribe('/audio');
  }, false);

  // retrieve bluetooth module state
  // TODO: debug this
  // getModuleState();
}

function loop() {
  // print ('looping!');
  // look for new messages from bluetooth module
  var msg = serialReadByteArray();
  if (msg !== null && msg.length > 0) {
      print('returned array with length: ' + msg.length + ' values: ' + msg);
      handleReceivedData(msg);
  }

  // handle any incoming mqtt messages
  if (messageQueue.length > 0) {
    for (var i = 0; i < messageQueue.length; i++) {
      handleMessage(messageQueue[i]);
    }
    messageQueue = [];
  }
}

function onMessage(e) {
  if (typeof e.topic !== 'undefined' && typeof e.payload !== 'undefined' ) {
    print('message! topic: ' + e.topic + ', value: ' + JSON.stringify(e.payload));
    if (e.topic === '/audio') {
      messageQueue.push(e.payload);
    }
  }
}
