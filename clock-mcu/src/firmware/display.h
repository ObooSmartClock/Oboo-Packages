#if MAX7219_ENABLED == 1
// MAX 7219 -> 7seg
#include "LedControl.h"

// defines
#define MAX7219_DIN			A0
#define MAX7219_CLK			A2
#define MAX7219_CS			A1
#define MAX7219_NUM_CHIPS	1
#define MAX7219_CHIP_NUM	0

#define MAX7219_MIN_BRIGHTNESS	0
#define MAX7219_MAX_BRIGHTNESS	15

#define DEFAULT_BRIGHTNESS		8


// 7seg controller
LedControl lc = LedControl(MAX7219_DIN,MAX7219_CLK,MAX7219_CS,MAX7219_NUM_CHIPS);	//  LedControl(DataIn, CLK, LOAD (CS), NumChips)

#elif TM1618_ENABLED == 1

#include "TM1618.h"

#define TM1618_DIN     A0
#define TM1618_CLK     A2
#define TM1618_CS      A1

#define TM1618_NUM_DISPS      8

#define TM1618_MIN_BRIGHTNESS	0
#define TM1618_MAX_BRIGHTNESS	7
#define DEFAULT_BRIGHTNESS		1

TM1618 lc = TM1618(TM1618_DIN, TM1618_CLK, TM1618_CS, TM1618_NUM_DISPS);   // TM16XX(byte dataPin, byte clockPin, byte strobePin, byte displays

#endif 	// MAX7219_ENABLED vs TM1618_ENABLED


/* specifies the value of the colon between the hours and minutes on a 7-segment display */
bool bSeparatorColon;

/* helper function declarations */
void _sanitizeTwoDigitInput	(int input, int *output);
void _setDigit		(int digit, byte value, boolean dp);
void _setChar		(int digit, char value, boolean dp);

/*
Setup Functions
*/
// setup the 7-segment display driver
void setupDisplay () {
	bSeparatorColon = true;

#if MAX7219_ENABLED == 1
	// MAX7219 //
	//wake up from power-saving mode
	lc.shutdown(0,false);
	// Set the brightness to default
	lc.setIntensity(0, DEFAULT_BRIGHTNESS);
	// clear the display
	lc.clearDisplay(0);

#elif TM1618_ENABLED == 1

	// TM1618 //
    // activate display and set brightness to default
    lc.setupDisplay(true, DEFAULT_BRIGHTNESS);
#endif 	// MAX7219_ENABLED vs TM1618_ENABLED


}

/*
Set Functions
*/
void setDisplayBrightness(int brightness) {
#if MAX7219_ENABLED == 1
	if (brightness >= MAX7219_MIN_BRIGHTNESS && brightness <= MAX7219_MAX_BRIGHTNESS) {
		lc.setIntensity(0,brightness);
	}
#elif TM1618_ENABLED == 1
	if (brightness >= TM1618_MIN_BRIGHTNESS && brightness <= TM1618_MAX_BRIGHTNESS) {
		lc.setIntensity(brightness);
	}
#endif 	// MAX7219_ENABLED vs TM1618_ENABLED
}

/*
 * Flip the value of hour-minute separator colon value on a 7-segment display
 *     If the separator is on, turn it off,
 *     If it is off, turn it on
 */
void flipTimeDisplaySeparator() {
	bSeparatorColon = !bSeparatorColon;
}

void setError() {
	// handle the error case
	_setChar(3, 'o', false);
	_setChar(2, 'o', false);
	_setChar(1, 'b', false);
	_setChar(0, 'o', false);
}

/*
 * Display the time on the Onion 7-Segment display.
 * Params:
 * hours	the hour value to be displayed - 24 hour time format (0-24)
 * minutes  the minute value to be displayed (0-59)
 * b12hMode
 *      false   - display in 24hr mode
 *      true    - display in 12hr mode
 * bDayNightIndicator
 *      false   - disable the Onion day-night indicator
 *      true    - enable the Onion day-night indicator
 */
void setTime(int hours, int minutes, boolean b12hMode, boolean bDayNightIndicator, boolean bDay) {
    int digits[4];
    bool bDayNight[2] = {false, false};

    // sort out 24hr vs 12hr format
    if (b12hMode) {
        // convert to 12hr mode
        if (hours > 12) {
            hours -= 12;
        }
        if (hours == 0) {
            hours += 12;
        }
    }

    // program the day-night indicator
    if (bDayNightIndicator) {
        if (bDay) {
            // day - both lights are on
            bDayNight[0] = true;
            bDayNight[1] = true;
        } else {
            bDayNight[0] = true;
            bDayNight[1] = false;
        }
    }

    // split the hours and minutes inputs into individial digits
    _sanitizeTwoDigitInput(minutes, &digits[0]);
    _sanitizeTwoDigitInput(hours, &digits[2]);

    // write to display
    _setDigit(3,digits[0], bDayNight[1]);
    _setDigit(2,digits[1], bDayNight[0]);

    _setDigit(1,digits[2], bSeparatorColon);
    // for 12hr time, do not show a leading zero
    if (!b12hMode || digits[3] > 0) {
        _setDigit(0,digits[3], bSeparatorColon);
    } else {
        _setChar(0, ' ', bSeparatorColon);
    }


}


// display helper Functions

/* sanitize and split a two digit integer input into an array */
void _sanitizeTwoDigitInput(int input, int *output) {
    if (input >= 10) {
        output[1] = (input/10) & 0x0f;
        output[0] = (input - (output[1]*10)) & 0x0f;
    } else {
        output[1] = 0;
        output[0] = (input & 0x0f);
    }
}

/* write a number to a 7segment digit */
void _setDigit(int digit, byte value, boolean dp) {
#if MAX7219_ENABLED == 1
	lc.setDigit(MAX7219_CHIP_NUM, digit, value, dp);
#elif TM1618_ENABLED == 1
	lc.setDisplayDigit(value, digit, dp);
#endif 	// MAX7219_ENABLED vs TM1618_ENABLED
}

/* write a character to a 7segment digit */
void _setChar(int digit, char value, boolean dp) {
#if MAX7219_ENABLED == 1
	lc.setChar(MAX7219_CHIP_NUM, digit, value, dp);
#elif TM1618_ENABLED == 1
	lc.setDisplayDigit(((byte)value)-32, digit, dp, FONT_DEFAULT);
#endif 	// MAX7219_ENABLED vs TM1618_ENABLED
}
