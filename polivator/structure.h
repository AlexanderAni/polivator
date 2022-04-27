// Default setup definitions

#pragma once
#define FLOWER_SCHEDULE_COUNT 10
// Structure Defenition

struct flowerDataStruct {
	uint16_t volume; // Volume, millilitre, max 65 535 (65.5 litre)
	byte period; // Water period, max 63
	byte humid; // Water humid limit, 0-100%
	DateTime water_time[FLOWER_SCHEDULE_COUNT]; // several last times of watering
	byte water_humidity[FLOWER_SCHEDULE_COUNT]; // several last humidities before watering
	int8_t water_temp[FLOWER_SCHEDULE_COUNT]; // several last temp before watering. -128..127, celsium
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
	bool save_data; // Need save data to memory
	bool tempSensor; // temp sensor loaded
	// Time
	uint32_t last_active_time; // Time in millis()
	uint32_t sensor_check_time; // Time in millis()
	uint32_t water_check_time; // Time in millis()
	byte display_upd_count; // Display update counter
	uint32_t sleep_delay; // Sleep delay in milliseconds, max 65 535
	// Water
	bool water_level; // Water level
	// Environment
	int humidity; // percent
	int8_t temp; // degree, -128..+127
	// Time
	// Menu
	byte menu_function; // 0-screen selector, 1-position selector, 2-position changing
	byte menu_screen; // Selected screen
	byte menu_position; // Selected position
	byte menu_position_slide; // Selected screen slide position
	byte menu_schedule_position; // Selected position
	// Task
	int8_t active_watering; // Active watering. `-1` is disabled. There can be only one flower watering at one time
	byte active_water_humidity; // Humidity at the start of Active watering
	byte active_water_temp; // Temperature at the start of Active watering
	uint32_t active_watering_stop_time; // time when flower watering will stop, millis()
	uint32_t leakage_time; // Leakage was detected. millis

} volatile state;

struct taskStruct {
	// FLOWER_COUNT = 10
	// if water_time[flower_num] != 0 and time is now or missing - open the valve, pump on
	// if water_time + water_duration is over - close valve, pump off
	uint32_t water_time[10]; // water task time, millis()
	int water_duration[10]; // water task duration, seconds()
} tasks;

struct globalSettings {
	byte day_start_hour;
	byte day_end_hour;
	byte pump_speed;
	uint16_t leakage_finish_delay;
} settings;
