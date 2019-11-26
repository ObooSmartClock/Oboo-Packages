#!/bin/sh

if [ "$1" == "" ]; then
	echo "ERROR: expecting address to upload"
	exit
fi

rsync -va config-update/updateConfig.js root@$1:/usr/bin/js/updateConfig.js
