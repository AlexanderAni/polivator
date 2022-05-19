#pragma once

#include <PCF8574.h>
#include <microDS3231.h>
#include <Adafruit_AHTX0.h> // Temp and Humidity sensor

#include "structure.h"

#define PROG_NAME "Polivator"
#define MAJOR_VER 1
#define MINOR_VER 10

// Time
// minute: 60 000, hour: 3 600 000. max 4 294 967 296
#define SLEEP_DELAY 600000 // Sleep delay, milliseconds / 10 min
#define AWAKE_DELAY 300 // Awake delay, milliseconds
#define AWAKE_TIME 20000 // Time before going to sleep, milliseconds
#define SCHEDULE_AWAKE_TIME 60000 // Time before going to sleep, milliseconds
#define DIM_TIME 1500 // Time to dim before going to sleep, milliseconds
#define DISPLAY_UPDATE_COUNT 5 // Count skiped before update display
#define SENSOR_CHECK_DELAY 5000 // Time delay for check temp humid and other sensors
#define WATER_CHECK_DELAY 3600000 // Time delay for check water level
// 1 000 000 000 is about 11.5 days delay check
// millis is `unsigned long type` 4 294 967 295 - 1 000 000 000 = 3 294 967 295 it is about 38 days to the next task
#define TASK_CHECK_DELAY 1000000000 // working with millis()
#define LEAKAGE_FINISH_DELAY 10 // seconds. max 65 535. Delay to check if leakage is finished. LEAKAGE_FINISH_DELAY=0 for stop forewer

#define HOT_DRY_TEMP 30 // More than 30c temperature
#define HOT_DRY_HUMID 30 // Less than 30% of humidity

#define DAY_START_HOUR 9
#define DAY_END_HOUR 23
// This year and time number will be "never"
#define NEVER_YEAR 65535
#define NEVER_TIME 2147483647

// I2C Defenition
#define I2C_SDA_PIN A4
#define I2C_SDL_PIN A5

// Flower pcf8574 Connectors
// pcf8574 connector contains 2 flowers on 8 pins:
// 0 - slot 0: Valve
// 1 - slot 0: Soil sensor
// 2 - slot 0: Water fill sensor
// 3 - slot 0: Water plate fill sensor
// 4 - slot 1: Valve
// 5 - slot 1: Soil sensor
// 6 - slot 1: Water fill sensor
// 7 - slot 1: Water plate fill sensor
const byte CONNECTORS_NUM = 1;
// Connector classes with I2C addresses
PCF8574 connectors[CONNECTORS_NUM] = {
	PCF8574(0x24),
};
byte flower_connectors[CONNECTORS_NUM][2] = {
	{0, 1}, // flowers 0 and 1 on connector 1
	// {2, 3}, // flowers 2 and 3 on connector 2
	// {4, 5}, // flowers 4 and 5 on connector 3
	// {6, 7}, // flowers 6 and 7 on connector 4
	// {8, 9}, // flowers 8 and 9 on connector 5
	// {x, -1}, // only flower x on connector slot 0
	// {-1, x}, // only flower x on connector slot 1
};
boolean connector_enable[CONNECTORS_NUM];

// Same PIN order as SSD1306
#define SPI_CLK_PIN   13
#define SPI_MOSI_PIN  11
#define SPI_RESET_PIN 8
#define SPI_DC_PIN    9
#define SPI_CS_PIN    10

// Water Tank
#define PUMP_SPEED 3 // Speed, millilitre per second, max 255 (0.255 litre)

// Pins
#define PUMP_PIN 7 // Pump
#define PUMP_PIN2 6 // Pump GND
#define ENC_SW_PIN 4
#define ENC_A_PIN 5
#define ENC_B_PIN 3 // With interrupt
#define LEAKAGE_INTERRUPT_PIN 2 // Leakage sensor
#define WATER_SENSOR_PIN 12 // Power to activate water level sensor
#define WATER_LEVEL_PIN A7 // Water level sensor input
#define WATER_LEVEL_TIME 2000 // Water level wait time. Seconds

void stateSetup() {
	// Water
	// state.water_level_ml = 10000; // Water level in tank, millilitre, max 65 535 (65.5 litre)
	// state.water_level_percent = 100; // Water level in tank, 0-100%
	// Time
	state.sleep_delay = AWAKE_DELAY; // Normal sleep delay
}

// 2 flowers
const byte FLOWER_COUNT = 2;
flowerDataStruct flowerData[FLOWER_COUNT];
flowerConnectionStruct flowerConnection[FLOWER_COUNT];

// Quantity matches with FLOWER_COUNT
const char fl_name_str1[] PROGMEM = "Chlorophy";
const char fl_name_str2[] PROGMEM = "Gerbera";
// const char fl_name_str3[] PROGMEM = "";
const char *const FLOWER_NAMES[] PROGMEM = {
	fl_name_str1, fl_name_str2,
};


void flowerSetup() {
	// 1: Volume, ml;
	// 2: Period,
	// 		0 - never
	// 		1..20 - every 1..20 hours
	// 		21..51 - every 1 - 30 days (x-20)
	// 		example: 6 - every 6 hours, 12 - every 12 hours, 21 - every day, 33 - every 13 days
	// 3: Period while Hot and Dry climat
	// Quantity matches with FLOWER_COUNT
	flowerData[0] = (flowerDataStruct) {50, 12, 6};
	flowerData[1] = (flowerDataStruct) {50, 12, 6};
	// flowerData[2] = (flowerDataStruct) {1000, 6, 20};
	// flowerData[3] = (flowerDataStruct) {1000, 0, 0};
	// flowerData[4] = (flowerDataStruct) {1000, 9, 100};
	// flowerData[5] = (flowerDataStruct) {100, 10, 20};
	// flowerData[6] = (flowerDataStruct) {100, 11, 20};
	// flowerData[7] = (flowerDataStruct) {1000, 12, 20};
	// flowerData[8] = (flowerDataStruct) {1000, 22, 20};
	// flowerData[9] = (flowerDataStruct) {1000, 41, 20};
};

// Menu

const byte POSITION_COUNT = 5;
const byte DISPLAY_POSITIONS = 4; // How many positions display
const char mnu_str1[] PROGMEM = "Show schedule";
const char mnu_str2[] PROGMEM = "Water the flower";
const char mnu_str3[] PROGMEM = "Period";
const char mnu_str4[] PROGMEM = "HotDry Period";
const char mnu_str5[] PROGMEM = "Volume";
const char *const FLOWER_MENU[] PROGMEM = {
	mnu_str1, mnu_str2, mnu_str3, mnu_str4, mnu_str5
};
const byte SETTINGS_COUNT = 7;
const char s_str1[] PROGMEM = "HotDry temp";
const char s_str2[] PROGMEM = "HotDry humid";
const char s_str3[] PROGMEM = "Day starts";
const char s_str4[] PROGMEM = "Day ends";
const char s_str5[] PROGMEM = "Pump speed";
const char s_str6[] PROGMEM = "Check pump 200ml";
const char s_str7[] PROGMEM = "leak ends";
const char *const SETTINGS_MENU[] PROGMEM = {
	s_str1, s_str2, s_str3, s_str4, s_str5, s_str6, s_str7,
};

const char PROGMEM STOP_WATERING[] = "Stop watering";
const char PROGMEM STR_NEVER[] = "never";
const char PROGMEM STR_NOW[] = "now";
const char PROGMEM STR_DAY[] = "day";
const char PROGMEM STR_NIGHT[] = "night";
const char PROGMEM STR_HOTDRY[] = "Hot Dry";
const char PROGMEM STR_LEAKAGE[] = "Leakage";
const char PROGMEM STR_WATERING[] = "Watering";
const char PROGMEM STR_STOPPED[] = "Stopped";
const char PROGMEM STR_CANCELLED[] = "Cancelled";
const char PROGMEM STR_PUMP_CHECK[] = "Pump check";
const char PROGMEM STR_PUMP_200ML[] = "200ml | ";
const char PROGMEM STR_PUMP_ERROR[] = "PUMP ERROR";
const char PROGMEM STR_VALVE_ERROR[] = "VALVE ERROR";
const char PROGMEM STR_PUMP_STOP_ERROR[] = "Cannot stop pump";
const char PROGMEM STR_MISSING[] = "Missing";
const char PROGMEM STR_PLANNED[] = "Planned";
const char PROGMEM STR_FINISHED[] = "Finished";
const char PROGMEM STR_NOWATER[] = "No water";
const char PROGMEM STR_WATERFULL[] = "Water full";
const char PROGMEM STR_DISABLED[] = "Disabled";
const char PROGMEM STR_FAILED[] = "Failed";
const char PROGMEM STR_WAIT[] = "Wait ...";
const char PROGMEM STR_LAST[] = "Last: ";
const char PROGMEM STR_NEXT[] = "Next: ";
const char PROGMEM STR_LEFT[] = "Left: ";
const char PROGMEM STR_COLON[] = ": ";
const char PROGMEM STR_S[] = "s";
const char PROGMEM STR_MLS[] = "ml/s";
const char PROGMEM STR_ZERO[] = "0";
const char PROGMEM STR_DASHDASH[] = "--";
const char PROGMEM STR_MEMORY[] = "Memory";
const char PROGMEM STR_MEMORY_LOADED[] = "Loaded data";
const char PROGMEM STR_MEMORY_MISSING[] = "Missing data";
const char PROGMEM STR_TEMP_SENSOR[] = "Temp & Humid sensor";

// Time

MicroDS3231 rtc;

// Temp & humidity sensor

Adafruit_AHTX0 aht;
