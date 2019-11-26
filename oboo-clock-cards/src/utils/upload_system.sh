#!/bin/sh

if [ "$1" == "" ]; then
	echo "ERROR: expecting address to upload"
	exit
fi
DEVICE="$1"

rsync -va system-card/system.js root@$DEVICE.local:/usr/bin/js/system.js
rsync -va system-card/img/bin/ root@$1:/usr/bin/img/
ssh -t root@$DEVICE.local '/usr/bin/ort /usr/bin/js/system.js'
