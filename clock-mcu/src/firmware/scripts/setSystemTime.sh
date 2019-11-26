#!/bin/sh

## Set the linux system time based on epoch from MCU

if [ "$1" == "" ]; then
	exit
fi

mcuEpoch="$1"
# convert epoch from mcu to local time string
timezone=`date +%Z`
timeString=`TZ=$timezone date +"%F %T" -d @$mcuEpoch`

date -s "$timeString"
