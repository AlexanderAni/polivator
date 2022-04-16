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
	if (state.menu_screen == 0) {
		// Set sleep time 1 second after
		mustSleep();
	} else {
		state.menu_function = 1;
		state.menu_position = 1;
		state.menu_position_slide = 0;
	}
}

// Position change

void prevPosition() {
	if (state.menu_position <= 0) {
		// Nothing selected (0)
	} else {
		state.menu_position -= 1;
		if (state.menu_position != 0 && state.menu_position <= state.menu_position_slide) {
			state.menu_position_slide -= 1;
		}
	}
}

void nextPosition() {
	if (state.menu_position >= (POSITION_COUNT)) {
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
		if (state.menu_position == 2) {
			// Water the flower
			byte flower_num = state.menu_screen - 1;
			char text[21];
			char title[11];
			strcpy(text, "Watering");
			if (flowerWateringQueueNow(flower_num)) {
				stopWateringTask(flower_num);
				if (flower_num == state.active_watering) {
					strcpy(title, "Stopped");
					stopActiveWatering();
					displayMessage(title, text, 1500);
				} else {
					strcpy(title, "Cancelled");
					displayMessage(title, text, 1500);
				}
			} else if (!state.plate_full[flower_num]) {
				strcpy(title, "Planned");
				setWateringTask(flower_num, millis());
				displayMessage(title, text, 1500);
			}
		} else {
			// Change value
			state.menu_function = 2;
		}
	}
}

// Values

void prevPositionValue() {
	byte flower_num = state.menu_screen - 1;
	state.flowerDataChanged = true;
	switch (state.menu_position) {
		case 3:
		// Change period
		if (flowerData[flower_num].period > 0) {
			flowerData[flower_num].period -= 1;
		}
		break;
		case 4:
		// Change sensor
		if (flowerData[flower_num].sensor > 0) {
			flowerData[flower_num].sensor -= 5;
		}
		break;
		case 5:
		// Change volume
		if (flowerData[flower_num].volume > 2000) {
			flowerData[flower_num].volume -= 500;
		} else if (flowerData[flower_num].volume > 100) {
			flowerData[flower_num].volume -= 100;
		} else if (flowerData[flower_num].volume > 10) {
			flowerData[flower_num].volume -= 10;
		}
		break;
	}
}

void nextPositionValue() {
	byte flower_num = state.menu_screen - 1;
	state.flowerDataChanged = true;
	switch (state.menu_position) {
		case 3:
		// Change period
		if (flowerData[flower_num].period < 51) {
			flowerData[flower_num].period += 1;
		}
		break;
		case 4:
		// Change sensor
		if (flowerData[flower_num].sensor < 100) {
			flowerData[flower_num].sensor += 5;
		}
		break;
		case 5:
		// Change volume
		if (flowerData[flower_num].volume < 100) {
			flowerData[flower_num].volume += 10;
		} else if (flowerData[flower_num].volume < 2000) {
			flowerData[flower_num].volume += 100;
		} else if (flowerData[flower_num].volume < 65500) {
			flowerData[flower_num].volume += 500;
		}
		break;
	}
}

void pressPositionValue() {
	clearDisplay();
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
		saveFlowerData();
		resetFlowerTasks();
		char title[11] = "Saved";
		char text[21];
		displayMessage(title, text, 1000);
	}
}
