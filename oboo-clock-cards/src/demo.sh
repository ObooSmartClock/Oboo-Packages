#! /bin/sh

## run a demo
echo "> starting demo!"

setup () {
        sh /usr/bin/weather.sh setup
        sh /usr/bin/timer.sh setup
        # sh /usr/bin/map.sh setup
        sh /usr/bin/status.sh wifi 1
        sh /usr/bin/status.sh battery 2
        sh /usr/bin/status.sh bluetooth 1
}

set_timer () {
        sh /usr/bin/timer.sh update 1 "13:52"
        sh /usr/bin/mcu/setButton.sh 0 ff0000
        sh /usr/bin/mcu/setButton.sh 1 00ff00
        sh /usr/bin/mcu/setButton.sh 2 0000ff
        sh /usr/bin/mcu/setButton.sh 3 88ffff
        sh /usr/bin/select.sh right
        sleep 1
        sh /usr/bin/timer.sh update 1 "13:51"
        sleep 1
        sh /usr/bin/timer.sh update 1 "13:50"
        sleep 1
        sh /usr/bin/timer.sh update 1 "13:49"
        sleep 1
        sh /usr/bin/timer.sh update 1 "13:48"
        sleep 1
        sh /usr/bin/timer.sh update 1 "13:47"
}

set_map () {
        sh /usr/bin/mcu/setColor.sh 0
        sh /usr/bin/select.sh right
}

set_reset () {
#       sh /usr/bin/mcu/setColor.sh 9b4b63
        sh /usr/bin/mcu/setColor.sh 0
        sh /usr/bin/select.sh first
}

if [ "$1" == "r" ]; then
        set_reset
elif [ "$1" == "s" ]; then
        setup
elif [ "$1" == "t" ]; then
        set_timer
else
        set_reset
        sleep 4

        set_timer
        sleep 1

        #set_map
        set_reset
        sleep 2
fi
