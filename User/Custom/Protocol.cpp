#include "Custom/Protocol.h"

void Protocol::write(BaseOutput &out, uint8_t *content)
{
	content[0] = 0xE1;
	content[1] = nIndex + 5;
	content[2] = mAction;
	uint8_t c = 0;
	for (uint16_t i = 0; i < nIndex; i ++) {
		content[3 + i] = mBuffer[i];
		c ^= mBuffer[i];
	}
	content[3 + nIndex] = c;
	content[4 + nIndex] = 0xEF;
	out.write(content, nIndex + 5);
}

uint16_t Protocol::build(uint8_t *content)
{
	content[0] = 0xE1;
	content[1] = nIndex + 5;
	content[2] = mAction;
	uint8_t c = 0;
	for (uint16_t i = 0; i < nIndex; i ++) {
		content[3 + i] = mBuffer[i];
		c ^= mBuffer[i];
	}
	content[3 + nIndex] = c;
	content[4 + nIndex] = 0xEF;
	return nIndex + 5;
}
