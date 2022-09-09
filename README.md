# POLIVATOR by AlexanderAni

Arduino, AVR, C++

[Russian/Русский](/README_RU.md)

## The watering machine is independent
`How the Polivator works`

This device is for watering up to 8 different colors according to a given schedule.
The waterer can be left alone to water the flowers during the holidays or simply water the flowers in the home greenhouse year-round. Itself checks the presence of water in the tank and controls the watering of each flower according to the schedule. This takes into account the temperature and humidity of the environment. The display allows you to check the current status and change the watering settings.
The features of the Polivator are an expandable modular system, the preservation of the history of watering, and work without pauses for watering a particular plant. During irrigation, all sensors continue to be polled and the schedule is checked. You can view the schedule or change settings without waiting for the end of watering.
Powering the entire system from a single usb 5v block. Power saving mode allows you to work on battery power.

[Facebook. Premiere](https://www.facebook.com/aanishchenko/posts/pfbid034qEHsVyJWHPmqgtMWfscPb4xsddgqDzVarsc6yDjJoVNN26u15hXLRgtBCGTqUbWl)
[Video. Full story about the device](https://www.facebook.com/100003967933465/videos/pcb.2457044897771084/1915227252002448)
[Video. Watering example](https://www.facebook.com/100003967933465/videos/3267990133452066/)
[Video. Menu Example](https://www.facebook.com/100003967933465/videos/pcb.2457044897771084/1137887480090560)

## Politator and his assistants
`What is included in the irrigation complex`

1. Main unit
- Block with 1.3 inch LCD display SSH1106 controlled by arduino pro mini 5v
- Management of all actions with the encoder
- DS3231 clock module
Dimensions:

The main unit is wired on a 2-sided board measuring 50x80mm. From the back there are connectors for connecting sensors, a clock module and a connector for connecting a 4-wire I2c wire.

2. Connectors for flowers. 4 things
- Small connector for 2 flowers based on the port expander PCF8574
- 2 normally closed water valves
- 2 water sensors
Dimensions:

The connectors are wired on a double-sided board measuring 20x50mm. There are holes on the board for connecting the "sandwich" of the pcf8574 board. Each connector has a loop through to the I2c wire.

3. Water tank
- 19 liter cooler bottle
- Pump powered by usb 5v
- Sensory water presence sensor

4. Pipeline
- Silicone hoses with an inner diameter of 6mm and 4mm

## We assemble ourselves
`Links to resources for self-assembly of the device`

- [github. Polivator code for arduino pro mini][]
- [EasyEDA. Polivator Main. Main block. Schematic, PCB layout][]
- [EasyEDA. Polivator Connector Type A. Schematic, PCB layout][]
- [github. Water Level T13. Water level sensor in the tank. Code for Attiny13][]
- [EasyEDA. Water Level T13. Water level sensor in the tank. Schematic, PCB layout][]

## Code structure
`Principal code structure`

Description will be added later

## What else can you do?
`Opportunities`

### Limitation to two color variations per connector
The PCF8574 connector has 8 I/O pins.
<!-- An npn mosfet is used to securely connect valves or sensors. -->
For one flower, 3 pins are used:
- Water opening valve
- Enable water sensor
- Water sensor measurement
The water sensor is switched on periodically only to poll the status. The rest of the time it is off. This mode allows you to save the contacts of the sensors, which are in the water for a long time, from galvanic oxidation of the contacts.

### Limited to 4 connectors and 8 colors
The limitations are primarily related to the size of the RAM for the ATmega328p
You can change the number of connectors and the number of `FLOWER_SCHEDULE_COUNT` history storage cells. While maintaining the balance of used RAM, increasing one, decreasing the second.

Calculation of allocated memory in platformio:
RAM: [======== ] 82.4% (used 1687 bytes from 2048 bytes)
Flash: [========= ] 93.8% (used 28830 bytes from 30720 bytes)
The maximum estimated RAM at which no errors occurred during testing: 84.8% (used 1736 bytes).

Theoretically, you can use a large number of connectors with atmega microcontrollers with more RAM. For example ATmega64 with 4 kilobytes of RAM.

###Todo list

- Select flower name via menu
