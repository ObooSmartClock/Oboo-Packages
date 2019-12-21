

#define JG106C_ADDRESS 0x2c

#define JG106C_INT_PIN	3
#define JG106C_ISR_NUM 	1

#if defined TOUCH_ENABLED && TOUCH_ENABLED == 1
#include <Wire.h>

#if (ARDUINO >= 100)
 #include <Arduino.h> // capital A so it is error prone on case-sensitive filesystems
 // Macro to deal with the difference in I2C write functions from old and new Arduino versions.
 #define _I2C_WRITE write
 #define _I2C_READ  read
#else
 #include <WProgram.h>
 #define _I2C_WRITE send
 #define _I2C_READ  receive
#endif

#define JG106C_SENS_REGISTER		0x10
#define JG106C_SLP_SENS_REGISTER	0x20

#define TOUCH_NUM_KEYS			4
#define TOUCH_KEY0				0
#define TOUCH_KEY1				1
#define TOUCH_KEY2				2
#define TOUCH_KEY3				3


static void read_i2c_bytes(uint8_t addr, uint8_t numBytes, uint8_t *values) {
  uint8_t count = 0;
  // Wire.beginTransmission(addr);
  // Wire._I2C_WRITE((byte)reg);
  // Wire.endTransmission();

  Wire.requestFrom(addr, (byte)2, (byte)true);
  while (Wire.available()) {
	  if (count < numBytes) {
		  values[count] = Wire.read();
		  count++;
	  }
	  else {
		  // read to clear the buffer
		  Wire.read();
	  }
  }
}

static void write_i2c_register(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire._I2C_WRITE((byte)reg);
  Wire._I2C_WRITE((byte)val);
  Wire.endTransmission();
}

void setButtonSensitivity (uint8_t button, uint8_t sensitivity) {
	uint8_t regVal;
	// set sensitivity
	regVal 	= JG106C_SENS_REGISTER | button;
	write_i2c_register(JG106C_ADDRESS, regVal, sensitivity);
	// set sleep sensitivity to the same
	regVal 	= JG106C_SLP_SENS_REGISTER | button;
	write_i2c_register(JG106C_ADDRESS, regVal, sensitivity);
}

void touchInit() {
	delay(300);
	Wire.begin();
	// initialize the IC
	write_i2c_register(JG106C_ADDRESS, (uint8_t)0x05, (uint8_t)TOUCH_NUM_KEYS);
	// byte0:
	//	Bit2 - KOM (Key Output Mode)
	//		0 = single touch; 1 = multi-touch
	//	Bit1 - RT (Reset Enable)
	//		0 = disabled; 1 = enabled
	//	Bit0 - SLP (Power Saving mode - enter after 4 seconds of inactivity)
	//		0 = disabled; 1 = enabled
	// byte1:
	//	defines how many keys are required

	// set the sensitivity
	setButtonSensitivity(TOUCH_KEY0, 0x0a);
	setButtonSensitivity(TOUCH_KEY1, 0x0a);
	setButtonSensitivity(TOUCH_KEY2, 0x0a);
	setButtonSensitivity(TOUCH_KEY3, 0x0a);
}

uint8_t handleTouch() {
	uint8_t buttons[2];
	// read the button status
	read_i2c_bytes(JG106C_ADDRESS, 2, buttons);

  return buttons[0];
}


void sendButtonCommand(uint8_t buttons, uint32_t timePressed) {
  char buf[64];
	// send button status via serial
	sprintf(buf, "%02x%lu", buttons, timePressed);
	sendCommand(CMD_SEND_TOUCH, buf);
}


#endif 	// TOUCH_ENABLED
