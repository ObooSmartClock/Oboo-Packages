#! /bin/sh

## run the oboo demo continuously

sleep 15
sh /usr/bin/demo.sh s
sleep 5
sh /usr/bin/demo.sh r
sleep 5

while [ 1 ]; do
        sh /usr/bin/demo.sh
        sleep 2
        sh /usr/bin/demo.sh r
        sleep 10
done