#!/bin/sh

TMP="/tmp/iwstatus"
OUTPUT="/tmp/wifi-active"

iwconfig > $TMP
status=$(grep apcli0 $TMP | sed -e 's/.*"\(.*\)"/\1/')

echo "status is .${status}."
if [ "$status" != "" ] && [ "$status" != "  " ]; then
        echo "1"
        echo "1" > $OUTPUT
else
        echo "0"
        rm -rf $OUTPUT
fi
