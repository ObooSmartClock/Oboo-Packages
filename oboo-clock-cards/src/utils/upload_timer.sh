#!/bin/sh

if [ "$1" == "" ]; then
	echo "ERROR: expecting address to upload"
	exit
fi

rsync -va timer-card/timer.js root@$1:/usr/bin/js/timer.js
rsync -va timer-card/img/bin/ root@$1:/usr/bin/img/
