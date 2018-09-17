#ifndef		__PROTOCOL_H
#define		__PROTOCOL_H

#include <stdint.h>
#include "Custom/BaseOutput.h"

class Protocol
{
	public:
		Protocol(uint8_t *buffer) : mBuffer(buffer), nIndex(0) {}
	
		inline Protocol &setAction(uint8_t action) {
			this->mAction = action;
			return *this;
		}
	
		inline Protocol &append1(uint8_t c) {
			mBuffer[nIndex] = c;
			nIndex ++;
			return *this;
		}
	
		inline Protocol &append2(uint16_t c) {
			mBuffer[nIndex] = (c >> 7) & 0x7F;
			nIndex ++;
			mBuffer[nIndex] = c & 0x7F;
			nIndex ++;
			return *this;
		}
		
		inline Protocol &append4(uint32_t c) {
			
			mBuffer[nIndex] = (c >> 21) & 0x7F;
			nIndex ++;
			
			mBuffer[nIndex] = (c >> 14) & 0x7F;
			nIndex ++;
			
			mBuffer[nIndex] = (c >> 7) & 0x7F;
			nIndex ++;
			
			mBuffer[nIndex] = c & 0x7F;
			nIndex ++;
			
			return *this;
		}
		
		void write(BaseOutput &out, uint8_t *content);
		
		uint16_t build(uint8_t *content);
		
	private:
		uint8_t mAction;
		uint8_t *mBuffer;
		uint16_t nIndex;
};


#endif
