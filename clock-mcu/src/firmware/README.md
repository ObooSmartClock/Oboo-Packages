# Oboo Smart Clock MCU Firmware

Firmware for the ATmega micro controller on the Oboo Smart Clock. Interacts with the following:

* RTC
* 7-Segment Display (via MAX7219)
* Gesture & Brightness Sensor
* Touch Input
* WS2818 Addressable LEDs
* Battery Level Monitoring

## Build Instructions

To release:

```
make clean
make
cp build-uno/firmware.hex bin/atmega328p/
```

Also need to update `version` file to hold latest version number. Device uses this to check for firmware updates.


## TODO List

* Switch to using generic MCU-to-Omega serial communication function (sendCommand in `protocol.h`)
* ~~Implement Touch detection~~
	* ~~Interrupt servicing routine, reading touch values, communication protocol~~
* Battery Level Monitoring
    * ~~Implement code~~
    * Test with new hardware
* Debug and improve Up/Down gesture detection algorithm
* _Optimize binary size for smaller mcu_
