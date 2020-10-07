#ifndef _CIRCQUEUE_H
#define _CIRCQUEUE_H

#include <Arduino.h>

class circQueue {
	private:
		uint16_t itemSize; // item size
		uint16_t numberItems; // number of items
		uint8_t* cqueue; // pointer to the start of our queue
		uint16_t nPush; // number of items pushed
		uint16_t nPull; // number of items popped
	public:
		circQueue(uint16_t itemSize, uint16_t numberItems);
		void clear();
		void push(void* item);
		void pop(void* item);
		void peekIndex(void* item, uint16_t index);
};
#endif