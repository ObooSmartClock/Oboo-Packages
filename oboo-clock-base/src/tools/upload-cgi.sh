#!/bin/sh

if [ "$1" == "" ]; then
	echo "ERROR: expecting destination address"
	exit 1
fi

rsync -va scripts/www/cgi-bin/ root@$1:/www/cgi-bin/
