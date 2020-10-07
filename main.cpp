#include "Adafruit_GFX.h"
#include "Arduino.h"
#include "MCUFRIEND_kbv.h"
#include "circQueue.h"
#include "constants.h"
#include "game.h"

#define YELLOW 0xFF50
#define BLUE 0x847F
#define PINK 0xFB2E
#define WHITE 0xFFFF
#define BLACK 0x0000

MCUFRIEND_kbv tft;
// struct which contains information about our snake and tft display
gameInfo snakeInfo;

void setup() {
	init();
	snakeInfo.tft = &tft;
	uint16_t ID = tft.readID();
	tft.begin(ID);
	tft.setRotation(1);
	
	Serial.begin(9600);
	Serial.flush();
	randomSeed(analogRead(47));
}

void initializeDisplay() {
	tft.fillScreen(WHITE);
	
	tft.drawRect(displayDimensions::padding - 1, 
		displayDimensions::padding - 1,
		(displayDimensions::width - 2*displayDimensions::padding + 2), 
		(displayDimensions::height - 2*displayDimensions::padding + 2),
		BLACK);
		
	tft.fillRect(displayDimensions::padding, 
		displayDimensions::padding,
		(displayDimensions::width - 2*displayDimensions::padding), 
		(displayDimensions::height - 2*displayDimensions::padding),
		YELLOW);
	
	tft.drawLine(displayDimensions::width - 6*displayDimensions::padding,
		displayDimensions::padding,
		displayDimensions::width - 6*displayDimensions::padding,
		displayDimensions::height - displayDimensions::padding,
		BLACK);
}

/*
We go through the trouble of defining our struct values here as we reset to these values on a "game over" and 
we opt to not have destructors or reset methods within our struct
*/
void initializeGame() {
	snakeInfo.currentLocation.x = random(displayDimensions::padding*5, displayDimensions::width - displayDimensions::padding*10);
	snakeInfo.currentLocation.y = random(displayDimensions::padding*5, displayDimensions::height - displayDimensions::padding*5);
	snakeInfo.snakecq.clear();
	snakeInfo.foodSpawned = false;
	snakeInfo.growth = false;
	snakeInfo.size = 5;
	snakeInfo.direction = 1;
	int16_t trueX = snakeInfo.currentLocation.x;
	/*
	For a defined length (which does not extend past screen bounds), generate our initial snake entity which is stored
	in a circular queue. Later restore back to the coordinate of our head. Snake always starts heading rightwards.
	*/
	for (int i = snakeInfo.size - 1; i > -1; i--) {
		snakeInfo.currentLocation.x = snakeInfo.currentLocation.x - i;
		snakeInfo.snakecq.push(&snakeInfo.currentLocation);
		snakeInfo.currentLocation.x = trueX;
	}
	snakeInfo.tft->setTextSize(2);
	snakeInfo.tft->setCursor(423, 16);
	snakeInfo.tft->setTextColor(BLUE, YELLOW);
	snakeInfo.tft->print(snakeInfo.size);
		
}

int main() {
	for(;;) {
		setup();
		initializeDisplay();
		initializeGame();
		// the game itself is largely contained within game.cpp, we use main.cpp to setup our game
		playGame();
	}
}