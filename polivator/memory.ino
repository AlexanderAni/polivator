// Memory

#include <EEPROM.h>

/*
5: Programm name
...
5 + len: Major version
6 + len: Minor version
7 + len: Success eeprom saved flag
8 + len: settings
9 + len + settings_len: FlowerData

*/

void readFlowerData() {
	// Check if eeprom has written data > read data
	if (state.memory_allowed) {
		char title[11];
		char text[21];
		byte len = strlen(PROG_NAME) + 1;
		int settings_len = sizeof(settings);
		char name[] = PROG_NAME;
		strcpy(title, "Memory");
		EEPROM.get(5, name);
		// Read All data if same name and version and "success flag" is true
		if (strcmp(name, PROG_NAME) == 0 && EEPROM.read(5 + len) == MAJOR_VER && EEPROM.read(6 + len) == MINOR_VER && EEPROM.read(7 + len) == true) {
			EEPROM.get(8 + len, settings);
			EEPROM.get(9 + len + settings_len, flowerData);
			state.flowerDataChanged = false;
			strcpy(text, "Loaded data");
		} else {
			strcpy(text, "Missing data");
			// if (strcmp(name, PROG_NAME) != 0) {
			// 	strcpy(text, "Missing PROG_NAME");
			// } else if (EEPROM.read(5 + len) != MAJOR_VER) {
			// 	strcpy(text, "Missing MAJOR_VER");
			// } else if (EEPROM.read(6 + len) != MINOR_VER) {
			// 	strcpy(text, "Missing MINOR_VER");
			// } else if (EEPROM.read(7 + len) != true) {
			// 	strcpy(text, "Errors");
			// }
			// Serial.println("Cannot read Flower Data from EEPROM");
			// Serial.flush();
		}
		displayMessage(title, text, 1500);
	}
}

void saveFlowerData() {
	// Write data
	int settings_len = sizeof(settings);
	// Success flag after success
	if (state.memory_allowed) {
		disableInterrupts();
		byte len = strlen(PROG_NAME) + 1;
		// Set success flag to false before trying to write
		EEPROM.update(7 + len, false);
		EEPROM.put(5, PROG_NAME);
		EEPROM.update(5 + len, MAJOR_VER);
		EEPROM.update(6 + len, MINOR_VER);
		EEPROM.put(8 + len, settings);
		EEPROM.put(9 + len + settings_len, flowerData);
		// Set success flag to true after success write
		EEPROM.update(7 + len, true);
		state.flowerDataChanged = false;
		// Serial.println("Save Flower Data to EEPROM");
		// Serial.flush();
		enableInterrupts();
	}
}

