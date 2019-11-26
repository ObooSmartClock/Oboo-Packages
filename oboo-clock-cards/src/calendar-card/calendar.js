var dateLast = new Date();
var day = dateLast.getDate();
var cardInfo = {
    id: -1,
    responseTopic: '/calendarCard_' + getEpochMillis(),
    //responseTopic: '/calendarCard',
    // bgColor: 0xc44569,
    bgColor: 0x0,
    "elements": [
        {
            "id": 0,
            "type": "calendar",
            "position": {
                "x": 60,
                "y": 70
            },
            "size": 3,
            "value": "hello",
            "date": {
              "year": dateLast.getYear(),
              "month": dateLast.getMonth()+1,
              "day": dateLast.getDate()
            }
        }
    ],
    enabled: true
}

var elementId = {
    day: 0,
    month: 1,
    year: 2,
    date: 3,
    calendar: 4,
    event: 5,
    cover: 6,
    separator: 7
};

function createCard () {
    print("createCard");
    var cardObj = generateNewCardObj(cardInfo.bgColor, cardInfo.responseTopic);

    //elementId.calendar
    cardObj.elements.push(generateElementCal(elementId.calendar, 'calendar',
                                            40, //size
                                            5, //posX
                                            0, //posY
                                            'center'  //alignment
                                          )
                                  );
    // init the card
    initCard(JSON.stringify(cardObj), true);
}

function readConfig() {
    print("readConfig");
    readFile('/etc/config.json', '', function (err, data) {
        if (!err) {
            var config;
    	    try {
    	        config = JSON.parse(data);
    	    } catch(e) {
    	        print(e); // error in the above string!
    	        return null;
    	    }

          if(config.cards['3'] && config.cards['3'].enabled !== undefined){
            cardInfo.enabled = config.cards['3'].enabled && cardInfo.enabled;
          }
        }
        print("card is enabled: " + cardInfo.enabled);
    });
}

function setup() {
    readConfig();
    if(cardInfo.enabled == true){
      connect('localhost', 1883, null, function () {
          subscribe('/cardResponse', function () {
              createCard();
          });
          subscribe('/config/update');
          // subscribe('/button');
          // subscribe('/gesture');
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

function updateCalendar () {
//    print("updateCalendar");
    var updateObj = generateUpdateCardObj(cardInfo.id);

    updateObj.elements.push(generateDate());

    updateCard(JSON.stringify(updateObj));
    return true;
}

function loop() {
    // return if card not initialized
    if (cardInfo.id < 0) return
    // exit program if card is invalid
    if (cardInfo.invalid === true) return -1

    dateLocal = new Date();

    //print("today is: " + day + " of month")

    if(dateLocal.getDate() != day){
       updateCalendar();
       day = dateLocal.getDate();
    }
}

function onMessage(e) {
        print("onmessage");
    if (typeof e.topic !== 'undefined' && typeof e.payload !== 'undefined' ) {
        print('message! topic: ' + e.topic + ', value: ' + e.payload);
        switch (e.topic) {
            case '/cardResponse':
                print("handle card on response");
                cardInfo = handleCardResponseMessage(cardInfo, e.payload);
                break;
            case '/config/update':
                readConfig();
                break;
            default:
                break;
        }
    }
}
