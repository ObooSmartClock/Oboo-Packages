#!/bin/sh

if [ "$1" == "" ]; then
	echo "ERROR: expecting address to upload"
	exit
fi

rsync -va music-card/music.js root@$1:/usr/bin/js/music.js
rsync -va music-card/img/bin/ root@$1:/usr/bin/img/
