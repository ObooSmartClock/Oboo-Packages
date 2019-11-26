#!/bin/sh

if [ "$1" == "" ]; then
	echo "ERROR: expecting address to upload"
	exit
fi

rsync -va calendar-card/calendar.js root@$1:/usr/bin/js/calendar.js
