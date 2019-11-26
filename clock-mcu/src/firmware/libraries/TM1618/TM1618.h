/*
TM1618.h - Library for TM1618.

Copyright (C) 2011 Ricardo Batista <rjbatista at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TM1618_h
#define TM1618_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM16XX.h"
#include "TM16XXFonts.h"


class TM1618 : public TM16XX
{
  public:
    /** Instantiate a TM1618 module specifying the display state, the starting intensity (0-7) data, clock and stobe pins. */
    TM1618(byte dataPin, byte clockPin, byte strobePin, byte displays, boolean activateDisplay = true,
    	byte intensity = 7);

	/** Set the display (segments and LEDs) active or off and intensity (range from 0-7). */
    void setupDisplay(boolean active, byte intensity);
	/** Set the display intensity (range from 0-7). */
    void setIntensity(byte intensity);

   protected:
     virtual void sendChar(byte pos, byte data, boolean dot);
};

#endif
