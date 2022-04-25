// Sleep

void wannaSleep() {
	uint32_t awake_time = AWAKE_TIME;
	if (state.menu_screen != 0 && state.menu_function == 2 && state.menu_position == 1) {
		awake_time = SCHEDULE_AWAKE_TIME;
	}
	// Check if active time is too long
	if (!state.sleep_mode) {
		if ((millis() - state.last_active_time > awake_time) && state.active_watering == -1) {
			// Go to deep sleep only while watering is not active
			goSleep();
		} else if (millis() - state.last_active_time > awake_time - DIM_TIME) {
			dimDisplay(true);
		}
	}
	// Dim time
	// Select sleep delay
	if (state.sleep_mode) {
		state.sleep_delay = SLEEP_DELAY; // Deep sleeping delay
	} else {
		state.sleep_delay = AWAKE_DELAY; // Normal sleep delay
	}
}

void awake() {
	// Awake by interrupts
	power.wakeUp();
	state.sleep_mode = false;
	displayOn();
}

void wasActive() {
	state.last_active_time = millis(); // Awake time is set to 'now'
	state.sleep_mode = false;
	dimDisplay(false);
	state.display_upd_count = 0;
}

void goSleep() {
	// Serial.println("Sleep mode");
	// Serial.flush();
	state.sleep_mode = true;
	displayOff();
	state.menu_function = 0; // 0-screen selector, 1-position selector, 2-position changing
	state.menu_screen = 0; // // Selected screen, max 15
	state.menu_position = 0; // Selected position, max 15
	state.menu_position_slide = 0;
	if (state.save_data) {
		saveFlowerData();
		resetFlowerTasks();
	}
}

void mustSleep() {
	// Set sleep time little bit later (DIM_TIME)
	state.last_active_time = millis() - AWAKE_TIME + DIM_TIME;
	dimDisplay(true);
}
