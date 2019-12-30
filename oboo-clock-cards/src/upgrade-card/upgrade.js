// TODO: what is this card? (looks like it runs an update from usb?)
var cardLib = require("card-lib");
var onRecvMessage = cardLib.onRecvMessage; // is there a cleaner way to do this?

var cardInfo = {
    id: -1,
    responseTopic: '/upgradeCard' + cardLib.getEpochMillis(),
    bgColor: 13,
}

// definitions for the program
var elementId = {
    upgradeImage: 0
};

var upgradeImg = {
    splash: "upgrade"
};

function handleButtonInput (e) {
  if (typeof e.id !== 'undefined' &&
      typeof e.action !== 'undefined' &&
      typeof e.duration !== 'undefined' &&
      typeof e.multitouch !== 'undefined')
   {
     if (e.id === 14 && e.multitouch && e.duration >= 10000) {
       print('UPGRADE COMMAND DETECTED!');
       runUpgradeProgram()
     }
   }
}

function runUpgradeProgram () {
    system('sh /usr/bin/usbUpgrade.sh');
}

function setup() {
    connect('localhost', 1883, null, function () {
        subscribe('/button');

        print('Setup callback complete');
    });
}

function loop() {
    // do nothing
}

function onInput(e) {
    if (typeof e.source !== 'undefined' && typeof e.payload !== 'undefined' ) {
        print('input! input source: ' + e.source + ', value: ' + JSON.stringify(e.payload));
        if ( e.source === 'button') {
          handleButtonInput(e.payload);
        }
    }
}

function onMessage(e) {
    // do nothing

    // if (typeof e.topic !== 'undefined' && typeof e.payload !== 'undefined' ) {
    //     print('message! topic: ' + e.topic + ', value: ' + e.payload);
    //     switch (e.topic) {
    //         case '/cardResponse':
    //             cardInfo = cardLib.handleCardResponseMessage(cardInfo, e.payload);
    //             break;
    //         default:
    //             break;
    //     }
    // }
}
