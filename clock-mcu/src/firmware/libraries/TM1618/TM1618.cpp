#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM1618.h"
#include "string.h"

TM1618::TM1618(byte dataPin, byte clockPin, byte strobePin, byte displays, boolean activateDisplay, byte intensity)
	: TM16XX(dataPin, clockPin, strobePin, displays, activateDisplay, intensity)
{
	// nothing else to do - calling super is enough
}

void TM1618::setupDisplay(boolean active, byte intensity)
{
  // set to 4 digits, 8 segments total
  sendCommand(0x00);
  // enable the display
  //	bits 7,6 	- display control mode
  //	bit 3		- display on/off
  //	bits 2-0	- brightness
  sendCommand(0x80 | (active ? 8 : 0) | min(7, intensity));
}

void TM1618::setIntensity(byte intensity)
{
	// set the brightness - assuming
    //	bits 7,6 	- display control mode
    //	bit 3		- display ON
    //	bits 2-0	- brightness
    sendCommand(0x80 | 8 | min(7, intensity));
}

void TM1618::sendChar(byte pos, byte data, boolean dot)
{
#ifdef TM1618_DEBUG
	char buf[64];
	sprintf(buf, "sendChar: pos = 0x%02x, data = 0x%02x, dot = %d\n", pos, data, dot ? 1 : 0);
	Serial.print(buf);
#endif

	// add in the Dp
	data = data | (dot ? 0b10000000 : 0);

	// Send first byte to specify Segs A-E
	//		byte0:	X X X segE  segD segC segB segA
	sendData(pos << 1, data & 0x1f);

	// Send second byte to specify Segs A-E
	//		byte0:	X X segDp segG  segF X X X
	sendData((pos << 1) + 1, (data & 0xe0) >> 2);
}
