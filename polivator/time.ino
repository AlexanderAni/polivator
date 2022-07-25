// MicroDS3231 Time management
const char PROGMEM STR_TIME_SENSOR[] = "Time Sensor DS3231";
const char PROGMEM STR_TIME_BATTERY[] = "DS3231 battery power";

void rtcSetupTime() {
	// Automatical time setup by compilation time
	// Setup once
	rtc.setTime(COMPILE_TIME);
}

void rtcSetup() {
	char title[11];
	char text[21];
	if (!rtc.begin()) {
		state.memory_allowed = false;
		strcpy_P(title, STR_MISSING);
		strcpy_P(text, STR_TIME_SENSOR);
		displayMessage(title, text, 2000);
	}
	checkPowerLost();
}

void checkPowerLost() {
	if (rtc.lostPower()) {
		// Battery power was lost
		state.memory_allowed = false;
		char title[11];
		char text[21];
		strcpy_P(title, STR_MISSING);
		strcpy_P(text, STR_TIME_BATTERY);
		displayMessage(title, text, 2000);
	}
}

long timeDelta(DateTime from, DateTime to) {
	// Returns delta time in seconds
	long delta = 0;
	static const uint8_t DAYS_IN_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	// If one of times is never
	if (from.year == NEVER_YEAR || to.year == NEVER_YEAR) {
		return NEVER_TIME; // never
	}

	// Seconds addition
	delta += (int8_t)to.second - from.second;
	// Minutes addition
	delta += (long)60 * (int8_t)(to.minute - from.minute);
	// Hours addition
	delta += (long)3600 * (int8_t)(to.hour - from.hour);
	// Days addition
	delta += (long)86400 * (int8_t)(to.date - from.date);
	// Months addition
	if (to.month > from.month) {
		byte month = from.month;
		while (month != to.month) {
			delta += (long)86400 * DAYS_IN_MONTH[month - 1];
			month += 1;
			if (month > 12) {
				month = 1;
			}
		}
	} else {
		byte month = to.month;
		while (month != from.month) {
			delta -= (long)86400 * DAYS_IN_MONTH[month - 1];
			month += 1;
			if (month > 12) {
				month = 1;
			}
		}
	}
	// Years addition
	delta += (long)31536000 * (int8_t)(to.year - from.year);
	// Here must be a leap year on 29 february addition
	// if (to.year == from.year && to.year == 2024 && from.month < 3 && to.month > 2 & ) {...}
	return delta;
}

// byte dayDelta(DateTime from, DateTime to) {
// 	// Returns delta days
// 	byte delta = 0;
// 	static const uint8_t DAYS_IN_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
// 	// If one of times is never
// 	if (from.year == NEVER_YEAR || to.year == NEVER_YEAR) {
// 		return 255; // never
// 	}

// 	// Days addition
// 	delta += to.date - from.date;
// 	// Months addition
// 	if (to.month > from.month) {
// 		byte month = from.month;
// 		while (month != to.month) {
// 			delta += DAYS_IN_MONTH[month - 1];
// 			month += 1;
// 			if (month > 12) {
// 				month = 1;
// 			}
// 		}
// 	} else {
// 		byte month = to.month;
// 		while (month != from.month) {
// 			delta -= DAYS_IN_MONTH[month - 1];
// 			month += 1;
// 			if (month > 12) {
// 				month = 1;
// 			}
// 		}
// 	}
// 	// Here must be a leap year on 29 february addition
// 	// if (to.year == from.year && to.year == 2024 && from.month < 3 && to.month > 2 & ) {...}
// 	return delta;
// }

bool dayTime() {
	byte hour = rtc.getTime().hour;
	if (hour > (settings.day_start_hour - 1) && hour < settings.day_end_hour) {
		return true;
	} else {
		return false;
	}
}
