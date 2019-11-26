#! /bin/sh

## run a demo
echo "> starting demo!"

setup () {
        sh /root/weather.sh setup
        sh /root/timer.sh setup
        sh /root/map.sh setup
        sh /root/status.sh wifi 1
        sh /root/status.sh battery 2
        sh /root/status.sh bluetooth 1
}

set_timer () {
        sh timer.sh update 1 "13:52"
        sh /root/mcu/setButton.sh 0 ff0000
        sh /root/mcu/setButton.sh 1 00ff00
        sh /root/mcu/setButton.sh 2 0000ff
        sh /root/mcu/setButton.sh 3 88ffff
        sh /root/select.sh right
        sleep 1
        sh timer.sh update 1 "13:51"
        sleep 1
        sh timer.sh update 1 "13:50"
        sleep 1
        sh timer.sh update 1 "13:49"
        sleep 1
        sh timer.sh update 1 "13:48"
        sleep 1
        sh timer.sh update 1 "13:47"
}

set_map () {
        sh /root/mcu/setColor.sh 0
        sh /root/select.sh right
}

set_reset () {
#       sh /root/mcu/setColor.sh 9b4b63
        sh /root/mcu/setColor.sh 0
        sh /root/select.sh first
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

        set_map
        sleep 2
fi
