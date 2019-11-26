#!/bin/sh

## script to run when Oboo configuration is updated

# set the timezone
tz=$(cat /etc/config.json | jsonfilter -e '@.config.tz')
timezone=$(cat /etc/config.json | jsonfilter -e '@.config.timezone')
onion time set $tz $timezone
sh /usr/bin/mcu/setMcuTime.sh

# let all listeners know to recheck the config file
mosquitto_pub -t '/config/update' -m '{"update":true}'
