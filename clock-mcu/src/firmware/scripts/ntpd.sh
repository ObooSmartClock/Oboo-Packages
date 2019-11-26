#!/bin/sh

#logger -t DEBUG "hotplug ntpd: action='$ACTION'"

[ "$ACTION" = step ] || exit 0

sh /usr/bin/mcu/setMcuTime.sh -force
logger -t DEBUG "hotplug ntpd: setting coprocessor time - action='$ACTION' date=`date`"
