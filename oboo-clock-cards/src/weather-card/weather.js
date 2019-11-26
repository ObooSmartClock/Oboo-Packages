var cardInfo = {
    id: -1,
    responseTopic: '/weatherCard_' + getEpochMillis(),
    // bgColor: 0xc44569,
    bgColor: 0x0,
    params: {
        location: 'Toronto,CA',
        tempUnit: 'fahrenheit',
        distanceUnit: 'imperial',
        prevCalendarDay: -1,
        prevUpdate: 0,
        updateInterval: 15*60*1000 // 15 min
    },
    enabled: true
}

// definitions for the program
var elementId = {
    weatherUnitIndicator: 0,
    weatherTemperature: 1,
    weatherMainImage: 2,
    date: 3,
    weatherParameterIcon: 4,
    weatherParameterValue: 5,
    weatherParameterUnits: 6,
    separator: 7,
    cover: 8
}

var weatherImg = {
    degree: "degree",
    degreeC: "degC",
    degreeF: "degF",
    oboo: "oboo",

    '01d': 'sunny',
    '01n': 'night-clear',

    '02d': 'cloudy',
    '02n': 'night',

    '03d': 'cloudy-1',
    '03n': 'cloudy-1',

    '04d': 'cloudy-1',
    '04n': 'cloudy-1',

    '09d': 'rain',
    '09n': 'rain',

    '10d': 'rainbow-1',
    '10n': 'night-1',

    '11d': 'thunderstorm',
    '11n': 'thunderstorm',

    '13d': 'snowy',
    '13n': 'snowy',

    '50d': 'fog-1',
    '50n': 'fog',

    'wind-speed': 'wind-sign',
    'humidity': 'humidity',
    'pressure': 'pressure',

    separator: "line",
}

function getFormattedDate () {
    var weekdays = [ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]

    var monthNames = ["Jan", "Feb", "Mar", "Apr", "May", "June",
    "July", "Aug", "Sept", "Oct", "Nov", "Dec"
    ]

    var date = new Date()

    return {
        weekday: weekdays[date.getDay()],
        month: monthNames[date.getMonth()],
        day: date.getDate()
    }
}


function createCard () {
    var dateObj = getFormattedDate()
    var cardObj = generateNewCardObj(cardInfo.bgColor, cardInfo.responseTopic)

    cardObj.elements.push(generateImageElement(
        elementId.weatherUnitIndicator,
        generateImgPath(imgRootPath, weatherImg['degree']),
                            200, 50) )

    cardObj.elements.push(generateTextElement(
        elementId.weatherTemperature,
        '--',
        82,
        -40, 40, 'right') )

    // main weather image
    cardObj.elements.push(generateImageElement(
        elementId.weatherMainImage,
        generateImgPath(imgRootPath, weatherImg['oboo']),
                            // generateImgPath(imgRootPath, weatherImg['sunny']),
                            10, 5)
    )
    // date
    cardObj.elements.push(generateTextElement(
        elementId.date,
        dateObj.weekday + ', ' + dateObj.month + ' ' + dateObj.day,
        23,
        0, 160, 'center')
    )
    cardObj.elements.push(generateImageElement(
        elementId.weatherParameterIcon,
        generateImgPath(imgRootPath, weatherImg['wind-speed']),
        30, 205)
    )
    cardObj.elements.push(generateTextElement(
        elementId.weatherParameterValue,
        '--',
        50,
        -64, 209, 'right')
    )
    cardObj.elements.push(generateTextElement(
        elementId.weatherParameterUnits,
        'km/h',
        23,
        180, 233, 'left')
    )
    // line separator
    cardObj.elements.push(generateImageElement(
        elementId.separator,
        generateImgPath(imgRootPath, weatherImg['separator']),
        21, 190)
    )

    // init the card
    initCard(JSON.stringify(cardObj), true)
}

var cycleCounter = 100
function cycleInfo () {
    cycleCounter += 1
    if (cycleCounter >= 100) {
        cycleCounter = 0
        return true
    } else {
        cycleCounter += 1
        return false
    }
}


var weather
function updateWeather () {
    var result = getYahooWeather(cardInfo.params.location, cardInfo.params.tempUnit, cardInfo.params.distanceUnit)

    if (result !== null) {
        weather = result
        updateDisplay(true)
        return true
    } else {
        return false
    }

}


var secIndex = 0
function updateDisplay (forceUpdate) {

    var updateObj = generateUpdateCardObj(cardInfo.id)
    var update = forceUpdate?true:false

    /* Date */
    var dateObj = getFormattedDate()
    // only update if the calendar day has changed
    if (cardInfo.params.prevCalendarDay !== dateObj.day) {

        updateObj.elements.push(generateElementUpdate(
            elementId.date,
            dateObj.weekday + ', ' + dateObj.month + ' ' + dateObj.day)
        )

        // update the prev day variable
        cardInfo.params.prevCalendarDay = dateObj.day

        update = true
    }

    /* Weather */
    if (weather !== null) {
        // update Main info
        updateObj.elements.push(generateElementUpdate(
            elementId.weatherTemperature,
            weather.temperature)
        )
        
        updateObj.elements.push(generateElementUpdate(
            elementId.weatherMainImage,
            generateImgPath(imgRootPath, weatherImg[weather.condition])
            )
        )

        updateObj.elements.push(generateElementUpdate(
            elementId.weatherUnitIndicator,
            generateImgPath(imgRootPath, weatherImg[cardInfo.params.tempUnit === 'fahrenheit'?'degreeF':'degreeC'])
            )
        )

        // update Secondary info
        if (cycleInfo()) {
            secIndex += 1
            secIndex %=   weather.secondary.length
            update = true
        }

        var secInfo = weather.secondary[secIndex]

        updateObj.elements.push(generateElementUpdate(
            elementId.weatherParameterIcon,
            generateImgPath(imgRootPath, weatherImg[secInfo.type]))
        )

        updateObj.elements.push(generateElementUpdate(
            elementId.weatherParameterValue,
            secInfo.value)
        )
        updateObj.elements.push(generateElementUpdate(
            elementId.weatherParameterUnits,
            secInfo.unit)
        )
        
    }
    if (update) {
        updateCard(JSON.stringify(updateObj))
    }
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
    	    cardIdentifier  = 0;    // TODO: this is temporary
    	    cardInfo.params.location        = config.cards['0'].location || cardInfo.params.location;
    	    cardInfo.params.tempUnit        = config.cards['0'].tempUnit || cardInfo.params.tempUnit;
    	    cardInfo.params.distanceUnit    = config.cards['0'].distanceUnit || cardInfo.params.distanceUnit;
          print(config.cards['0'].distanceUnit);
          if(config.cards['0'] && config.cards['0'].enabled !== undefined){
            cardInfo.enabled = config.cards['0'].enabled && cardInfo.enabled;
          }
        }
        print("card is enabled: " + cardInfo.enabled);
        print('configuration: location = ' + cardInfo.params.location + '; temperature unit = ' + cardInfo.params.tempUnit + '; distance unit = ' + cardInfo.params.distanceUnit);
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

function loop() {
    // return if card not initialized
    if (cardInfo.id < 0) return
    // exit program if card is invalid
    if (cardInfo.invalid === true) return -1

    // update weather in every  {{updateInterval}}
    if ((new Date - cardInfo.params.prevUpdate) > cardInfo.params.updateInterval) {
        if ( updateWeather() ){
            // reset time stamp if successful
            cardInfo.params.prevUpdate = new Date
        } else {
            // check back in 1 minute
            cardInfo.params.prevUpdate = new Date - (cardInfo.params.updateInterval - 1 * 60 * 1000)
        }
    }

    // update display every loop
    updateDisplay()
}

function onInput(e) {
    if (typeof e.source !== 'undefined' && typeof e.payload !== 'undefined' ) {
        print('input! input source: ' + e.source + ', value: ' + e.payload)
    }
}

function onMessage(e) {
    if (typeof e.topic !== 'undefined' && typeof e.payload !== 'undefined' ) {
        print('message! topic: ' + e.topic + ', value: ' + e.payload)
        switch (e.topic) {
            case '/cardResponse':
              cardInfo = handleCardResponseMessage(cardInfo, e.payload)
              break
            case '/config/update':
              readConfig()
              cardInfo.params.prevUpdate = 0 // force an update
              break
            default:
              break
        }
    }
}
