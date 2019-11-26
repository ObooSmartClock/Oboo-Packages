#include "display.h"
// RTC
//#include <Wire.h>
#if RTC_ENABLED == 1
#include "RTClib.h"

#define DESIRED_SQ_WAVE		SquareWave32kHz
#endif 	// RTC_ENABLED



// Real Time Clock
#if RTC_ENABLED == 1
RTC_DS1307 rtc;
DateTime currentTime;
#endif 	// RTC_ENABLED


uint32_t prevSeparatorTime = 0;
uint32_t separatorInterval = 500;

/*
Setup Functions
*/
// setup the RTC driver
void _setupTime () {
#if RTC_ENABLED == 1
	rtc.begin();

	if (! rtc.isrunning()) {
		// Serial.println("RTC is NOT running! Setting time to program compile time");
		// following line sets the RTC to the date & time this program was compiled
		rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	}

	// set the desired square wave (if not already set)
	// if (rtc.readSqwPinMode() != DESIRED_SQ_WAVE) {
	// 	rtc.writeSqwPinMode(DESIRED_SQ_WAVE);
	// }
#endif 	// RTC_ENABLED
}

// setup the clock sub-system
void setupClock () {
	// RTC Setup //
	_setupTime();

	// 7-segment setup //
	setupDisplay();
}

// handler functions //
void handleTime() {
	boolean bDay = false;

#if RTC_ENABLED == 1
	currentTime = rtc.now();

	if (currentTime.hour() > 24 || currentTime.minute() > 60) {
		// handle the case where RTC cannot be read
		setError();
	}
	else {
		// find the setting for the day-night indicator
		if (currentTime.hour() >= 6 && currentTime.hour() < 18) {
			bDay = true;
		}

		// onion.io: TODO: lc should only be updated if rtc minute has changed since last time
		setTime(currentTime.hour(), currentTime.minute(), mcuConfig.twelveHMode, mcuConfig.dayNightIndicator, bDay );
	}
#else
	setTime(2, 14, mcuConfig.twelveHMode, mcuConfig.dayNightIndicator, bDay );
#endif 	// RTC_ENABLED

	// interval for flashing time display separator colon
	if (millis() - prevSeparatorTime >= separatorInterval) {
		flipTimeDisplaySeparator();

		// prepare for next interval
		prevSeparatorTime = millis();
	}
}

// helper functions
void setRtcTime(long timestamp) {
#if RTC_ENABLED == 1
	rtc.adjust(DateTime(timestamp));
#endif 	// RTC_ENABLED

	// syncronize separator blink
	prevSeparatorTime = millis();
}



uint32_t getEpoch() {
#if RTC_ENABLED == 1
	return (currentTime.unixtime());
#else
	return (1517497971);
#endif 	// RTC_ENABLED
}

uint32_t getSqWvSetting() {
#if RTC_ENABLED == 1
	return (rtc.readSqwPinMode());
#else
	return (0xff);
#endif 	// RTC_ENABLED
}
