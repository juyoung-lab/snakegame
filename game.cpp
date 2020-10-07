#include "game.h"
#include "joystick.h"
#include "constants.h"
#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"
#include "main.h"
#include "Arduino.h"

#define YELLOW 0xFF50
#define BLUE 0x847F
#define PINK 0xFB2E
#define WHITE 0xFFFF
#define BLACK 0x0000

extern gameInfo snakeInfo;

void spawnFood() {
	if (!snakeInfo.foodSpawned) {
		coordinates food;
		// let us generate a random spawn point away from game borders
		food.x = random(displayDimensions::upperCornerX + displayDimensions::foodSize, 
		displayDimensions::lowerCornerX - displayDimensions::foodSize);
		food.y = random(displayDimensions::upperCornerY + displayDimensions::foodSize, 
		displayDimensions::lowerCornerY - displayDimensions::foodSize);
		coordinates indivPoint;
		
		// the odds of needing to loop twice are slim
		for (int i = 0; i < snakeInfo.size; i++) {
			// peek our circular queue at index i to get the coordinates of that snake point
			snakeInfo.snakecq.peekIndex(&indivPoint, i);

			if ((indivPoint.x >= (food.x - displayDimensions::foodSize/2)) &&
				(indivPoint.x <= (food.x + displayDimensions::foodSize/2)) &&
				(indivPoint.y >= (food.y - displayDimensions::foodSize/2)) &&
				(indivPoint.y <= (food.y + displayDimensions::foodSize/2))) {

				// if we find our snake within the new food spawn, let us make a new spawn and iterate through our snake again (i = 0)
				food.x = random(displayDimensions::upperCornerX + displayDimensions::foodSize, 
				displayDimensions::lowerCornerX - displayDimensions::foodSize);
				food.y = random(displayDimensions::upperCornerY + displayDimensions::foodSize, 
				displayDimensions::lowerCornerY - displayDimensions::foodSize);
				i = 0;
			}
		}
		
		// update the location of our food and draw it in
		snakeInfo.foodLocation.x = food.x;
		snakeInfo.foodLocation.y = food.y;
		snakeInfo.tft->fillRect(food.x - displayDimensions::foodSize/2, food.y - displayDimensions::foodSize/2, 
		displayDimensions::foodSize, displayDimensions::foodSize, BLUE);
		
		snakeInfo.foodSpawned = true;
	}
}

// can only do quarter turns to avoid turning back into ourselves and thereby ending the game
void updateDirection(int js) {
	switch(js) {
		// one-liner cases for clarity
		case 1: if (snakeInfo.direction == 2 || snakeInfo.direction == 4) {snakeInfo.direction = 1;} break;
		case 2: if (snakeInfo.direction == 1 || snakeInfo.direction == 3) {snakeInfo.direction = 2;} break;
		case 3: if (snakeInfo.direction == 2 || snakeInfo.direction == 4) {snakeInfo.direction = 3;} break;
		case 4: if (snakeInfo.direction == 1 || snakeInfo.direction == 3) {snakeInfo.direction = 4;} break;
	}
}

void gameOver() {
	snakeInfo.tft->fillScreen(BLACK);
	snakeInfo.tft->setCursor(0, 0);
	snakeInfo.tft->setTextColor(WHITE);
	snakeInfo.tft->setTextSize(3);

	snakeInfo.tft->println("GAME OVER");

	char endGame[19];
	endGame[18] = '\0';
	sprintf(endGame, "Final length: %u", snakeInfo.size);
	snakeInfo.tft->println(endGame);
	snakeInfo.tft->println("Restarting...");
	delay(5000);
	main();
}

void collisionCheck() {
	// check for collision with food within the size of our food
	if (snakeInfo.foodSpawned) {
		
		if ((snakeInfo.currentLocation.x >= (snakeInfo.foodLocation.x - displayDimensions::foodSize/2)) &&
			(snakeInfo.currentLocation.x <= (snakeInfo.foodLocation.x + displayDimensions::foodSize/2)) &&
			(snakeInfo.currentLocation.y >= (snakeInfo.foodLocation.y - displayDimensions::foodSize/2)) &&
			(snakeInfo.currentLocation.y <= (snakeInfo.foodLocation.y + displayDimensions::foodSize/2))) {
			
			// here we indicate that the food is no longer spawned (it is eaten) and that our snake will grow when we update position
			snakeInfo.growth = true;
			snakeInfo.foodSpawned = false;
			
			// erase the food spawn
			snakeInfo.tft->fillRect(snakeInfo.foodLocation.x - displayDimensions::foodSize/2, 
				snakeInfo.foodLocation.y - displayDimensions::foodSize/2, 
				displayDimensions::foodSize, displayDimensions::foodSize, YELLOW);			
		}
	}
	// iterate through our snake queue to check for collision
	// this will get slow once our snake becomes very large but the game ends before that point
	coordinates indivPoint;
	for (int i = 0; i < snakeInfo.size; i++) {
		snakeInfo.snakecq.peekIndex(&indivPoint, i);
		if (snakeInfo.currentLocation.x == indivPoint.x &&
			snakeInfo.currentLocation.y == indivPoint.y) {
				gameOver();
		}
	}
	// check for collision with game border
	if (snakeInfo.currentLocation.x < displayDimensions::upperCornerX || 
		snakeInfo.currentLocation.x > displayDimensions::lowerCornerX ||
		snakeInfo.currentLocation.y < displayDimensions::upperCornerY ||
		snakeInfo.currentLocation.y > displayDimensions::lowerCornerY) {
		gameOver();
	}
}

/*
The order in which we update vaues is actually important. First we must remove the tail (in the event of no growth) so that 
a non-growing 5+ length snake would not collide with itself when chasing its tail. After this we check for collisions and then update the head
*/
void updateValues() {
	coordinates localCoords;
	if (!snakeInfo.growth) {
		snakeInfo.snakecq.pop(&localCoords);
		Serial.println(localCoords.x);
		Serial.println(localCoords.y);
		snakeInfo.tft->drawPixel(localCoords.x, localCoords.y, YELLOW);
	} else {
		snakeInfo.size++;
		snakeInfo.growth = false;
		
		// magic numbers: these are within the sidebar where we print our current length
		snakeInfo.tft->setTextSize(2);
		snakeInfo.tft->setCursor(423, 16);
		snakeInfo.tft->setTextColor(BLUE, YELLOW);
		snakeInfo.tft->print(snakeInfo.size);
	}
	// update our head in the direction of movement
	switch(snakeInfo.direction) {
		case 1: snakeInfo.currentLocation.x++; break;
		case 2: snakeInfo.currentLocation.y--; break;
		case 3: snakeInfo.currentLocation.x--; break;
		case 4: snakeInfo.currentLocation.y++; break;
	}
	// if there is a collision with wall or self, gameOver() is called, else we continue with our program
	collisionCheck();

	// updates the head, draws the pixel is
	snakeInfo.snakecq.push(&snakeInfo.currentLocation);
	snakeInfo.tft->drawPixel(snakeInfo.currentLocation.x, snakeInfo.currentLocation.y, PINK);
}

void playGame() {
	int js;
	for(;;) {
		spawnFood();
		js = readJoystick();
		updateDirection(js);
		updateValues();
		// let us end the game at 255 as that is the max size of your uint8_t variable for snake size, this is convenient
		if (snakeInfo.size == 255) {
			gameOver();
		}
		// this delay feels natural and slows the snake down enough for proper control at its size:foodsize ratio
		delay(20);
	}
}

