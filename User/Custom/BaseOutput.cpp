#include "Custom/BaseOutput.h"
#include <stdio.h>
#include <stdarg.h>


void BaseOutput::format(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	int len = vsprintf((char *) mBufferMemory, format, ap);
	write((const uint8_t *) mBufferMemory, len);
}

uint16_t BaseOutput::build(uint8_t *content)
{
	content[0] = 0xE1;
	content[1] = mIndex + 5;
	content[2] = mAction;
	uint8_t c = 0;
	for (uint16_t i = 0; i < mIndex; i ++) {
		content[3 + i] = mBufferMemory[i];
		c ^= mBufferMemory[i];
	}
	content[3 + mIndex] = c;
	content[4 + mIndex] = 0xEF;
	return mIndex + 5;
}










