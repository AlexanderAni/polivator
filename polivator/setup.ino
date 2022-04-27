// Setup

void interfaceSetup() {
	// Default analog input reference 5v (Arduino Pro mini 5v)
	analogReference (DEFAULT);
	// Pump
	pinMode(PUMP_PIN, OUTPUT);
	pinMode(PUMP_PIN2, OUTPUT);
	digitalWrite(PUMP_PIN, LOW);
	digitalWrite(PUMP_PIN2, LOW);
	// Sensors
	pinMode(LEAKAGE_INTERRUPT_PIN, INPUT);
	pinMode(WATER_SENSOR_PIN, OUTPUT);
	digitalWrite(WATER_SENSOR_PIN, LOW);
	pinMode(WATER_LEVEL_PIN, INPUT_PULLUP);
	// Encoder
	encSetup();
	// Watchdog.enable(INTERRUPT_RESET_MODE, WDT_PRESCALER_2);  // Combined watchdog, timeout ~16ms
}

void disableInterrupts() {
	// Detach interrupt for reading buttons
	detachInterrupt(digitalPinToInterrupt(ENC_B_PIN));
	detachInterrupt(digitalPinToInterrupt(LEAKAGE_INTERRUPT_PIN));
}

void enableInterrupts() {
	// Attach interrupt for reading buttons
	attachInterrupt(digitalPinToInterrupt(ENC_B_PIN), encoderInterrupt, RISING);
	if (!state.water_leak) {
		attachInterrupt(digitalPinToInterrupt(LEAKAGE_INTERRUPT_PIN), leakageInterrupt, RISING);
	}
}

void powerSetup() {
	power.autoCalibrate(); // Calibrate sleep timer
	power.setSleepMode(POWERDOWN_SLEEP); // Sleep powerdown sleep mode
	power.correctMillis(true); // Correct millis() after sleep
}

// Settings

void defaultSettings() {
	settings.day_start_hour = DAY_START_HOUR;
	settings.day_end_hour = DAY_END_HOUR;
	settings.pump_speed = PUMP_SPEED;
	settings.leakage_finish_delay = LEAKAGE_FINISH_DELAY;
}

// State setup

void defaultState() {
	state.water_allowed = true; // Water is allowed before verification
	state.water_leak = false; // Water leak is disabled
	state.sleep_mode = false; // Sleep mode is disabled
	state.water_level = false; // No water default
	state.save_data = false;
	state.last_active_time = millis(); // Awake from start (now)
	state.sensor_check_time = millis() - SENSOR_CHECK_DELAY; // now
	state.water_check_time = millis() - WATER_CHECK_DELAY; // now
	state.leakage_time = 0;
	// Menu
	state.menu_function = 0; // 0-screen selector, 1-position selector, 2-position changing
	state.menu_screen = 0; // // Selected screen
	state.menu_position = 0; // Selected position
	state.menu_position_slide = 0; // Position slide to fit the screen
	state.menu_schedule_position = 0; // Position of schedule
	// Environment
	// state.day_part = 0; // 0-undefined, 1-day, 2-night
	state.humidity = 0; // percent
	state.temp = 0; // degree, -128..+127
	state.tempSensor = false;

	// Flower tasks, last water time
	DateTime no_time;
	no_time.year = NEVER_YEAR;


	for (byte i = 0; i < FLOWER_COUNT; i++) {
		// Default all flowers are never watered
		for (byte i1 = 0; i1 < FLOWER_SCHEDULE_COUNT; i1 += 1) {
			flowerData[i].water_time[i1] = no_time;
			flowerData[i].water_humidity[i1] = 255;
			flowerData[i].water_temp[i1] = 127;
		}
		flowerData[i].last_time = 0;
		tasks.water_time[i] = 0;
		state.flower_water_sensor[i] = true; // Default not to water
	}

	state.active_watering = -1; // off
	// Connector pins and slots
	for (byte i = 0; i < CONNECTORS_NUM; i = i + 1) {
		// Valve defalults
		// Just to enable connection
		connectors[i].pinMode(P0, OUTPUT, LOW); // valve
		connectors[i].pinMode(P1, OUTPUT, LOW); // water sensor
		connectors[i].pinMode(P2, INPUT); // water sensor
		connectors[i].pinMode(P4, OUTPUT, LOW); // valve
		connectors[i].pinMode(P5, OUTPUT, LOW); // water sensor
		connectors[i].pinMode(P6, INPUT); // water sensor
		// Connector check
		if (connectors[i].begin()) {
			connector_enable[i] = true;
			// Slot 0
			if (flower_connectors[i][0] != -1) {
				flowerConnection[flower_connectors[i][0]].connector = i; // Set number of connector
				flowerConnection[flower_connectors[i][0]].conn_slot = 0; // Set slot of the connector to 0
				flowerConnection[flower_connectors[i][0]].connected = true; // Set flower connected
				// Pin Mode
				connectors[i].digitalWrite(P0, LOW); // Valve
				// Serial.print("Slot 0: flower ");
				// Serial.println(flower_connectors[i][0]);
				// Serial.flush();
			}
			// Slot 1
			if (flower_connectors[i][1] != -1){
				flowerConnection[flower_connectors[i][1]].connector = i; // Set number of connector
				flowerConnection[flower_connectors[i][1]].conn_slot = 1; // Set slot of the connector to 1
				flowerConnection[flower_connectors[i][1]].connected = true; // Set flower connected
				// Pin Mode
				connectors[i].digitalWrite(P4, LOW); // Valve
				// Serial.print("Slot 1: flower ");
				// Serial.println(flower_connectors[i][1]);
				// Serial.flush();
			}
		} else {
			// Serial.print("Connector disabled: ");
			// Serial.println(i);
			// Serial.flush();
			connector_enable[i] = false;
		}
	}
}

void sensorSetup() {
	char title[11];
	char text[21];
	if (! aht.begin()) {
		strcpy(title, "Missing");
		strcpy(text, "Temp & Humid sensor");
		displayMessage(title, text, 2000);
	} else {
		state.tempSensor = true;
	}
}
