// Default setup definitions

#pragma once

// Structure Defenition

struct flowerDataStruct {
	uint16_t volume; // Volume, millilitre, max 65 535 (65.5 litre)
	byte period; // Water period, max 63
	byte sensor; // Water by sensor, 0-100%
	byte soil_pin; // Soil humid sensor pin (A1, A2...)
	byte humidity; // Soil humid by sensor, 0-100%
	DateTime water_time[5]; // 5 last times of watering
	byte water_humidity[5]; // 5 last humidities before watering
	byte last_time; // A last time position from water_time array (circle)
};

struct flowerConnectionStruct {
	byte connector: 4; // Connector number, max 15
	bool conn_slot: 1; // Connector slot, 0- first, 1-second
	bool connected: 1; // Connected
};

struct stateStruct {
	bool memory_allowed; // Save is allowed
	bool water_allowed; // Water is allowed
	bool water_leak; // Water leak is active
	bool sleep_mode; // Sleep mode is active
	bool flowerDataChanged; // Need save data to memory
	bool tempSensor; // temp sensor loaded
	bool lightSensor; // light sensor loaded
	uint32_t last_active_time; // Time in millis()
	uint32_t sensor_check_time; // Time in millis()
	byte display_upd_count; // Display update counter
	uint32_t sleep_delay; // Sleep delay in milliseconds, max 65 535
	// Water
	// uint16_t water_level_ml; // Water level in tank, millilitre, max 65 535 (65.5 litre)
	// byte water_level_percent; // Water level in tank, 0-100%
	// Environment
	// byte day_part; // 0-undefined, 1-day, 2-night
	int humidity; // percent
	int8_t temp; // degree, -128..+127
	// Time
	// Menu
	byte menu_function; // 0-screen selector, 1-position selector, 2-position changing
	byte menu_screen; // Selected screen
	byte menu_position; // Selected position
	byte menu_position_slide; // Selected screen slide position
	// Task
	int8_t active_sensor; // Active flower sensor. `-1` is disabled. There can be only one analog soil moisure sensor measurement at one time
	uint32_t active_sensor_check_time; // time when flower sensor will be ready, millis()
	int8_t active_watering; // Active watering. `-1` is disabled. There can be only one flower watering at one time
	byte active_water_humidity; // Humidity before Active watering
	uint32_t active_watering_stop_time; // time when flower watering will stop, millis()
	bool plate_full[10];
	uint32_t leakage_time; // Leakage was detected. millis
	// bool water_full[10];

} volatile state;

struct taskStruct {
	// FLOWER_COUNT = 10
	// if sensor_time[flower_num] != 0 and time is now or missing - do sensor check
	// uint32_t sensor_time[10]; // flower sensor task_time, millis()
	// if water_time[flower_num] != 0 and time is now or missing - open the valve, pump on
	// if water_time + water_duration is over - close valve, pump off
	uint32_t water_time[10]; // water task time, millis()
	int water_duration[10]; // water task duration, seconds()
} tasks;

