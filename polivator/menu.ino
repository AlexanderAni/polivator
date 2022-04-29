// Menu surfing

void prevMenu() {
	clearDisplay();
	if (state.menu_screen <= 0) {
		state.menu_screen = FLOWER_COUNT;
	} else {
		state.menu_screen -= 1;
	}
}

void nextMenu() {
	clearDisplay();
	if (state.menu_screen >= (FLOWER_COUNT)) {
		state.menu_screen = 0;
	} else {
		state.menu_screen += 1;
	}
}

void pressMenu() {
	clearDisplay();
	state.menu_function = 1;
	state.menu_position = 1;
	state.menu_position_slide = 0;
}

// Position change

void prevPosition() {
	if (state.menu_position < 1) {
		// Nothing selected (0)
	} else {
		state.menu_position -= 1;
		if (state.menu_position != 0 && state.menu_position <= state.menu_position_slide) {
			state.menu_position_slide -= 1;
		}
	}
}

void nextPosition() {
	if ((state.menu_screen == 0 && state.menu_position >= (SETTINGS_COUNT)) || (state.menu_screen != 0 && state.menu_position >= (POSITION_COUNT))) {
		// Stop at the end
	} else {
		state.menu_position += 1;
		// DISPLAY_POSITIONS how many positions to display > menu scrolling
		if ((state.menu_position - state.menu_position_slide) > DISPLAY_POSITIONS) {
			state.menu_position_slide += 1;
		}
	}
}

void pressPosition() {
	clearDisplay();
	if (state.menu_position == 0) {
		state.menu_function = 0;
	} else {
		if (state.menu_screen == 0) {
			// Settings screen
			if (state.menu_position == 4) {
				// Check pump by water 200 ml
				check_pump_speed();
			} else {
				// Change settings
				state.menu_function = 2;
			}

		} else {
			// Flower screen
			byte flower_num = state.menu_screen - 1;
			if (state.menu_position == 2) {
				// Water the flower
				char text[21];
				char title[11];
				strcpy(text, "Watering");
				if (flowerWateringQueueNow(flower_num)) {
					// Already Watering now
					stopWateringTask(flower_num, 4); // Manually
					if (flower_num == state.active_watering) {
						strcpy(title, "Stopped");
						stopActiveWatering();
						displayMessage(title, text, 1500);
					} else {
						strcpy(title, "Cancelled");
						displayMessage(title, text, 1500);
					}
				} else {
					// Plan watering
					flowerWaterSensorCheck(flower_num);
					if (state.flower_water_sensor[flower_num]) {
						// Flower is full of water
						strcpy(title, "Missing");
						strcpy(text, "Full of water!");
						displayMessage(title, text, 1500);
					} else {
						// Water the flower
						strcpy(title, "Planned");
						setWateringTask(flower_num, millis());
						displayMessage(title, text, 1500);
					}
				}
			} else {
				// Change value
				state.menu_schedule_position = 0;
				state.menu_function = 2;
			}
		}
	}
}

// Values

void prevPositionValue() {
	if (state.menu_screen == 0) {
		// Settings
		switch (state.menu_position) {
			case 1:
			// Change hours
			if (settings.day_start_hour > 0) {
				settings.day_start_hour -= 1;
				state.save_data = true;
			}
			break;
			case 2:
			// Change hours
			if (settings.day_end_hour > settings.day_start_hour + 1) {
				settings.day_end_hour -= 1;
				state.save_data = true;
			}
			break;
			case 3:
			// Change pump speed
			if (settings.pump_speed > 0) {
				settings.pump_speed -= 1;
				state.save_data = true;
			}
			break;
			case 5:
			// Change leakage finish
			if (settings.leakage_finish_delay > 0) {
				settings.leakage_finish_delay -= 1;
				state.save_data = true;
			}
			break;
		}
	} else {
		// Flower
		byte flower_num = state.menu_screen - 1;
		switch (state.menu_position) {
			case 1:
			// Change schedule position
			if (state.menu_schedule_position < FLOWER_SCHEDULE_COUNT - 4) {
				state.menu_schedule_position += 1;
			}
			break;
			case 3:
			// Change period
			if (flowerData[flower_num].period > 0) {
				flowerData[flower_num].period -= 1;
				state.save_data = true;
			}
			break;
			case 4:
			// Change humid value
			if (flowerData[flower_num].humid > 0) {
				flowerData[flower_num].humid -= 5;
				state.save_data = true;
			}
			break;
			case 5:
			// Change volume
			if (flowerData[flower_num].volume > 2000) {
				flowerData[flower_num].volume -= 500;
				state.save_data = true;
			} else if (flowerData[flower_num].volume > 100) {
				flowerData[flower_num].volume -= 100;
				state.save_data = true;
			} else if (flowerData[flower_num].volume > 10) {
				flowerData[flower_num].volume -= 10;
				state.save_data = true;
			}
			break;
		}
	}
}

void nextPositionValue() {
	if (state.menu_screen == 0) {
		// Settings
		switch (state.menu_position) {
			case 1:
			// Change hours
			if (settings.day_start_hour < settings.day_end_hour - 1) {
				settings.day_start_hour += 1;
				state.save_data = true;
			}
			break;
			case 2:
			// Change hours
			if (settings.day_end_hour < 24) {
				settings.day_end_hour += 1;
				state.save_data = true;
			}
			break;
			case 3:
			// Pump speed
			if (settings.pump_speed < 255) {
				settings.pump_speed += 1;
				state.save_data = true;
			}
			break;
			case 5:
			// Leakage_finish_delay
			if (settings.leakage_finish_delay < 65535) {
				settings.leakage_finish_delay += 1;
				state.save_data = true;
			}
			break;
		}
	} else {
		// Flowers
		byte flower_num = state.menu_screen - 1;
		switch (state.menu_position) {
			case 1:
			// Change schedule position
			if (state.menu_schedule_position > 0) {
				state.menu_schedule_position -= 1;
			}
			break;
			case 3:
			// Change period
			if (flowerData[flower_num].period < 51) {
				flowerData[flower_num].period += 1;
				state.save_data = true;
			}
			break;
			case 4:
			// Change humid value
			if (flowerData[flower_num].humid < 100) {
				flowerData[flower_num].humid += 5;
				state.save_data = true;
			}
			break;
			case 5:
			// Change volume
			if (flowerData[flower_num].volume < 100) {
				flowerData[flower_num].volume += 10;
				state.save_data = true;
			} else if (flowerData[flower_num].volume < 2000) {
				flowerData[flower_num].volume += 100;
				state.save_data = true;
			} else if (flowerData[flower_num].volume < 65500) {
				flowerData[flower_num].volume += 500;
				state.save_data = true;
			}
			break;
		}
	}
}

void pressPositionValue() {
	clearDisplay();
	if (state.menu_screen == 0) {
		state.menu_function = 1;
		saveData();
	} else {
		if (state.menu_position == 1) {
			// From schedule
			// Back to position select
			state.menu_function = 1;
		} else if (state.menu_position == 2) {
			// Nothing happends
			// No position change on watering function
		} else {
			// From values changing
			// Back to position select
			state.menu_function = 1;
			saveData();
		}
	}
}

void saveData() {
	saveFlowerData();
	resetFlowerTasks();
}
