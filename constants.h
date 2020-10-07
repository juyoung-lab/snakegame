#ifndef _CONSTANTS_H
#define _CONSTANTS_H
#include <MCUFRIEND_kbv.h>
#include "circQueue.h"

/*
We want to be sure the user is pressing in a direction and not detect slight mistakes, hence a large deadZone
Optionally we can use a four button input but it feels clunky at times to use
*/
namespace pinInfo {
	const int16_t joyCentre = 512;
	const int16_t deadZone = 256;
}

/*
All information about our actual display size and our game screen bounds as well as the size of food for the snake
*/
namespace displayDimensions {
	const int16_t width = 480;
	const int16_t height = 320;
	const int16_t padding = 10;
	const int16_t upperCornerX = 10;
	const int16_t upperCornerY = 10;
	const int16_t lowerCornerX = 419;
	const int16_t lowerCornerY = 309;
	const int16_t foodSize = 9;
}

/*
General coordinates struct to store x, y of items such as the food and snake
*/
typedef struct coordinates {
	int16_t x, y;
	coordinates(int16_t x1 = 0, int16_t y1 = 0) : x(x1), y(y1) {}
} coordinates;

/*
Externed struct which is used to pass information between cpp files and hold global values easily
*/
struct gameInfo {
	MCUFRIEND_kbv* tft;
	uint8_t size;
	bool foodSpawned;
	bool growth;
	uint8_t direction;
	coordinates foodLocation;
	coordinates currentLocation;
	// initialize our circular queue with max size 255 (as our size only goes from 0 to 255)
	circQueue snakecq = circQueue(sizeof(coordinates), 255);
};

#endif