#!/bin/sh

cd /tmp

rm -rf *.ipk
wget --no-check-certificate $1
opkg remove --force-depends oboo-clock-cards
opkg install --force-reinstall oboo-clock-cards-0.2.6-2-mipsel-24kc.ipk
/etc/init.d/oboo-cards restart

cd -
