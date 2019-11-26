var cardInfo = {
    id: -1,
    active: false,
    cardCreated: false,
    responseTopic: '/systemcard',
    bgColor: -1,
    nightlight: [
        0x4eb96f,
        0xc44569,
        0xf5cd79,
        0x546de5
    ],
    params: {
    },
    versions: {
      fw: '0',
      sw: '0'
    }
}
var notification = {
  active: false,
  duration: 0,
  startTime: 0
}
var messageQueue = [];

var setupComplete = false;

// definitions for the program
var elementId = {
    systemImage: 0,
    versionInfoFw: 1,
    versionInfoSw: 2,
};

var cardImg = {
    background: "blank_bg",
    buttons: "blank_buttons",
};


// card functions
function createCard () {
    if (cardInfo.cardCreated) {
      return;
    }
    var cardObj = generateNewCardObj(cardInfo.bgColor, cardInfo.responseTopic);

    cardObj.elements.push(generateImageElement(
                            elementId.systemImage,
                            generateImgPath(imgRootPath, 'system'),
                            0, 0)
                        );

    cardObj.elements.push(generateTextElement(
                            elementId.versionInfoFw,
                            'Build: ' + cardInfo.versions.fw,
                            15,
                            0, 245, 'left')
                        );

    cardObj.elements.push(generateTextElement(
                            elementId.versionInfoSw,
                            'Software: ' + cardInfo.versions.sw,
                            15,
                            0, 260, 'left')
                        );

    // init the card
    initCard(JSON.stringify(cardObj));
    cardInfo.cardCreated = true;
}

// notification functions
function setNotification(msg, duration) {
  publish('/notification', JSON.stringify({
    cmd: 'set',
    text: msg
  }));
  notification.active = true;
  notification.duration = duration;
  notification.startTime = Date.now();
}

// temporary solution to clear a notification after a set amount of time
function checkNotification() {
  // only check if notification is active
  if (notification.active && notification.duration > 0) {
    if ((Date.now() - notification.startTime) >= (notification.duration * 1000)) {
      print('notification expired');
      publish('/notification', JSON.stringify({
        cmd: 'clear'
      }));
      notification.active = false;
    }
  }
}

// input handlers
function handleButtonInput(e) {
    print('Handling button input');
    print(JSON.stringify(e));
    if (!cardInfo.cardCreated) {
      // look for simultaneous three button press
      if (e.id === 14 && e.multitouch === true && e.action === 'press') {
        // create the card
        print('Creating System card!');
        createCard()
      }
    }
    else if (cardInfo.active) {
      // check for button long press
      if (e.duration >= 3000) {
        print('Detected 3sec button ' + e.id);
        switch (e.id) {
          case 0:
            // reset wifi
            setNotification('Resetting all WiFi settings', 5);
            var result = popen('wifisetup reset');
            break;
          case 1:
            // enter setup mode
            setNotification('Now in Setup Mode!', 5);
            system('touch /tmp/setupMode');
            break;
          case 2:
            // trigger software update
            setNotification('Software Update in progress', 0);
            system('sh /etc/cron.week/firmware_update.sh');
            break;
          case 3:
            // enter bluetooth pairing mode
            setNotification('Bluetooth Pairing started', 5);
            publish('/audio', JSON.stringify({
              cmd: 'btPair'
            }))
            break;
          default:
            // nothing
            break;
        }
      }
    }
}

function handleGestureInput(e) {
  print('Handling gesture input');
  print(JSON.stringify(e));
  if (cardInfo.cardCreated) {
    if (e.direction === 'left' || e.direction === 'right') {
      print('deleting card!');
      // delete the card
      deleteCard(cardInfo.id);
      // reset the card ID
      cardInfo.id = -1;
      cardInfo.cardCreated = false;
      cardInfo.active = false;
    }
  }
}

// base functions
function setup() {
    // grab version info
    cardInfo.versions.fw = popen('uci get onion.@onion[0].build');
    cardInfo.versions.fw = cardInfo.versions.fw.slice(0,-1);
    cardInfo.versions.sw = popen('opkg list-installed | grep oboo-clock-base | awk \'{printf $3;}\'');
    print('version data: ' + JSON.stringify(cardInfo.versions));
    // connect to mqtt broker
    connect('localhost', 1883, null, function () {
          print('Running connect callback');
          subscribe('/cardResponse');
          subscribe('/button');
          subscribe('/gesture');
          setupComplete = true;
          print('setup complete');
      },
      null,
      '/card',
      JSON.stringify({
        cmd: 'remove_card',
        cardName: cardInfo.responseTopic
      })
    );
}

function loop() {
    if (!setupComplete) {
      print('skipping loop');
      return 0;
    }

    // handle notifications
    checkNotification();

    // handle received messages
    if (messageQueue.length > 0) {
        for (var i = 0; i < messageQueue.length; i++) {
          if (messageQueue[i].source === 'button') {
            handleButtonInput(messageQueue[i].payload);
          } else if (messageQueue[i].source === 'gesture') {
            handleGestureInput(messageQueue[i].payload);
          }
        }
        messageQueue = [];
    }
}

function onInput(e) {
    if (typeof e.source !== 'undefined' && typeof e.payload !== 'undefined' ) {
        print('input! input source: ' + e.source);
        messageQueue.push(e);
    }
}

function onMessage(e) {
    if (typeof e.topic !== 'undefined' && typeof e.payload !== 'undefined' ) {
        print('message! topic: ' + e.topic + ', value: ' + e.payload);
        switch (e.topic) {
            case '/cardResponse':
                cardInfo = handleCardResponseMessage(cardInfo, e.payload);
                break;
            default:
                break;
        }
    }
}
