# oboo-clock-mcu
Microcontroller firmware for Oboo Smart Clock


## Pre-requisites

To compile microcontroller binary on Mac OS

* Install Arduino IDE version 1.5.0 or higher
* Install the [`arduino-mk` package (Arduino Makefile)](https://github.com/sudar/Arduino-Makefile) with brew


## Building Firmware

* Go to `firmware` directory
* Run `make`
* Compiled file will be at: `build-uno/firmware.hex`

## Compiling Firmware and Flashing it to the Oboo Microcontroller

Locally compiled microcontroller binaries can be easily flashed to the Oboo Microcontroller.

* Make sure Oboo is connected to the same WiFi network as the development computer
* Go to `firmware` directory
* Run `sh test.sh <OBOO ADDRESS>` where `<OBOO ADDRESS>` is the Oboo's IP address on the network, or its mDNS name

This will perform a clean compile of the microcontroller firmware, transfer the binary to the Oboo's filesystem, and flash it to the Oboo's microcontroller.

Examples:
* Using IP address: `sh test.sh 192.168.1.20`
* Using mDNS name: `sh test.sh oboo-clock-70cb.local`

## Restoring the Official Microcontroller Firmware

Re-flash the Oboo's existing microcontroller by running `sh /usr/bin/mcu/flash.sh` on the Oboo.

This *should* work even if you've uploaded a custom firmware that causes issues.
