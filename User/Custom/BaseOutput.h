#ifndef		__BASE_OUTPUT_H
#define		__BASE_OUTPUT_H

#include <stdint.h>

class BaseOutput
{
	public:
		BaseOutput(uint8_t *buffer) : mBufferMemory(buffer) {}
	
	public:
	
		virtual void write(const uint8_t *byteArray, uint16_t length) = 0;
	
		void format(const char *format, ...);
		
		uint16_t build(uint8_t *content);
		
		inline void buildAndWrite(void) 
		{
			uint16_t len = build(mBufferMemory + mIndex);
			write(mBufferMemory + mIndex, len);
		}
	
		inline BaseOutput &setAction(uint8_t action) 
		{
			mAction = action;
			mIndex = 0;
			return *this;
		}
		
		inline BaseOutput &append1(uint8_t byte) 
		{
			mBufferMemory[mIndex] = byte & 0x7F;
			mIndex ++;
			return *this;
		}
		
		inline BaseOutput &append2(uint16_t d) 
		{
			mBufferMemory[mIndex] = (d >> 7) & 0x7F;
			mIndex ++;
			mBufferMemory[mIndex] = d & 0x7F;
			mIndex ++;
			return *this;
		}
		
		inline BaseOutput &append4(uint32_t d)
		{
			mBufferMemory[mIndex] = (d >> 21) & 0x7F;
			mIndex ++;
			mBufferMemory[mIndex] = (d >> 14) & 0x7F;
			mIndex ++;
			mBufferMemory[mIndex] = (d >> 7) & 0x7F;
			mIndex ++;
			mBufferMemory[mIndex] = d & 0x7F;
			mIndex ++;
			return *this;
		}
	
	private:
		uint8_t *mBufferMemory;
		uint8_t mAction;
		uint8_t mIndex;
};






#endif
