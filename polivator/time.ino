// MicroDS3231 Time management

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
		strcpy(title, "Missing");
		strcpy(text, "Time Sensor DS3231");
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
		strcpy(title, "Missing");
		strcpy(text, "DS3231 battery power");
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

bool dayTime() {
	byte hour = rtc.getTime().hour;
	if (hour > (DAY_START_HOUR - 1) && hour < DAY_END_HOUR) {
		return true;
	} else {
		return false;
	}
}
