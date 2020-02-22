#!/bin/sh

if [ "$1" == "" ]; then
	echo "ERROR: expecting address to upload"
	exit
fi

rsync -va oboo-runtime/js/card-lib.js root@$1:/usr/bin/js/card-lib.js
