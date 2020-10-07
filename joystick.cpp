#include "constants.h"
#include "joystick.h"
#define joyXPin A8
#define joyYPin A9
/*
Finds the PRIMARY direction of the joystick (we do not have angled movement in snake).
returns:
	0: no primary direction
	1: right (positive x)
	2: up (positive y)
	3: left (negative x)
	4: down (negative y)
note: we check against current direction in game.cpp to ensure we can only quarter turn and not turn back into ourselves
*/
int readJoystick() {
	int x = analogRead(joyXPin);
	int y = analogRead(joyYPin);
	int magX = abs(x - pinInfo::joyCentre);
	int magY = abs(y - pinInfo::joyCentre);
	// compare magnitude of displacement from centre of joystick to determine dominant direction
	if (magX > magY) {
		if (x > (pinInfo::joyCentre + pinInfo::deadZone)) {
			return 3;
		} else if (x < (pinInfo::joyCentre - pinInfo::deadZone)) {
			return 1;
		} else {
			return 0;
		}
	} else {
		if (y > (pinInfo::joyCentre + pinInfo::deadZone)) {
			return 4;
		} else if (y < (pinInfo::joyCentre - pinInfo::deadZone)) {
			return 2;
		} else {
			return 0;
		}
	}
}