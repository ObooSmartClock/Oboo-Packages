#!/bin/sh

## Set the MCU RTC to the current linux system time
##      run with -force argument to communicate directly to MCU

# format current system time to a string
currentTime=`date +"%F %T"`
# create an epoch time and account for current timezone (MCU has no concept of timezones)
epochTime=`TZ=GMT0 date +%s -d "$currentTime"`

if [ "$1" == "-force" ]; then
	echo "~T$epochTime;" > /dev/ttyS1
else
	# publish to mosquitto topic
	mosquitto_pub -t "/set" -m "{\"cmd\":\"rtc\",\"value\":\"$epochTime\"}"
fi
