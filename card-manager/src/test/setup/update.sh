#! /bin/sh

## script to pull latest card-manager-ipk from online and install it

url="$1"
if [ "$url" == "" ]; then
        echo "Error: need URL"
        exit
fi

cd /tmp
rm -rf *.ipk
wget --no-check-certificate $url

opkg remove --force-depends oboo-clock-card-manager

# TODO: need to update version numbers
opkg install oboo-clock-card-manager-0.2.6-1-mipsel-24kc.ipk

/etc/init.d/card-manager stop