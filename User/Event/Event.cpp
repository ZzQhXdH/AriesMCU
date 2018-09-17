#include "Event/Event.h"
#include "Driver/MainUart.h"
#include "Custom/Protocol.h"

uint8_t Event::check(void) const
{
	uint32_t end = nMsgCount - 2;
	uint8_t sum = 0;
	for (uint32_t i = 3; i < end; i ++) {
		sum ^= pMsg[i];
	}
	return sum;
}

bool Event::isCorrect(void) const
{
	if (0xE1 != pMsg[0] || 0xEF != pMsg[nMsgCount - 1]) {
		return false;
	}
	
	if (pMsg[1] != nMsgCount) {
		return false;
	}
	
	if (check() != pMsg[nMsgCount - 2]) {
		return false;
	}
	
	return true;
}

uint8_t Event::isCorrectOfResult(void) const 
{
	if (0xE1 != pMsg[0] || 0xEF != pMsg[nMsgCount - 1]) {
		return 1;
	}
	
	if (pMsg[1] != nMsgCount) {
		
		return 2;
	}
	
	if (check() != pMsg[nMsgCount - 2]) {
		
		return 3;
	}
	
	return 0;	
}

void Event::onReturn(uint8_t status) 
{
	
}

void Event::onStatusChange(uint8_t row, uint8_t col, uint8_t status)
{
	
}

void Event::onResult(const uint8_t *buffer, uint16_t length)
{
	
}




