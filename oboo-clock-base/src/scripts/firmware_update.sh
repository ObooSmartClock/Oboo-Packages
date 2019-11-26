#!/bin/sh

# include the json sh library
. /usr/share/libubox/jshn.sh
json_init

PACKAGE_NAME="oboo-clock-base"

MAC=`cat /sys/class/net/ra0/address`
PACKAGE_VER=`opkg info $PACKAGE_NAME | grep Version | sed -e 's/^Version:\s//g'`
MODEL=`ubus call system board | grep model | sed -e 's/:"(.*?)"/\1/p'`

DATA=`wget -qO- https://api.getoboo.com/v1/firmware/update?mac=$MAC`

# Parse JSON content
json_load "$DATA"
json_get_var update_available update_available
json_get_var mac mac
json_get_var package_version package_version


doUpdate () {
    opkg update
    opkg upgrade --force-overwrite $PACKAGE_NAME
    sh /usr/share/oboo/oboo-sw-upgrade.sh
}

if [ $update_available ] && [ "$PACKAGE_VER" != "$package_version" ]; then
    echo "update available"
    doUpdate
else
    echo "no update available"
fi;
