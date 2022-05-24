// Buttons

#include <EncButton.h>
EncButton<EB_TICK, ENC_A_PIN, ENC_B_PIN, ENC_SW_PIN> enc(INPUT_PULLUP);

void encSetup() {
	enc.setEncType(EB_FULLSTEP);
	// enc.setEncType(EB_HALFSTEP);
}

void encoderInterrupt() {
	disableInterrupts();
	power.wakeUp();
	if (state.sleep_mode) {
		awake();
		enc.resetState(); // Reset any encoder state after awake
		wasActive();
	}
	enableInterrupts();
}

void encoderCheck() {
	// Check encoder
	if (enc.tick()) {
		if (enc.click() && (millis() - state.last_active_time) > 200) {
			// Position select
			wasActive();
			// Serial.println("OK");
			okButtonPressed();
		}
		if (enc.left()) {
			// Position change
			wasActive();
			// Serial.println("Left");
			leftTurned();
		}
		if (enc.right()) {
			// Position change
			wasActive();
			// Serial.println("Right");
			rightTurned();
		}
    enc.resetState();
    displayData();
	}
}

void okButtonPressed() {
	switch (state.menu_function) {
		case 0:
			// Menu
			pressMenu();
			break;
		case 1:
			// Position select
			pressPosition();
			break;
		case 2:
			// Position change
			pressPositionValue();
			break;
	}
}

void leftTurned() {
	switch (state.menu_function) {
		case 0:
			// Menu
			nextMenu();
			break;
		case 1:
			// Position select
			prevPosition();
			break;
		case 2:
			// Position change
			prevPositionValue();
			break;
	}
}

void rightTurned() {
	switch (state.menu_function) {
		case 0:
			// Menu
			prevMenu();
			break;
		case 1:
			// Position select
			nextPosition();
			break;
		case 2:
			// Position change
			nextPositionValue();
			break;
	}
}
