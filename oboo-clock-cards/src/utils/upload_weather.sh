#!/bin/sh

if [ "$1" == "" ]; then
	echo "ERROR: expecting address to upload"
	exit
fi

rsync -va weather-card/weather.js root@$1:/usr/bin/js/weather.js
rsync -va oboo-runtime/js/weatherAPI.js root@$1:/usr/bin/js/modules/weatherAPI.js
rsync -va weather-card/img/bin/ root@$1:/usr/bin/img/
