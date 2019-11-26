#!/bin/sh

## update status bar based on wifi connection status
FILE="/tmp/wifi-active"

if [ "$ACTION" = "ifup" ]; then
	if [ "$DEVICE" = "apcli0" ]; then
		mosquitto_pub -t '/status' -m '{ "cmd":"update", "elements": [ { "type": "wifi", "value": 1 } ] }'
		echo "1" > $FILE
	fi
elif [ "$ACTION" = "ifdown" ]; then
	# TODO: potentially change this device, sometimes shows just "" when if goes down
	# if [ "$DEVICE" = "apcli0" ]; then
	# 	mosquitto_pub -t '/status' -m '{ "cmd":"update", "elements": [ { "type": "wifi", "value": 0 } ] }'
	# fi
	
	# note: device is generally blank when wifi goes down
	mosquitto_pub -t '/status' -m '{ "cmd":"update", "elements": [ { "type": "wifi", "value": 0 } ] }'
	rm -rf $FILE
fi
