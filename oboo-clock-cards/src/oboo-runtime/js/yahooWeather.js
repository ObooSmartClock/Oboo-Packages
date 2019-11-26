
function convertTemp (temp, unit) {
	if (unit === 'fahrenheit') {
		return parseInt((temp - 273.15) * 9 / 5 + 32)
	} else {
		return parseInt(temp - 273.15)
	}
}

function convertSpeed (speed, unit) {
	speed = speed * 3.6 // m/s -> km/h
	print(speed)
	if (unit === 'metric') {
		return parseInt(speed)
	} else {
		return  parseInt(speed / 1.60934) // km/h -> mph
	}
}

function getYahooWeather (location, tempUnit, distanceUnit) {
	if (tempUnit === undefined) tempUnit = 'celsius'
	if (distanceUnit === undefined) distanceUnit = 'metric'

    print('getting weather');
    var result = httpRequest({
        method: 'GET',
        url: 'https://api.getoboo.com/v1/weather?q=' + encodeURIComponent(location)
    });

	if (result) {
	    var jsonResult;
	    try {
	        jsonResult = JSON.parse(result);
	    } catch(e) {
	        print(e); // error in the above string!
	        return null;
	    }

	    print(JSON.stringify(jsonResult))

	    var weatherObj = {
	        'temperature': convertTemp(jsonResult.main.temp, tempUnit),
	        'condition': jsonResult.weather[0].icon,
	        'wind': convertSpeed(jsonResult.wind.speed, distanceUnit),
	        'secondary': [
	        	{
	        		type: 'wind-speed',
	        		unit: (distanceUnit === 'metric' ? 'km/h' : 'mph'),
	        		value: convertSpeed(jsonResult.wind.speed, distanceUnit)
	        	},
	        	{
	        		type: 'humidity',
	        		unit: '%',
	        		value: jsonResult.main.humidity
	        	},
	        	{
	        		type: 'pressure',
	        		unit: 'hPa',
	        		value: jsonResult.main.pressure
	        	}
	        ]
	    }

	    return weatherObj;
	}
	else {
		return null;
	}
}