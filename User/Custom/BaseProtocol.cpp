#include "Custom/BaseProtocol.h"


BaseProtocol::BaseProtocol(uint8_t *buffer, const Event &env, uint8_t status) : mBuffer(buffer), mLength(0)
{
	mLength = env.size() + 1;
	uint16_t count = env.argCount();
	mBuffer[0] = 0xE1;
	mBuffer[2] = env.action() | 0x80;
	mBuffer[1] = mLength;
	uint8_t c = 0;
	for (uint16_t i = 0; i < count; i ++)
	{
		mBuffer[3 + i] = env.arg(i + 1); 
		c ^= env.arg(i + 1);
	}
	mBuffer[3 + count] = status;
	c ^= status;
	mBuffer[mLength - 2] = c;
	mBuffer[mLength - 1] = 0xEF;	
}

BaseProtocol::BaseProtocol(uint8_t *buffer, uint8_t action, const uint8_t *args, uint16_t size) : mBuffer(buffer), mLength(0)
{
	mLength = size + 5;
	mBuffer[0] = 0xE1;
	mBuffer[2] = action | 0x80;
	mBuffer[1] = mLength;
	uint8_t c = 0;
	for (uint16_t i = 0; i < size; i ++)
	{
		c ^= args[i];
		mBuffer[3 + i] = args[i];
	}
	mBuffer[size + 3] = c;
	mBuffer[size + 4] = 0xEF;	
}



