#! /bin/sh

## script to pull latest binary from online and make it executable

url="$1"
if [ "$url" == "" ]; then
        echo "Error: need URL"
        exit
fi

rm -rf card-manager
wget --no-check-certificate  $url
chmod +x card-manager

