#include "config.h"
#include <GyverPower.h> // Power and Sleep Mode Library
#include <Wire.h>
#include <TimerMs.h>
#include <avr/pgmspace.h>

TimerMs tmr(AWAKE_DELAY, 0, 0);

// MAIN

void saveWaterState(byte flower_num, DateTime time, byte err=0);

void setup() {
	Serial.begin(9600);
	// Serial.println("Setup ...");
	state.memory_allowed = true;
	defaultSettings();
	displaySetup();
	displayHello();
	// rtcSetupTime(); // time autosetup by compilation time
	rtcSetup();
	displayHello();
	flowerSetup();
	defaultState();
	readFlowerData();
	stateSetup();
	sensorSetup();
	displayHello();
	powerSetup();
	// Start
	interfaceSetup();
	// Serial.println("Setup finished");
	delay(1500);
	clearDisplay();
	wasActive();
	enableInterrupts();
	tmr.start();
}

void loop() {
	encoderCheck();
	if (tmr.tick()) {
		// Use tmr to do slow job with AWAKE_DELAY
		// Slow job does not work with encoder
		allwaysDo();
		if (state.sleep_mode) {
			whileSleep();
		} else {
			whileAwake();
		}
	}
	// Sleep
	wannaSleep();
	if (state.sleep_mode) {
		power.sleepDelay(state.sleep_delay);
	}
}

// WORKER

void allwaysDo() {
	sensorCheck();
	taskCheck();
	taskWorker();
}

void whileSleep() {}

void whileAwake() {
	displayData();
}

void sensorCheck() {
	leakageCheck();
	checkPowerLost();
	checkSurroundSensors();
}

void leakageCheck() {
	// Leak check
	if ((millis() - state.leakage_time) > settings.leakage_finish_delay * 1000L) {
		bool leak;
		leak = digitalRead(LEAKAGE_INTERRUPT_PIN);
		// Check leakage for finish
		if (leak) {
			// Do all you need after leakage detected: stop pump, set state
			if (!state.water_leak) {
				leakageDetected();
			}
		} else if (settings.leakage_finish_delay != 0) {
			// Turn off leakage after it has finished
			state.water_leak = false;
			enableInterrupts();
		}
	}
}

void leakageInterrupt() {
	if (!state.water_leak) {
		disableInterrupts();
		// leakageDetected();
		power.wakeUp();
		awake();
		wasActive();
		enableInterrupts();
	}
}

void setWateringTask(byte flower_num, uint32_t time, int duration=0) {
	// Set a task for watering the flower at direct time
	// Duration in seconds. 0 = default calculation by the flower volume
	if (flowerConnection[flower_num].connected) {
		tasks.water_time[flower_num] = time;
		if (duration == 0) {
			duration = wateringDurationInSeconds(flower_num);
		}
		tasks.water_duration[flower_num] = duration;
		// Serial.print("Scheduled watering task for flower ");
		// Serial.println(flower_num);
		// Serial.flush();
	}
}

void setDefaultWateringTask(byte flower_num) {
	// Set default water task with default watering duration
	long seconds_from_last_watering;
	long water_period_sec;
	uint32_t water_time_millis;
	seconds_from_last_watering = timeDelta(lastWaterTime(flower_num), rtc.getTime());
	if (seconds_from_last_watering == NEVER_TIME) {
		// No water time
		seconds_from_last_watering = 0;
	}
	// Period in seconds
	water_period_sec = periodHourValue(flowerPeriod(flower_num)) * 3600L;
	water_period_sec = water_period_sec - seconds_from_last_watering;
	water_time_millis = water_period_sec * 1000ul;
	water_time_millis += millis();
	setWateringTask(flower_num, water_time_millis);
}

void stopWateringTask(byte flower_num, byte err=0) {
	// Stop a task for watering the flower
	tasks.water_time[flower_num] = 0;
	if (err != 0 || state.active_watering == flower_num) {
		// Save watering time if active watering has been stopped
		saveWaterState(flower_num, rtc.getTime(), err);
	}
	// Serial.print("Stop watering ");
	// Serial.println(flower_num);
	// Serial.flush();
	saveFlowerData();
}

void resetFlowerTasks() {
	// Reset all tasks for a new task calculations
	// Use after flower data changed
	for (byte i = 0; i < FLOWER_COUNT; i = i + 1) {
		tasks.water_time[i] = 0;
	}
}

void taskCheck() {
	// Check periods for each flower
	for (byte i = 0; i < FLOWER_COUNT; i = i + 1) {
		// Set next water time if period is not 0 and flower has been connected
		if (flowerConnection[i].connected && flowerPeriod(i) && tasks.water_time[i] == 0) {
			setDefaultWateringTask(i);
			// Serial.print(periodHourValue(flowerPeriod(i)));
			// Serial.println(F(" hours"));
			// Serial.flush();
		}
	}
}

int periodHourValue(byte period) {
	// Get period value in hours
	// 0 - never
	// 1..20 - every 1..10 hours
	// 21..51 - every 1 - 30 days (x - 20)
	// example: 6 - every 6 hours, 23 - every 3 days, 33 - every 13 days
	if (period == 0) {
		return 0;
	} else if (period < 21) {
		return period;
	} else {
		return (period - 20) * 24;
	}
}

int wateringDurationInSeconds(byte flower_num) {
	return flowerData[flower_num].volume / settings.pump_speed;
}

byte flowerPeriod(byte flower_num) {
	if (state.hot_dry) {
		return flowerData[flower_num].hot_dry_period;
	} else {
		return flowerData[flower_num].period;
	}
}


void taskWorker() {
	// Watering
	// state.active_watering is zero if nothing is active
	if (state.active_watering != -1) {
		if ((millis() - state.active_watering_stop_time) < TASK_CHECK_DELAY) { // now >= water time
			// Stop watering by time
			// state.active_watering = flower
			stopWateringTask(state.active_watering);
			// Serial.println("Watering time is up");
			// Serial.flush();
			stopActiveWatering();
		} else if (!state.water_allowed) {
			// Something happends and watering has not been finished
			// state.active_watering = flower
			// Put left water task time to the queue
			uint32_t delta_sec;
			delta_sec = state.active_watering_stop_time - millis(); // calc milliseconds
			if (delta_sec < TASK_CHECK_DELAY) { // Some time left
				delta_sec = delta_sec / 1000ul;
				setWateringTask(state.active_watering, millis(), delta_sec);
			}
			// Serial.println("Watering is not allowed");
			// Serial.flush();
			stopActiveWatering();
			// Stop the pump
		}
		// Otherwise continue watering and wait
	}
	if (state.water_allowed && (state.active_watering == -1)) {
		// Check next water task for each flower
		for (byte i = 0; i < FLOWER_COUNT; i = i + 1) {
			// Set active
			if (flowerWateringQueueNow(i)) {
				char title[11];
				PGM_P name = pgm_read_word(FLOWER_NAMES + i);
				strcpy_P(title, name);
				char text[21];
				if (state.water_level) {
					// Water Level check before starting. Not to get in check cycle
					waterLevelCheck();
					// Water sensor check before starting
					flowerWaterSensorCheck(i);
					if (state.water_level && !state.flower_water_sensor[i]) {
						// Start Watering
						state.active_watering = i; // active watering = flower
						state.active_water_humidity = state.humidity; // save humidity value before active watering
						state.active_water_temp = state.temp; // save temperature value before active watering
						state.active_watering_stop_time = millis() + (long)tasks.water_duration[i] * 1000;
						// Serial.print("Start watering flower ");
						// Serial.print(i);
						// Serial.print(": for ");
						// Serial.print(wateringDurationInSeconds(i));
						// Serial.println(" seconds");
						// Serial.flush();
						if (state.sleep_mode)  {
							awake();
							wasActive();
						}
						// Open the Valve
						if (openFlowerValve(i)) {
							// Then start the pump
							if (!startPump()) {
								// If the pump has not started
								state.active_watering = -1;
								// Serial.println("ERROR: Pump has not started");
								// Serial.flush();
								strcpy(text, "PUMP ERROR");
							} else {
								strcpy(text, "WATERING");
							}
						} else {
							// If the valve has not opened
							state.active_watering = -1;
							strcpy(text, "VALVE ERROR");
							// Serial.println("ERROR: Valve has not opened");
							// Serial.flush();
						}
						displayMessage(title, text, 1500);
						break;
					} else {
						// Skip task
						if (state.flower_water_sensor[i]) {
							// Because full of water
							stopWateringTask(i, 2);
						}
					}
				}
			}
		}
	}

	// Check water allowed
	if (!state.water_allowed) {
		// Allow when no leak
		// Allow at day time
		if (!state.water_leak && dayTime() && state.water_level) {
			state.water_allowed = true;
		}
	} else if (!dayTime() || !state.water_level) {
		state.water_allowed = false;
	}
}

bool flowerWateringQueueNow(byte flower_num){
	// Return true if flower is in watering queue
	if (tasks.water_time[flower_num] != 0 && (millis() - tasks.water_time[flower_num]) < TASK_CHECK_DELAY) { // now >= water time
		return true;
	}
	return false;
}

DateTime lastWaterTime(byte flower_num) {
	byte last_time;
	last_time = flowerData[flower_num].last_time;
	return flowerData[flower_num].water_time[last_time];
}

void saveWaterState(byte flower_num, DateTime time, byte err) {
	byte last_time;
	last_time = flowerData[flower_num].last_time;
	if (last_time == FLOWER_SCHEDULE_COUNT - 1) {
		last_time = 0;
	} else {
		last_time += 1;
	}
	flowerData[flower_num].water_time[last_time] = rtc.getTime();
	flowerData[flower_num].water_humidity[last_time] = state.active_water_humidity;
	flowerData[flower_num].water_temp[last_time] = state.active_water_temp;
	flowerData[flower_num].water_error[last_time] = err;
	flowerData[flower_num].last_time = last_time;
}

void leakageDetected() {
	// Do all you need after leakage detected: wakeup, stop pump, set state
	char title[11];
	char text[21];
	power.wakeUp();
	awake();
	wasActive();
	state.water_allowed = false;
	state.water_leak = true;
	state.leakage_time = millis();
	strcpy(text, "Water detected");
	// Stop the pump
	if (!stopPump()) {
		// Serial.println("ERROR: Pump");
		// Serial.flush();
		strcpy(text, "Cannot stop pump");
	}
	// Close all the valves
	for (byte i = 0; i < FLOWER_COUNT; i = i + 1) {
		// Close the Valve
		if (!closeFlowerValve(i)) {
			// Serial.print("ERROR: Valve ");
			// Serial.println(i);
			// Serial.flush();
			strcpy(text, "Valve error");
		}
	}
	strcpy(title, "Leakage");
	displayMessage(title, text, 2000);
}

void stopActiveWatering() {
	// Stop the pump
	if (!stopPump()) {
		// Serial.println("ERROR: Pump");
		// Serial.flush();
	}
	// Close the Valve
	if (!closeFlowerValve(state.active_watering)) {
		// Serial.println("ERROR: Valve");
		// Serial.flush();
	}
	state.active_watering = -1;
}

// PUMP

bool openFlowerValve(byte flower_num) {
	// Return true after success opened valve
	if (flowerConnection[flower_num].connected) {
		// Slot 0: P0
		// Slot 1: P4
		byte slot = flowerConnection[flower_num].conn_slot  * 4;
		connectors[flowerConnection[flower_num].connector].digitalWrite(slot, HIGH);
	} else {
		return false;
	}
	// Here must be a check for succsess opened
	// Serial.print("Valve ");
	// Serial.print(flower_num);
	// Serial.println(" opened");
	// Serial.flush();
	return true;
}

bool closeFlowerValve(byte flower_num) {
	// Return true after success closed valve
	if (flowerConnection[flower_num].connected) {
		// Slot 0: P0
		// Slot 1: P4
		byte slot = flowerConnection[flower_num].conn_slot  * 4;
		connectors[flowerConnection[flower_num].connector].digitalWrite(slot, LOW);
	} else {
		return false;
	}
	// Here must be a check for succsess closed
	// Serial.print("Valve ");
	// Serial.print(flower_num);
	// Serial.println(" closed");
	// Serial.flush();
	return true;
}

bool startPump() {
	// Return true after success started pump
	// First inverse to swing the pump
	digitalWrite(PUMP_PIN, LOW);
	digitalWrite(PUMP_PIN2, HIGH);
	delay(20);
	// Then normal
	digitalWrite(PUMP_PIN, HIGH);
	digitalWrite(PUMP_PIN2, LOW);
	// Here must be a check for succsess opened
	// Serial.println("Pump started");
	// Serial.flush();
	return true;
}
bool stopPump() {
	// Return true after success stopped pump
	// First stop
	digitalWrite(PUMP_PIN, HIGH);
	digitalWrite(PUMP_PIN2, HIGH);
	delay(20);
	// Then off
	digitalWrite(PUMP_PIN, LOW);
	digitalWrite(PUMP_PIN2, LOW);

	// Here must be a check for succsess stopped
	// Serial.println("Pump stopped");
	// Serial.flush();
	return true;
}

// Sensors

void checkSurroundSensors() {
	// Temperature and Humidity sensor
	if ((millis() - state.sensor_check_time) > SENSOR_CHECK_DELAY) {
		humidTempCheck();
		if (state.temp < settings.hot_dry_temp || state.humidity > settings.hot_dry_humid) {
			if (state.hot_dry == true) {
				// Reset all the tasks if climat changed
				resetFlowerTasks();
			}
			state.hot_dry = false;
		} else {
			if (state.hot_dry == false) {
				// Reset all the tasks if climat changed
				resetFlowerTasks();
			}
			state.hot_dry = true;
		}
		state.sensor_check_time = millis();
	}
	// Water level sensor
	if (dayTime() && (millis() - state.water_check_time) > WATER_CHECK_DELAY) {
		waterLevelCheck();
		for (byte i = 0; i < FLOWER_COUNT; i++) {
			flowerWaterSensorCheck(i);
		}
		state.water_check_time = millis();
	}
}

void humidTempCheck() {
	if (state.tempSensor == true) {
		sensors_event_t humidity, temp;
		aht.getEvent(&humidity, &temp);// Populate temp and humidity objects with fresh data
		state.temp = int(round(temp.temperature));
		state.humidity = int(humidity.relative_humidity);
	}
}

void waterLevelCheck() {
	digitalWrite(WATER_SENSOR_PIN, HIGH);
	delay(WATER_LEVEL_TIME);
	int level = analogRead(WATER_LEVEL_PIN);
	if (level > 512) {
		state.water_level = true;
	} else {
		state.water_level = false;
	}
	digitalWrite(WATER_SENSOR_PIN, LOW);
}

void flowerWaterSensorCheck(byte flower_num) {
	byte slot = flowerConnection[flower_num].conn_slot  * 4;
	// Sensor on
	connectors[flowerConnection[flower_num].connector].digitalWrite(slot + 1, HIGH);
	delay(20);
	// Sensor check
	state.flower_water_sensor[flower_num] = connectors[flowerConnection[flower_num].connector].digitalRead(slot + 2);
	// Sensor off
	connectors[flowerConnection[flower_num].connector].digitalWrite(slot + 1, LOW);
	delay(20);
}

void check_pump_speed() {
	// Water 200 ml
	char title[11];
	char text[21];
	char time_text[21];
	int time = 200 / settings.pump_speed;
	PGM_P name = pgm_read_word(FLOWER_NAMES);
	strcpy(title, "Pump check");
	strcpy(text, "200ml | ");
	itoa(time, time_text, 10);
	strcat(text, time_text);
	strcat(text, "s");
	displayMessage(title, text, 2000);

	strcpy_P(title, name);
	strcpy(text, "Watering ...");
	displayMessage(title, text, 0);
	// Water check
	openFlowerValve(0);
	startPump();
	delay(time * 1000L);
	stopPump();
	closeFlowerValve(0);
	strcpy(text, "Finished");
	displayMessage(title, text, 2000);
}
