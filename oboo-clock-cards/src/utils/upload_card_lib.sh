#!/bin/sh

if [ "$1" == "" ]; then
	echo "ERROR: expecting address to upload"
	exit
fi

rsync -va oboo-runtime/js/card-lib.js root@$1:/usr/bin/js/card-lib.js
#rsync -va oboo-runtime/js/yahooWeather.js root@$1:/usr/bin/js/yahooWeather.js
#rsync -va weather-card/img/bin/ root@$1:/usr/bin/img/
