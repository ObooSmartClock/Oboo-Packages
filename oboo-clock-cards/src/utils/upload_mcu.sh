#!/bin/sh

if [ "$1" == "" ]; then
	echo "ERROR: expecting address to upload"
	exit
fi

rsync -va input-processor/mcu.js root@$1:/usr/bin/js/mcu.js
