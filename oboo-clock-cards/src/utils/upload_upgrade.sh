#!/bin/sh

if [ "$1" == "" ]; then
	echo "ERROR: expecting address to upload"
	exit
fi

rsync -va upgrade-card/upgrade.js root@$1:/usr/bin/js/upgrade.js
rsync -va upgrade-card/img/bin/ root@$1:/usr/bin/img/
