
#define APDS9960_INT_PIN	2
#define APDS9960_ISR_NUM 	0

// gesture sensor object
#if GESTURE_ENABLED && GESTURE_SPARKFUN_ENABLED

#include "SparkFun_APDS9960.h"
#define GESTURE_INTERRUPT 1
SparkFun_APDS9960 apds = SparkFun_APDS9960();

#elif GESTURE_ENABLED && GESTURE_ADAFRUIT_ENABLED

#include "APDS9960.h"
APDS9960 apds;

#endif // GESTURE_ENABLED

// brightness value
uint16_t ambientLightReading = 0;
uint16_t ambientLightMaxDelta = 1000;

// setup function
void setupGesture () {
	// APDS9960 //
#if GESTURE_ENABLED && GESTURE_SPARKFUN_ENABLED
	apds.init();
	apds.enableGestureSensor(true);
#elif GESTURE_ENABLED && GESTURE_ADAFRUIT_ENABLED
	apds.begin();
	//for gesture to work we have to have proximity enabled and to manually set GMODE to  1
	apds.enableProximity(true);
	apds.enableGesture(true);
	// TODO: enable light sensor

	// customization

	// onion.io
	// Note: LED current will affect touch buttons !
	/*Set LED current and Boost
	LED drive current 100 mA LDRIVE = 0
	50 LDRIVE = 1
	25 LDRIVE = 2
	12.5 LDRIVE = 3

	LED boost  100 % LED_BOOST = 0
	150 LED_BOOST = 1
	200 LED_BOOST = 2
	300 LED_BOOST = 3
	*/
	apds.setLED((apds9960LedDrive_t)0x3, APDS9960_LEDBOOST_100PCNT); //onion.io Aug 16 2018: lower LED power improves touch performance, interface in the library is fucked up, if using the enum it'll be too large to fit

#endif // GESTURE_ENABLED
}

void setupAmbientLight () {
	// initialize - if not already initialized
#if GESTURE_ENABLED && GESTURE_SPARKFUN_ENABLED
	uint8_t mode = apds.getMode();
	if (mode == 0x00 || mode == 0xff) {
		apds.init();
	}
	apds.enableLightSensor(false);
#endif // GESTURE_ENABLED
}

// helper functions
void sendAmbientLightReading() {
	char buf[64];
	sprintf(buf, "%d", ambientLightReading);
	sendCommand(CMD_SEND_AMBIENT_LIGHT, buf);
}

// handle gesture data
void handleGesture() {
#if GESTURE_ENABLED && GESTURE_SPARKFUN_ENABLED
	if ( apds.isGestureAvailable() ) {
		switch ( apds.readGesture() ) {
			case DIR_UP:
        sendCommand(CMD_SEND_GESTURE, "U");
				break;
			case DIR_DOWN:
        sendCommand(CMD_SEND_GESTURE, "D");
				break;
			case DIR_LEFT:
        sendCommand(CMD_SEND_GESTURE, "L");
				break;
			case DIR_RIGHT:
        sendCommand(CMD_SEND_GESTURE, "R");
				break;
			case DIR_NEAR:
        sendCommand(CMD_SEND_GESTURE, "N");
				break;
			case DIR_FAR:
				sendCommand(CMD_SEND_GESTURE, "F");
				break;
			default:
        sendCommand(CMD_SEND_GESTURE, "X");
				break;
		}
	}
#elif GESTURE_ENABLED && GESTURE_ADAFRUIT_ENABLED
	switch ( apds.readGesture() ) {
		case APDS9960_UP:
			sendCommand(CMD_SEND_GESTURE, "U");
			break;
		case APDS9960_DOWN:
			sendCommand(CMD_SEND_GESTURE, "D");
			break;
		case APDS9960_LEFT:
			sendCommand(CMD_SEND_GESTURE, "L");
			break;
		case APDS9960_RIGHT:
			sendCommand(CMD_SEND_GESTURE, "R");
			break;
		default:
			// do nothing
			break;
	}
#endif // GESTURE_ENABLED
}

void handleAmbientLight() {
	uint16_t val = 0;
	uint16_t delta;
	bool status = false;

#if GESTURE_ENABLED && GESTURE_SPARKFUN_ENABLED
	status = apds.readAmbientLight(val);
#endif // GESTURE_ENABLED

	if (status) {
		// find the delta (abs function doesn't work for uint)
		if (ambientLightReading > val) {
			delta = ambientLightReading - val;
		} else {
			delta = val - ambientLightReading;
		}

		// update the reading
		ambientLightReading = val;
		// communicate immediately if change is larger than delta
		if (delta >= ambientLightMaxDelta) {
			// Serial.print("Large delta detected, delta = ");
			// Serial.println(delta, DEC);
			sendAmbientLightReading();
		}
	}
}
