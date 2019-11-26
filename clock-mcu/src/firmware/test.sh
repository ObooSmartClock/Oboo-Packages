#!/bin/sh

DEVICE=""

if [ "$1" == "" ]; then
    echo "ERROR: expecting Oboo IP address or mDNS name"
    exit 1
fi
DEVICE="$1"

make clean
make

if [ $? == 0 ]; then
	rsync -va build-uno/firmware.hex root@${DEVICE}:/root/firmware.hex
	ssh -t root@$DEVICE 'sh /usr/bin/arduino-dock flash /root/firmware.hex'
else
	echo ""
	echo "ERROR: build failed!"
fi
