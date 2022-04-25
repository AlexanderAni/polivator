// Working with display

#include <GyverOLED.h>

#define DISPLAY_TEXT_WIDTH_3 7 // Size 3
#define DISPLAY_TEXT_WIDTH_2 11 // Size 2
#define DISPLAY_TEXT_WIDTH 21 // Size 1


// GyverOLED does not work without buffer for 1106 Display
GyverOLED<SSH1106_128x64, OLED_NO_BUFFER, OLED_SPI, SPI_CS_PIN, SPI_DC_PIN, SPI_RESET_PIN> display;
// GyverOLED<SSD1306_128x64, OLED_NO_BUFFER, OLED_SPI, SPI_CS_PIN, SPI_DC_PIN, SPI_RESET_PIN> display;
// GyverOLED<SSH1106_128x64, OLED_NO_BUFFER, OLED_I2C> display;

// Declaration

void displayPosition(char *text, byte pos=0, bool active=false);
void displayMessage(char* title, char* text, int pause=0);

// Setup

void displaySetup() {
	display.init();
	display.autoPrintln(false);
	display.textMode(BUF_REPLACE);
	// Serial.println("Display loaded");
	clearDisplay();
	displayHello();
}

void displayHello() {
	char text[DISPLAY_TEXT_WIDTH];
	char title[] = PROG_NAME;
	char minor[2];
	itoa(MAJOR_VER, text, 10);
	strcat(text, ".");
	itoa(MINOR_VER, minor, 10);
	strcat(text, minor);
	displayMessage(title, text, 1000);
}

// Tools


char* timeText(DateTime time) {
	static char time_text[6];
	char hour[3];
	char minutes[3];
	// strcpy(time_text, "");
	// time_text[0] = '\0';
	time_text[0] = 0; // Empty the text

	// Collect time text
	itoa(time.hour, hour, 10);
	itoa(time.minute, minutes, 10);
	if (time.hour < 10) {
		strcat(time_text, "0");
	}
	strcat(time_text, hour);
	strcat(time_text, ":");
	if (time.minute < 10) {
		strcat(time_text, "0");
	}
	strcat(time_text, minutes);
	return time_text;
}

char* tempText() {
	static char temp_text[5];
	itoa(state.temp, temp_text, 10);
	strcat(temp_text, "c");
	return temp_text;
}

void periodValueText(char text[5], byte val) {
	// Write period val to text
	// 0 - never
	// 1..20 - every 1..20 hours
	// 21..51 - every 1 - 30 days (x - 20)
	// example: 6 - every 6 hours, 12 - every 12 hours, 21 - every day, 33 - every 13 days
	if (val == 0) {
		strcpy(text, "never");
	} else if (val < 21) {
		itoa(val, text, 10);
		strcat(text, "h");
	} else {
		itoa(val - 20, text, 10);
		strcat(text, "d");
	}
}

void millisValueToText(char text[5], uint32_t val, bool seconds=true) {
	// Write millis value to text
	// Default with seconds
	if (val == 0) {
		strcpy(text, "0");
	} else if (seconds && val < 60000) { // Up to 1 minute > seconds (if seconds is true)
		itoa((long)val / 1000, text, 10);
		strcat(text, "s");
	} else if (val < 3600000) { // Up to 90 minutes > minutes
		itoa((long)val / 60000, text, 10);
		strcat(text, "m");
	} else if (val < 86400000) { // Up to 24 hours > hours
		itoa((long)val / 3600000, text, 10);
		strcat(text, "h");
	} else {
		itoa((long)val / 86400000, text, 10); // > 24 hours
		strcat(text, "d");
	}
}

void secondsValueToText(char text[6], long val, bool seconds=true) {
	// Write seconds value to text
	// Default with seconds
	char detailed[3]; // Detailed value. Second part of a text
	if (val == 0) {
		strcpy(text, "0");
	} else if (seconds && val < 60) { // Up to 1 minute > seconds (if seconds is true)
		itoa(val, text, 10);
		strcat(text, "s");
	} else if (!seconds && val < 60) { // Up to 1 minute > seconds (if seconds is false)
		strcpy(text, "now");
	} else if (val < 3600) { // Up to 90 minutes > minutes
		// minutes
		itoa((long)val / 60, text, 10);
		strcat(text, "m");
		// seconds
		if (seconds) {
			itoa((long)val % 60, detailed, 10);
			strcat(text, detailed);
			strcat(text, "s");
		}
	} else if (val < 86400) { // Up to 24 hours > hours
		// hours
		itoa((long)val / 3600, text, 10);
		strcat(text, "h");
		// minutes
		itoa((long)((long)val % 3600) / 60, detailed, 10);
		strcat(text, detailed);
		strcat(text, "m");
	// } else if (val < 2592000) {  // > 24 hours < 30 days
	} else if (val < 7776000) {  // > 24 hours < 90 days
		// days
		itoa((long)val / 86400, text, 10);
		strcat(text, "d");
		// hours
		itoa((long)((long)val % 86400) / 3600, detailed, 10);
		strcat(text, detailed);
		strcat(text, "h");
	} else { // very long time
		strcpy(text, "--");
	}
}

void dateToText(char *text, DateTime time) {
	char value[4];
	if (time.year == NEVER_YEAR) {
		strcpy(text, "--");
	} else {
		// Day
		itoa(time.date, value, 10);
		if (time.date < 10) {
			strcpy(text, "0");
			strcat(text, value);
		} else {
			strcpy(text, value);
		}
		// Month
		strcat(text, ".");
		itoa(time.month, value, 10);
		if (time.month < 10) {
			strcat(text, "0");
		}
		strcat(text, value);
		// Hour
		strcat(text, " ");
		itoa(time.hour, value, 10);
		if (time.hour < 10) {
			strcat(text, "0");
		}
		strcat(text, value);
		// Minute
		strcat(text, ":");
		itoa(time.minute, value, 10);
		if (time.minute < 10) {
			strcat(text, "0");
		}
		strcat(text, value);
	}
}

void soilNumberText(char text[5], int8_t val) {
	// Write sensor value to text
	if (val == -1) {
		strcpy(text, "no");
	} else {
		itoa(val + 1, text, 10);
	}
}

void sensorValueText(char text[6], byte val) {
	// Write sensor value to text
	if (val == 100) {
		strcpy(text, "never");
	} else {
		itoa(val, text, 10);
		strcat(text, "%");
	}
}

void volumeValueText(char text[5], uint16_t val) {
	// Write volume value to text
	if (val < 1000) {
		itoa(val, text, 10);
		strcat(text, "ml");
	} else {
		dtostrf((float)val / 1000, 4, 1, text);
		strcat(text, "L");
	}
}

// Message

void displayMessage(char* title, char* text, int pause) {
	disableInterrupts();
	clearDisplay();
	display.setScale(2);
	display.setCursor(0, 2);
	fillTextCentred(title, DISPLAY_TEXT_WIDTH_2);
	display.print(title);
	display.setScale(1);
	display.setCursor(0, 5);
	display.autoPrintln(true);
	fillTextCentred(text, DISPLAY_TEXT_WIDTH);
	display.print(text);
	display.update();
	display.autoPrintln(false);
	if (pause) {
		delay(pause);
		clearDisplay();
	}
	enableInterrupts();
}

// Big Title Lines
// Title Size 3
// Line Size 2
// Lines: 2

void displayBigTitle(char* text) {
	display.setScale(3);
	display.home();
	fillTextLeftAlign(text, DISPLAY_TEXT_WIDTH_3);
	display.print(text);
	display.update();
}

void displayBigLR(char* text1, char* text2) {
	char text[DISPLAY_TEXT_WIDTH_3];
	collectTextLR(text, text1, text2, DISPLAY_TEXT_WIDTH_3);
	displayBigTitle(text);
}

// pos is a line number from the top

void displayBigLine(char* text, byte pos=0, bool indent=false) {
	display.setScale(2);
	display.setCursor(indent * 2, 3 + pos * 2);
	fillTextLeftAlign(text, DISPLAY_TEXT_WIDTH_2 - indent);
	display.print(text);
	display.update();
}

void displayBigLineLR(char* text1, char* text2, byte pos=0) {
	char text[DISPLAY_TEXT_WIDTH_2];
	collectTextLR(text, text1, text2, DISPLAY_TEXT_WIDTH_2 - 1);
	displayBigLine(text, pos, false); // No indent
}

// Small Title Lines
// Title Size 2
// Line Size 2
// Lines: 2

void displaySmallTitle(char* text, bool selected=false) {
	display.setScale(2);
	display.home();
	fillTextLeftAlign(text, DISPLAY_TEXT_WIDTH_2);
	display.invertText(selected); // Selection
	display.print(text);
	display.update();
	display.invertText(false); // Selection
}

void displaySmallLR(char* text1, char* text2, bool selected=false) {
	char text[DISPLAY_TEXT_WIDTH_2];
	collectTextLR(text, text1, text2, DISPLAY_TEXT_WIDTH_2 - 1);
	displaySmallTitle(text, selected);
}

// pos is a line number from the top

void displaySmallLine(char* text, byte pos=0, bool indent=false) {
	display.setScale(2);
	display.setCursor(indent * 2, 3 + pos * 3);
	fillTextLeftAlign(text, DISPLAY_TEXT_WIDTH_2 - indent);
	display.print(text);
	display.update();
}

void displaySmallLineLR(char* text1, char* text2, byte pos=0) {
	char text[DISPLAY_TEXT_WIDTH_2];
	collectTextLR(text, text1, text2, DISPLAY_TEXT_WIDTH_2 - 1);
	displaySmallLine(text, pos, false); // No indent
}

// Menu Lines
// Line Size 1
// Lines: 5
// pos is a line number from the top

void displayMenuLine(char* text, byte pos=0, bool selected=false, bool indent=false) {
	display.setScale(1);
	display.setCursor(indent, 3 + pos);
	fillTextLeftAlign(text, DISPLAY_TEXT_WIDTH - indent);
	display.invertText(selected); // Selection
	display.print(text);
	display.update();
	display.invertText(false); // Selection
}

void displayMenuLineValue(char* name, char* value, byte pos=0, bool selected=false, bool indent=false) {
	char text[DISPLAY_TEXT_WIDTH];
	collectTextLR(text, name, value, DISPLAY_TEXT_WIDTH - indent);
	displayMenuLine(text, pos, selected, indent);
}

void displayMenuLineLR(char* text1, char* text2, byte pos=0, bool selected=false, bool indent=false) {
	char text[DISPLAY_TEXT_WIDTH];
	collectTextLR(text, text1, text2, DISPLAY_TEXT_WIDTH - indent);
	displayMenuLine(text, pos, selected, indent);
}

void collectTextLR(char* text, char* text1, char* text2, byte width) {
	// Collect text with text1 on the left side and text2 on the right
	// text1 . . . . . text2
	strcpy(text, text1);
	while ((strlen(text) + strlen(text2)) < width) {
		// Fill central indent with spaces
		strcat(text, " ");
	}
	strcat(text, text2);
}

void fillTextLeftAlign(char* text, byte width) {
	// Fill text with spaces: left align
	// text . . .
	// Concatenate right spaces
	while (strlen(text) < width) {
		strcat(text, " ");
	}
}

void fillTextRightAlign(char* text, byte width) {
	// Fill text with spaces: right align
	// . . . text
	char text_bak[width];
	strcpy(text_bak, text); // Save text
	strcpy(text, ""); // Clear text
	// Fill left spaces
	while ((strlen(text_bak) + strlen(text)) < width) {
		strcat(text, " ");
	}
	// Concatenate
	strcat(text, text_bak);
}

void fillTextCentred(char* text, byte width) {
	// Fill text with spaces: centre align
	// . . . text . . .
	char text_bak[width];
	strcpy(text_bak, text); // Save text
	strcpy(text, ""); // Clear text
	// Fill left spaces
	while ((strlen(text_bak) + 2 * strlen(text) + 1) < width) {
		strcat(text, " ");
	}
	// Concatenate
	strcat(text, text_bak);
	// Concatenate right spaces
	while (strlen(text) < width) {
		strcat(text, " ");
	}
}

// Data screen

void displayData() {
	if (state.display_upd_count == 0) {
		disableInterrupts();
		if (state.menu_screen == 0) {
			if (state.menu_function == 0) {
				displayDataScreen();
			} else {
				displaySettingsScreen();
			}
		} else {
			displayFlowerScreen(state.menu_screen - 1);
		}
		enableInterrupts();
		state.display_upd_count = DISPLAY_UPDATE_COUNT;
	} else {
		state.display_upd_count -= 1;
	}
}

void displayDataActiveWatering() {
	char text[DISPLAY_TEXT_WIDTH_2];
	char num[5];
	// First line: Name of flower
	strcpy(text, "Water: ");
	strcat(text, FLOWER_NAMES[state.active_watering]);
	displaySmallLine(text, 1);
	// Second Line: Left time
	strcpy(text, "Left: ");
	millisValueToText(num, state.active_watering_stop_time - millis(), true); // with seconds
	strcat(text, num);
	displaySmallLine(text, 2);
}

void displayDataScreen() {
	char text1[DISPLAY_TEXT_WIDTH_2];
	char text2[DISPLAY_TEXT_WIDTH_2];
	// Main Time and temp
	displaySmallLR(timeText(rtc.getTime()), tempText());

	if (state.active_watering != -1) {
		displayDataActiveWatering();
	} else {
		// First line
		strcpy(text1, "");
		if (state.tempSensor == true) {
			itoa(state.humidity, text1, 10);
			strcat(text1, "%");
		}
		if (dayTime()) {
			strcpy(text2, "day");
		} else {
			strcpy(text2, "night");
		}
		displaySmallLineLR(text1, text2, 0);
		// Last line

		if (state.water_allowed) {
			strcpy(text1, "Allowed");
			displaySmallLine(text1, 1);
		} else {
			if (state.water_leak) {
				strcpy(text1, "Leakage");
			} else if (!state.water_level) {
				strcpy(text1, "No water");
			} else {
				strcpy(text1, "Disabled");
			}
			displaySmallLine(text1, 1);
		}
	}
}

// Settings

void displaySettingsScreen() {
	// Title
	char title[DISPLAY_TEXT_WIDTH_2] = "Settings";
	char position_text[DISPLAY_TEXT_WIDTH];
	displaySmallTitle(title);

	switch (state.menu_function) {
		case 1:
			// Position change
			for (byte i = state.menu_position_slide; i < state.menu_position_slide + DISPLAY_POSITIONS; i = i + 1) {
				bool active = false;
				// Is Active
				if (i == state.menu_position - 1) {active = true;};
				settingsPositionText(position_text, i); // text preparation
				displayPosition(position_text, i - state.menu_position_slide, active);
			}
			break;
		case 2:
			// Position change
			displaySettingsValue();
			break;
	}
}

void settingsPositionText(char* text, byte pos) {
	strcpy(text, SETTINGS_MENU[pos]);
	char value[DISPLAY_TEXT_WIDTH_2];
	switch (pos) {
		case 0:
			timeFromHourText(value, settings.day_start_hour);
			break;
		case 1:
			timeFromHourText(value, settings.day_end_hour);
			break;
		case 2:
			itoa(settings.soil_sensor_zero, value, 10);
			break;
		case 3:
			itoa(settings.soil_sensor_full, value, 10);
			break;
		case 4:
			itoa(settings.leakage_finish_delay, value, 10);
			break;
	}
	strcat(text, ": ");
	strcat(text, value);
}

void timeFromHourText(char text[6], byte val) {
	// "9" >> "09:00"
	// "21" >> "21:00"
	char value_txt[6];
	if (val < 10) {
		strcpy(text, "0");
	} else {
		strcpy(text, "");
	}
	itoa(val, value_txt, 10);
	strcat(text, value_txt);
	strcat(text, ":00");
}

void displaySettingsValue() {
	// Display position
	char text[DISPLAY_TEXT_WIDTH_2];
	char value[DISPLAY_TEXT_WIDTH_2];
	strcpy(text, SETTINGS_MENU[state.menu_position - 1]);

	// Display value
	switch (state.menu_position) {
		case 0:
			// No position
			break;
		case 1:
			//	Hours
			timeFromHourText(value, settings.day_start_hour);
			break;
		case 2:
			//	Hours
			timeFromHourText(value, settings.day_end_hour);
			break;
		case 3:
			//	sensor
			itoa(settings.soil_sensor_zero, value, 10);
			break;
		case 4:
			//	sensor
			itoa(settings.soil_sensor_full, value, 10);
			break;
		case 5:
			//	leakage finish
			itoa(settings.leakage_finish_delay, value, 10);
			break;
	}
	displaySmallLine(text, 0); // Parameter on the left
	if (state.menu_position > 2) {
		int sensor_value = analogRead(SOIL_SENSOR_PINS[0]);
		char sensor_text[4];
		itoa(sensor_value, sensor_text, 10);
		strcat(value, "|now");
		strcat(value, sensor_text);
	}
	displaySmallLine(value, 1); // Value on the right
}

// Flower Screen

void displayFlowerScreen(byte flower_num) {
	// Title
	char title[DISPLAY_TEXT_WIDTH_2];
	char position_text[DISPLAY_TEXT_WIDTH];
	titleText(flower_num, title);

	switch (state.menu_function) {
		case 0:
			// Main menu
			displayFlowerBigTitle(title);
			displayFlowerData(flower_num);
			break;
		case 1:
			// Position change
			displayFlowerSmallTitle(title);
			for (byte i = state.menu_position_slide; i < state.menu_position_slide + DISPLAY_POSITIONS; i = i + 1) {
				bool active = false;
				// Is Active
				if (i == state.menu_position - 1) {active = true;};

				if (i == 1 && flowerWateringQueueNow(flower_num)) {
					// Watering
					// Stop watering the flower
					strcpy(position_text, FLOWER_MENU_STOP_WATERING);
				} else if ((i == 1 && state.plate_full[flower_num])) {
					// Watering
					// Already full of water
					strcpy(position_text, FLOWER_MENU_FULL_OF_WATER);
				} else {
					positionText(position_text, i, flower_num); // text preparation
				}
				displayPosition(position_text, i - state.menu_position_slide, active);
			}
			break;
		case 2:
			// Position change or schedule
			displayFlowerSmallTitle(title);
			displayFlowerValue(flower_num);
			break;
	}
}

void displayFlowerBigTitle(char *text) {
	displaySmallTitle(text);
}

void displayFlowerSmallTitle(char *text) {
	displaySmallTitle(text);
}

void titleText(byte flower_num, char text[10]) {
	itoa(flower_num + 1, text, 10);
	strcat(text, " ");
	strcat(text, FLOWER_NAMES[flower_num]);
}

void displayPosition(char *text, byte pos, bool active) {
	displayMenuLine(text, pos, active, true);
}

void positionText(char* text, byte pos, byte flower_num) {
	strcpy(text, FLOWER_MENU[pos]);
	char value[DISPLAY_TEXT_WIDTH_2];
	switch (pos) {
		case 0:
			// Schedule
			break;
		case 1:
			// Water the flower
			break;
		case 2:
			periodValueText(value, flowerData[flower_num].period);
			strcat(text, ": ");
			strcat(text, value);
			break;
		case 3:
			soilNumberText(value, flowerData[flower_num].soil_num);
			strcat(text, ": ");
			strcat(text, value);
			break;
		case 4:
			if (flowerData[flower_num].soil_num != -1) {
				sensorValueText(value, flowerData[flower_num].sensor);
			} else {
				strcpy(value, "never");
			}
			strcat(text, ": ");
			strcat(text, value);
			break;
		case 5:
			volumeValueText(value, flowerData[flower_num].volume);
			strcat(text, ": ");
			strcat(text, value);
			break;
	}
}

void displayFlowerValue(byte flower_num) {
	// Display position
	char text[DISPLAY_TEXT_WIDTH_2];
	char value[DISPLAY_TEXT_WIDTH_2];
	strcpy(text, FLOWER_MENU[state.menu_position - 1]);

	// Display value
	switch (state.menu_position) {
		case 0:
			// No position
			break;
		case 1:
			// Schedule
			displaySchedule(flower_num);
			break;
		case 2:
			// Water the flower
			break;
		case 3:
			periodValueText(value, flowerData[flower_num].period);
			break;
		case 4:
			soilNumberText(value, flowerData[flower_num].soil_num);
			break;
		case 5:
			if (flowerData[flower_num].soil_num != -1) {
				sensorValueText(value, flowerData[flower_num].sensor);
			} else {
				strcpy(value, "not set");
			}
			break;
		case 6:
			volumeValueText(value, flowerData[flower_num].volume);
			break;
	}
	if (state.menu_position > 2) {
		displaySmallLine(text, 0); // Parameter on the left
		strcpy(text, "");
		displaySmallLineLR(text, value, 1); // Value on the right
	}
}

void displaySchedule(byte flower_num) {
	char text[DISPLAY_TEXT_WIDTH];
	char text1[DISPLAY_TEXT_WIDTH];
	char text2[DISPLAY_TEXT_WIDTH];
	int8_t last_time;

	// Now time - Next watering
	flowerNextWateringTimeText(text, flower_num);
	strcpy(text1, "next: ");
	strcat(text1, text);
	displayMenuLineLR(timeText(rtc.getTime()), text1, 0, false); // unselected

	for (byte i = 0; i < 4; i += 1) {
		last_time = flowerData[flower_num].last_time - state.menu_schedule_position - i;
		if (last_time < 0) {
			last_time += FLOWER_SCHEDULE_COUNT;
		} else if (last_time > (FLOWER_SCHEDULE_COUNT -1)) {
			last_time -= FLOWER_SCHEDULE_COUNT;
		}

		dateToText(text, flowerData[flower_num].water_time[last_time]);
		itoa(i + state.menu_schedule_position + 1, text1, 10); // Number
		byte humidity = flowerData[flower_num].water_humidity[last_time];
		if (humidity == 255) { // No humidity
			strcpy(text2, "");
		} else {
			itoa(humidity, text2, 10); // Humidity percentage
			strcat(text2, "%");
		}
		strcat(text1, " ");
		strcat(text1, text);
		displayMenuLineLR(text1, text2, i + 1);
	}
}

void displayFlowerData(byte flower_num) {
	char humid[DISPLAY_TEXT_WIDTH_2];
	char text[DISPLAY_TEXT_WIDTH_2];
	// Humidity
	int8_t soil_num = flowerData[flower_num].soil_num;
	strcpy(humid, "");
	if (soil_num != -1) {
		itoa(state.soil_humidity[soil_num], humid, 10);
		strcat(humid, "%");
	}
	// Normal mode
	if (flowerConnection[flower_num].connected && !flowerWateringQueueNow(flower_num)) {
		flowerWateringTimeText(text, flower_num);
	// Action mode
	} else {
		flowerAction(text, flower_num);
	}
	displaySmallLine(humid, 0);
	displaySmallLine(text, 1);
}

void flowerWateringTimeText(char* text, byte flower_num) {
	// Write last watering time to text
	long val = timeDelta(lastWaterTime(flower_num), rtc.getTime());
	// Watered some time before
	secondsValueToText(text, val, false); // without seconds
}

void flowerNextWateringTimeText(char* text, byte flower_num) {
	// Write last watering time to text
	if (tasks.water_time[flower_num] == 0) {
		strcpy(text, "--");
	} else if ((tasks.water_time[flower_num] - millis()) < TASK_CHECK_DELAY) { // water time >= now
		uint32_t val = tasks.water_time[flower_num] - millis();
		val = val / 1000ul;
		secondsValueToText(text, val, false); // without seconds
	} else {
		strcpy(text, "now");
	}
}

void flowerAction(char* text, byte flower_num) {
	if (!flowerConnection[flower_num].connected) {
		strcpy(text, "failed");
	} else if (flowerWateringQueueNow(flower_num)) {
		strcpy(text, "wait..");
	}
}

// Power and Lighting

void clearDisplay() {
	display.clear();
}

void dimDisplay(bool dim) {
	if (dim) {
		display.setContrast(0);
	} else {
		display.setContrast(255);
	}
}

void displayOn(){
	power.hardwareEnable(PWR_SPI);
	display.setPower(true);
	dimDisplay(false);
	state.display_upd_count = 0;
}

void displayOff(){
	clearDisplay();
	display.setPower(false);
	power.hardwareDisable(PWR_SPI);
}
