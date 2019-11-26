#!/bin/sh

if [ "$1" == "" ]; then
        echo "ERROR: expecting address to upload"
        exit
fi
DEVICE="$1"

rsync -va audio-manager/audio.js root@$DEVICE:/usr/bin/js/audio.js
ssh -t root@$DEVICE '/usr/bin/ort /usr/bin/js/audio.js'
