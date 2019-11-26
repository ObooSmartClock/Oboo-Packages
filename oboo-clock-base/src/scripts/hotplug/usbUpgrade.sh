#!/bin/#!/bin/sh

## check for binary file on USB drive, if it exists, install it

binFile="oboo.bin"
logFile="upgrade.log"
usbPath="/tmp/mounts/USB-A1"
tmpPath="/tmp"

if [ "${ACTION}" = "add" ]; then
        sleep 4

        MAC=`cat /sys/class/net/ra0/address`
        if grep -Fxq "$MAC" $usbPath/$logFile
        then
            #echo "Device exist, do not upgrade" >> $usbPath/$logFile
            exit 0
        else
            echo $MAC >> $usbPath/$logFile
        fi

        #echo "Continue with FW upgrade !" >> $usbPath/$logFile

        if [ -e $usbPath/$binFile ]; then
                # update the display
                cat /usr/bin/img/img_upgrade.bin > /dev/fb0
                # copy the binary file
                cp $usbPath/$binFile $tmpPath/$binFile
                # start the upgrade
                sysupgrade -n -v $tmpPath/$binFile &
        fi
fi
