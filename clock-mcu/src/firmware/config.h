#include <EEPROM.h>

#define RTC_ENABLED         1
#define TIME_DISP_ENABLED   1
#define GESTURE_ENABLED     1
#define LIGHTBAR_ENABLED    1
#define TOUCH_ENABLED       1
#define BATTERY_ENABLED     1

#define MAX7219_ENABLED     0
#define TM1618_ENABLED      1

#define GESTURE_SPARKFUN_ENABLED   0
#define GESTURE_ADAFRUIT_ENABLED   1

#define CONF_ADDR 0

struct ConfigData {
  bool twelveHMode; // True = 12h; False = 24h
  bool dayNightIndicator; // True = enable; False = disable
};

extern ConfigData mcuConfig;

void initConfig() {
  mcuConfig = {true, true};
}

void saveConfig() {
  EEPROM.put(CONF_ADDR, mcuConfig);
}

void loadConfig() {
  EEPROM.get(CONF_ADDR, mcuConfig);
}
