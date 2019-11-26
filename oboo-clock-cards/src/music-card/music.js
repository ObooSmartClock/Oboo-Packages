var cardInfo = {
    id: -1,
    active: false,
    responseTopic: '/musicCard_' + getEpochMillis(),
    bgColor: -1,
    nightlight: [
        0x4eb96f,
        0xc44569,
        0xf5cd79,
        0x546de5
    ],
    params: {
    },
    enabled: true
}

// definitions for the program
var elementId = {
    background: 0,
    buttons: 1,
};

var cardImg = {
    background: "music_bg",
    buttons: "music_buttons",
};

// card functions
function createCard () {
    var cardObj = generateNewCardObj(cardInfo.bgColor, cardInfo.responseTopic);
    // background and buttons
    cardObj.elements.push(generateImageElement(
                            elementId.background,
                            generateImgPath(imgRootPath, cardImg['background']),
                            20, 80)
                        );
    cardObj.elements.push(generateImageElement(
                            elementId.buttons,
                            generateImgPath(imgRootPath, cardImg['buttons']),
                            12, 16)
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
          if(config.cards['2'] && config.cards['2'].enabled !== undefined){
            cardInfo.enabled = config.cards['2'].enabled && cardInfo.enabled;
          }
        }
        print("card is enabled: " + cardInfo.enabled);
    });
}

// base functions
function setup() {
    readConfig();
    if(cardInfo.enabled == true){
      connect('localhost', 1883, null, function () {
          subscribe('/cardResponse', function () {
              createCard();
          });
          subscribe('/button');
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
  //do nothing
}

function volumeUp () {
    publish('/audio', JSON.stringify({cmd: 'volUp'}))
}

function volumeDown () {
    publish('/audio', JSON.stringify({cmd: 'volDown'}))
}

function playPause () {
    publish('/audio', JSON.stringify({cmd: 'playPause'}))
}

function nextTrack () {
    publish('/audio', JSON.stringify({cmd: 'nextTrack'}))
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
                    playPause();
                    break;
                case 1:
                case '1':
                    nextTrack();
                    break;
                case 2:
                case '2':
                    volumeDown();
                    break;
                case 3:
                case '3':
                    volumeUp();
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
            default:
                break;
        }
    }
}
