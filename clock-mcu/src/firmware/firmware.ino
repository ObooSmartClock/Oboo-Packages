// libraries //
#include "config.h"
#include "clock.h"
#if defined LIGHTBAR_ENABLED && LIGHTBAR_ENABLED == 1
#include "lightbar.h"
#endif 	// LIGHTBAR_ENABLED

#include "protocol.h"


#include "gesture.h"
#include "touch.h"

#include "battery.h"

#define ISR_NUM2 		2
#define NUM_ISR			3

uint32_t prevOutboundTime;
uint32_t outboundInterval;
uint8_t buttonsPressed;
uint32_t buttonPressTime;

// global variables //
ConfigData mcuConfig;

volatile byte isrFlag[NUM_ISR];


/*
Gesture ISR
*/
void gestureInterruptRoutine() {
	isrFlag[APDS9960_ISR_NUM] = 1;
}

/*
Touch ISR
*/
void touchInterruptRoutine() {
	if (digitalRead(JG106C_INT_PIN) == 0) {
		// FALLING
		isrFlag[JG106C_ISR_NUM] = 1;
	}
	else {
		// RISING
		isrFlag[ISR_NUM2] = 1;
	}
}

/*
UART ISR
*/
char* rxBuff = new char[64];
int rxIndex = 0;
void serialEvent() {
	while (Serial.available()) {

	char inChar = (char)Serial.read();

	if (inChar == '~'){
			rxIndex = 0;
			continue;
		} else if (inChar == ';'){
			rxBuff[rxIndex] = 0;
			parseInput(rxBuff);
			continue;
		} else {
			rxBuff[rxIndex++] = inChar;
		}
	}
}



/*
 * main program setup function
 */
void setup() {
	Serial.begin(9600);
	// Serial.println(F("SETUP FUNCTION"));
	prevOutboundTime = 0;
	outboundInterval = 15;
	buttonPressTime = 0;
	buttonsPressed = 0;

	loadConfig();
#if defined LIGHTBAR_ENABLED && LIGHTBAR_ENABLED == 1
	lightBarInit();
	// lightBarSetColor(0x6432c8);
#endif 	// LIGHTBAR_ENABLED
#if defined TOUCH_ENABLED && TOUCH_ENABLED == 1
	touchInit();
#endif  // TOUCH_ENABLED

	// initialize pins
	pinMode(APDS9960_INT_PIN, INPUT);
	pinMode(JG106C_INT_PIN, INPUT);

	// initialize peripherals
	setupClock();
#if defined GESTURE_ENABLED && GESTURE_ENABLED == 1
	setupGesture();
	setupAmbientLight();
#endif	// GESTURE_ENABLED

	// Initialize interrupt service routines
#if GESTURE_INTERRUPT
	//     gesture sensor
	attachInterrupt(0, gestureInterruptRoutine, FALLING);
#endif // GESTURE_INTERRUPT
	//     touch sensor
	attachInterrupt(digitalPinToInterrupt(JG106C_INT_PIN), touchInterruptRoutine, CHANGE);
}


// the super-loop function
void loop() {

	// handle clock and time display
	handleTime();

	// handle touch
	if (isrFlag[JG106C_ISR_NUM] == 1 || isrFlag[ISR_NUM2] == 1) {
		detachInterrupt(digitalPinToInterrupt(JG106C_INT_PIN));
#if defined TOUCH_ENABLED && TOUCH_ENABLED == 1
		if (isrFlag[JG106C_ISR_NUM] == 1) {
			buttonPressTime = millis();
			buttonsPressed = handleTouch();
			isrFlag[JG106C_ISR_NUM] = 0;
		}
		if (isrFlag[ISR_NUM2] == 1) {
			sendButtonCommand(buttonsPressed, millis() - buttonPressTime);
			// reset
			buttonPressTime = 0;
			buttonsPressed = 0;
			isrFlag[ISR_NUM2] = 0;
		}
#endif	// TOUCH_ENABLED

		attachInterrupt(digitalPinToInterrupt(JG106C_INT_PIN), touchInterruptRoutine, CHANGE);
	}

	// handle gestures
#if GESTURE_INTERRUPT
	if (isrFlag[APDS9960_ISR_NUM] == 1) {
		detachInterrupt(0);
#if defined GESTURE_ENABLED && GESTURE_ENABLED == 1
		handleGesture();
#endif	// GESTURE_ENABLED
		isrFlag[APDS9960_ISR_NUM] = 0;
		attachInterrupt(0, gestureInterruptRoutine, FALLING);
	}
#elif GESTURE_ENABLED
	handleGesture();
#endif // GESTURE_INTERRUPT

	// handle ambient light brightness
#if defined GESTURE_ENABLED && GESTURE_ENABLED == 1
	handleAmbientLight();
#endif	// GESTURE_ENABLED

	// update lightbar
#if defined LIGHTBAR_ENABLED && LIGHTBAR_ENABLED == 1
	lightBarFade();
#endif 	// LIGHTBAR_ENABLED

	// outbound communication on an interval
	if (getEpoch() - prevOutboundTime >= outboundInterval) {
		// ambient light
#if defined GESTURE_ENABLED && GESTURE_ENABLED == 1
		// onion.io: testing - removed this
		sendAmbientLightReading();
#endif	// GESTURE_ENABLED

#if defined BATTERY_ENABLED && BATTERY_ENABLED == 1
		// onion.io: testing - removed this
		sendBatteryLevel();
#endif  // BATTERY_ENABLED

		// prepare for next interval
		prevOutboundTime = getEpoch();
	}
}
