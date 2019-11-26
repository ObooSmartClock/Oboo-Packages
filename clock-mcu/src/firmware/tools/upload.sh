#!/bin/sh

OBOOADDR='oboo-clock-56be.local'

scp ./build-uno/firmware.hex root@$OBOOADDR:/tmp

ssh root@$OBOOADDR sh /usr/bin/arduino-dock flash /tmp/firmware.hex
