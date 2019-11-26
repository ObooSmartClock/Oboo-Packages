#define BATTERY_PIN     A6

#if defined BATTERY_ENABLED && BATTERY_ENABLED == 1

void sendBatteryLevel() {
	uint16_t batteryLevel;
	char buf[64];

	// read the battery voltage level
	batteryLevel = analogRead(BATTERY_PIN);

	// send battery level via serial
	sprintf(buf, "%d", batteryLevel);
	sendCommand(CMD_SEND_BATTERY, buf);
}
#endif  // BATTERY_ENABLED
