//#include "storage.h"

//#define FLAG_SEND_GESTURE 1
//#define FLAG_SEND_AMBIENT_LIGHT 1<<1
//#define FLAG_SEND_BATTERY_LEVEL 1<<2
//#define FLAG_SEND_TOUCH 1<<3
//
//#define FLAG_SET_TIME 1
//#define FLAG_SET_LIGHT_BAR 1<<1
//#define FLAG_SET_DISPLAY_BRIGHTNESS 1<<2
//#define FLAG_SET_HOUR_MODE 1<<3
//#define FLAG_SET_DAY_NIGHT_LED 1<<4
//#define FLAG_SET_INIT 1<<5

#define CMD_SET_TIME 'T'
#define CMD_SET_HOUR_MODE 'H'
#define CMD_SET_LIGHT_BAR 'L'
#define CMD_SET_BUTTON 'b'
#define CMD_SET_ALL_BUTTONS 'l'
#define CMD_SET_DISPLAY_BRIGHTNESS 'B'
#define CMD_SET_DAY_NIGHT_LED 'D'
#define CMD_SET_BLINKING 'K'
#define CMD_SET_INIT 'I'

#define CMD_SEND_GESTURE 'G'
#define CMD_SEND_TOUCH 't'
#define CMD_SEND_BATTERY 'V'
#define CMD_SEND_AMBIENT_LIGHT 'A'
#define CMD_SEND_INIT 'i'

//int setFlag = 0;
//int sendFlag = 0;


void sendCommand (char cmd, String payload){
  Serial.print('~');
  Serial.print(cmd);
  Serial.print(payload);
  Serial.println(';');
}

bool parseInput (char* rxData) {

  char cmd = rxData[0];
  String payload = String(++rxData);

  long timeStamp = 0;
  int payloadInt = 255;
  char buf[64];
  uint32_t payloadArr[LIGHTBAR_NUM_LEDS];

  switch (cmd) {
    case CMD_SET_TIME:
      timeStamp = payload.toInt();

      // set RTC to received time
      setRtcTime(timeStamp);

      Serial.print("Setting RTC to: ");
      Serial.println(timeStamp);
    break;

    case CMD_SET_HOUR_MODE:
      if (payload == "12") {
        mcuConfig.twelveHMode = true;
        Serial.println("12h Mode");
      }else if (payload == "24") {
        mcuConfig.twelveHMode = false;
        Serial.println("24h Mode");
      }
      saveConfig();
    break;

    case CMD_SET_DISPLAY_BRIGHTNESS:
      payloadInt = payload.toInt();
      setDisplayBrightness(payloadInt);
    break;

#if defined LIGHTBAR_ENABLED && LIGHTBAR_ENABLED == 1
    case CMD_SET_LIGHT_BAR:
      // Serial.print("Setting lightbar to 0x");
      // Serial.println((uint32_t)payload.toFloat(), HEX);
      lightBarSetColor((uint32_t)payload.toFloat());
    break;

    case CMD_SET_BUTTON:
      // payload = [buttonIndex (1 char)][buttonColor (1-many chars)]
      payloadInt = (int)rxData[0] - '0';
      payload = String(++rxData);

      // Serial.print("Setting button ");
      // Serial.print(payloadInt);
      // Serial.print(" to 0x");
      // Serial.println((uint32_t)payload.toFloat(), HEX);

      lightBarSetButton(payloadInt, (uint32_t)payload.toFloat() );
    break;

    case CMD_SET_ALL_BUTTONS:
      // payload = [button0 Color]:[button1 Color]:[button2 Color]:[button3 Color]

      sscanf(payload.c_str(), "%lu:%lu:%lu:%lu",
                                     &payloadArr[0],
                                     &payloadArr[1],
                                     &payloadArr[2],
                                     &payloadArr[3]
                                   );

      // Serial.print("Setting buttons to ");
      // Serial.print((uint32_t)payloadArr[0], HEX);
      // Serial.print((uint32_t)payloadArr[1], HEX);
      // Serial.print((uint32_t)payloadArr[2], HEX);
      // Serial.print((uint32_t)payloadArr[3], HEX);

      lightBarSetButton(0, payloadArr[0] );
      lightBarSetButton(1, payloadArr[1] );
      lightBarSetButton(2, payloadArr[2] );
      lightBarSetButton(3, payloadArr[3] );
    break;
#endif  // LIGHTBAR_ENABLED
    case CMD_SET_DAY_NIGHT_LED:
      if (payload == "1") {
        mcuConfig.dayNightIndicator = true;
      }else if (payload == "0") {
        mcuConfig.dayNightIndicator = false;
      }
      saveConfig();
    break;

    case CMD_SET_BLINKING:
      mcuConfig.blink = (unsigned)(*rxData - '0') < 2 ? (*rxData - '0') : 2; 
      saveConfig();
    break;


    case CMD_SET_INIT:
    	sprintf(buf, "%lu", getEpoch());
    	sendCommand(CMD_SEND_INIT, buf);
    break;

    // TODO: add case to set max ambient light delta
    // TODO: add case to set number of transition steps for lightbar (lower priority)
  }

  return true;
}
