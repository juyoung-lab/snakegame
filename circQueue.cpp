#include "circQueue.h"
#include "Arduino.h"

/*
This is the key to a circular queue. When incrementing our index, if we reach our upper bound, we need to instead 
circle back around to the start so that we can write continuously to our dedicated amount of memory in a dynamic fashion.
*pointerIndex: pointer to our index which we adjust
upper: the upper bound, here we use the max number of items in our queue as described when constructing
lower: lower bound, simply zero
*/
static void incrementIndex(uint16_t* pointerIndex, uint16_t upper, uint16_t lower) {
	if (*pointerIndex < (upper - 1)) {
		(*pointerIndex)++;
	} else {
		*pointerIndex = lower;
	}
}

/*
We initialize our queue with the size of each item (we do this by using sizeof() when calling), and the number of items (max)
*/
circQueue::circQueue(uint16_t is, uint16_t ni) {

	itemSize = is;
	numberItems = ni;
	uint32_t qSize = itemSize * numberItems;
	
	cqueue = (uint8_t*) malloc(qSize);
	clear();
}

/*
Sets the number of items pushed/pulled to zero, we use this when restarting the game.
*/
void circQueue::clear() {
	nPush = 0;
	nPull = 0;
}

/*
Method to push an item into our queue. 
*/
void circQueue::push(void* item) {
	// calculates the pointer to start at as the pointer to our queue plus the number of items pushed multiplied by item size
	uint8_t* pointerStart = cqueue + (nPush * itemSize);
	// increment after pushing to keep track
	incrementIndex(&nPush, numberItems, 0);
	// copy our input item to the memory location we are currently at 
	memcpy(pointerStart, item, itemSize);
}

/*
Method to pull an item from our queue.
*/
void circQueue::pop(void* item) {
	// similar to push method, calculate the start but this time from the number of items pulled thus far
	uint8_t* pointerStart = cqueue + (nPull * itemSize);
	// increment after pulling
	incrementIndex(&nPull, numberItems, 0);
	// copy item pulled so we can return it
	memcpy(item, pointerStart, itemSize);
	
}

/*
Method to peek at an item given an index
*/
void circQueue::peekIndex(void* item, uint16_t index) {
	// finds the position by having initial cqueue and adding item size multiplied by modulus of index plus items pulled by number of items
	// this is setup in this fashion as we have a circular queue which does loop around but is limited by number of items initially declared
	uint8_t* pointerStart = cqueue + (itemSize * ((nPull + index) % numberItems));
	// copy so we can return from function
	memcpy(item, pointerStart, itemSize);
}