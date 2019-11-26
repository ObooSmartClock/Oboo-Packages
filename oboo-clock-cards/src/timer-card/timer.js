var cardInfo = {
    id: -1,
    active: false,
    responseTopic: '/timerCard_' + getEpochMillis(),
    bgColor: 13,
    nightlight: [
        0xff0000,
        0x00ff00,
        0x0000ff,
        0x88ffff
    ],
    params: {
        timerRunning: false,
        timerExpired: false,
        timerStartValue: 0,
        timerLength: 10*60*1000,    // 10 minutes in ms
        lastUpdate: "",
        expireSound: '/usr/share/oboo/alarm/timer-0.mp3',
        timerArmOffset: 6*1000, // arm the timer 6 seconds before it rings
        timerArmed: false
    },
    enabled: true
}

// definitions for the program
var elementId = {
    background: 0,
    buttons: 1,
    timerStartValue: 2,
    timerValue: 3,
};

var timerImg = {
    background: "timer_bg",
    buttons: "timer_buttons",
};

function tick() {
    var timeLeft;
    if (cardInfo.params.timerExpired) {
        timeLeft = 0;
    }
    else if (cardInfo.params.timerRunning) {
        timeLeft = cardInfo.params.timerStartValue + cardInfo.params.timerLength - Date.now();
        if (timeLeft <= cardInfo.params.timerArmOffset && !cardInfo.params.timerArmed) {
          // arm the timer
          armTimer();
        }
        if (timeLeft <= 0) {
            stopTimer();
            expireTimer();
            timeLeft = 0;
        }
    } else {
        timeLeft = cardInfo.params.timerLength;
    }
    timeLeft = parseInt(timeLeft/1000);

    updateDisplay(timeLeft);
    return;
}

// button functions
function startTimer () {
    cardInfo.params.timerStartValue = Date.now();
    cardInfo.params.timerRunning = true;
    cardInfo.params.timerExpired = false;
    cardInfo.params.timerArmed = false;
}

function stopTimer () {
    // stop the timer
    cardInfo.params.timerRunning = false;
    // reset the timer
    cardInfo.params.timerExpired = false;
    // updateDisplay();
}

function armTimer () {
  publish('/audio', JSON.stringify({
    cmd:'source',
    value:'usb'
  }))
  cardInfo.params.timerArmed = true;
}

function incrementTimer () {
    // increase 1 min
    cardInfo.params.timerLength = Math.min(cardInfo.params.timerLength + 1 * 60 * 1000, 60*60*1000);
    // updateDisplay();
}

function decrementTimer () {
    // decrease 1 min
    cardInfo.params.timerLength = Math.max(cardInfo.params.timerLength - 1 * 60 * 1000, 0);
    // updateDisplay();
}

function expireTimer () {
    cardInfo.params.timerExpired = true;
    publish('/audio', JSON.stringify({
        cmd: 'play',
        value: cardInfo.params.expireSound
    }))
}

// functions to be passed to runTimer function
function updateDisplay(timeLeft) {
    timeLeft = timeLeft || 0;

    var seconds = timeLeft % 60;
    var minutes = (timeLeft - seconds)/60
    var timerString = zeroPad(minutes, 2) + ':' + zeroPad(seconds, 2)

    if (timerString !== cardInfo.params.lastUpdate) {
        var updateObj = generateUpdateCardObj(cardInfo.id);
        updateObj.elements.push(generateElementUpdate(
                                    elementId.timerValue,
                                    timerString)
                                );

        updateCard(JSON.stringify(updateObj));

        cardInfo.params.lastUpdate = timerString;
    }
}

// card functions
function createCard () {
    var cardObj = generateNewCardObj(cardInfo.bgColor, cardInfo.responseTopic);
    // background and buttons
    cardObj.elements.push(generateImageElement(
                            elementId.background,
                            generateImgPath(imgRootPath, timerImg['background']),
                            2, 46)
                        );
    cardObj.elements.push(generateImageElement(
                            elementId.buttons,
                            generateImgPath(imgRootPath, timerImg['buttons']),
                            21, 10)
                        );
    // timer start and timer value
    // cardObj.elements.push(generateTextElement(
    //                         elementId.timerStartValue,
    //                         cardInfo.params.timerStartValue,
    //                         23,
    //                         0, 103, 'center')
    //                     );
    cardObj.elements.push(generateTextElement(
                            elementId.timerValue,
                            "00:00",
                            82,
                            0, 131, 'center')
                        );
    // TODO: add a "Remaining" text element?

    // init the card
    initCard(JSON.stringify(cardObj), true);
}

function readConfig() {
    readFile('/etc/config.json', '', function (err, data) {
        if (!err) {
            var config;
    	    try {
    	        config = JSON.parse(data);
    	    } catch(e) {
    	        print(e); // error in the above string!
    	        return null;
    	    }

    	    // apply the settings from the config file
          if(config.cards['1'] && config.cards['1'].enabled !== undefined){
            cardInfo.enabled = config.cards['1'].enabled && cardInfo.enabled;
          }
        }
        print("card is enabled: " + cardInfo.enabled);
    });
}

// base functions
function setup() {
    readConfig();
    if(cardInfo.enabled == true){
      cardInfo.params.timerStartValue = Date.now()

      connect('localhost', 1883, null, function () {
          subscribe('/cardResponse', function () {
              createCard();
          });
          subscribe('/button');
          subscribe('/gesture');
          subscribe('/test/+')
        },
        null,
        '/card',
        JSON.stringify({
          cmd: 'remove_card',
          cardName: cardInfo.responseTopic
        })
      );
    }
}

function loop() {
    // exit program if card is invalid
    if (cardInfo.invalid === true) return -1
    
    if (cardInfo.id >= 0) {
        tick();
    }
}

function onCreationSuccess () {

}

function setActive(bActive) {
    if (bActive) {
        cardInfo.active = true;
    }
}

function onInput(e) {
    if (typeof e.source !== 'undefined' && typeof e.payload !== 'undefined' ) {
        print('input! input source: ' + e.source);
        if (    e.source === 'button' &&
                e.payload.action === 'press' &&
                cardInfo.active === true
            )
        {
            switch(e.payload.id) {
                case 0:
                case '0':
                    stopTimer();
                    break;
                case 1:
                case '1':
                    startTimer();
                    break;
                case 2:
                case '2':
                    decrementTimer();
                    break;
                case 3:
                case '3':
                    incrementTimer();
                    break;
                default:
                    break;
            }
        }
    }
}

function onMessage(e) {
    if (typeof e.topic !== 'undefined' && typeof e.payload !== 'undefined' ) {
        print('message! topic: ' + e.topic + ', value: ' + e.payload);
        switch (e.topic) {
            case '/cardResponse':
                cardInfo = handleCardResponseMessage(cardInfo, e.payload);
                break;
            case '/test/set3':
                print('setting timerLength to 3000');
                cardInfo.params.timerLength = 3 * 1000;
                break;
            default:
                break;
        }
    }
}
