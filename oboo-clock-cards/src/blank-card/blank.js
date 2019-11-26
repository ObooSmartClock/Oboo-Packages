var cardInfo = {
    id: -1,
    active: false,
    responseTopic: '/blankCard_' + getEpochMillis(),
    bgColor: -1,
    nightlight: [
        0x4eb96f,
        0xc44569,
        0xf5cd79,
        0x546de5
    ],
    params: {
    }
}

// definitions for the program
var elementId = {
    background: 0,
    buttons: 1,
};

var cardImg = {
    background: "blank_bg",
    buttons: "blank_buttons",
}; 

// card functions
function createCard () {
    //var cardObj = generateNewCardObj(cardInfo.bgColor, cardInfo.responseTopic);
    
    // init the card
    //initCard(JSON.stringify(cardObj));
}

// base functions
function setup() {    
    connect('localhost', 1883, null, function () {
        subscribe('/cardResponse', function () {
            createCard();
        });
        subscribe('/button');
    });
}

function loop() {
  //do nothing
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
                    break;
                case 1:
                case '1':
                    break;
                case 2:
                case '2':
                    break;
                case 3:
                case '3':
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
