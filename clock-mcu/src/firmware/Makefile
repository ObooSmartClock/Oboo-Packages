# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile
# # WARNING: Trailing whitespace will cause options to be read incorrectly.

## Makefile Setup
ARDUINO_DIR   = /Applications/Arduino.app/Contents/Java
ARDMK_DIR     = /usr/local/Cellar/arduino-mk/1.5.2

# define board
#ARCHITECTURE = avr
#BOARD_TAG = atmega88_16
BOARD_TAG = uno

# Default user-defined libraries directory
USER_LIB_PATH += libraries

# Additional user-defined libraries may be added to this list
ARDUINO_LIBS += \
	EEPROM \
	Wire \
	RTClib \
	LedControl \
	TM1618 \
	SparkFun_APDS9960 \
	APDS9960 \
	Adafruit_NeoPixel


### Do not touch - the path to Arduino.mk, inside the ARDMK_DIR
include $(ARDMK_DIR)/Arduino.mk
