#include "Driver/Ds18b20.h"
#include "Driver/SysTimer.h"
#include <rtx_os.h>

#define SetDQ()		GPIOE->BSRRL = 0x02
#define ClrDQ()		GPIOE->BSRRH = 0x02
#define ReadDQ()	(GPIOE->IDR & 0x02)

/**
 * @brief PE1 <-> DQ
 * 开漏输出模式
 */
Ds18b20::Ds18b20(void)
{
	RCC->AHB1ENR |= 0x10; // GPIOE
	
	GPIOE->MODER &= ~0x0C;
	GPIOE->MODER |= 0x04;
	GPIOE->OTYPER |= 0x02;
	GPIOE->OSPEEDR |= 0x0C;
	GPIOE->PUPDR &= ~0x0C;
	GPIOE->PUPDR |= 0x04;
	SetDQ();
}

void Ds18b20::vReset(void)
{
	ClrDQ();
	SysTimer::vDelayUs(750);
	SetDQ();
	SysTimer::vDelayUs(15);
}

bool Ds18b20::xCheckDevice(void)
{
	uint8_t count = 0;
	while ( ReadDQ() && (count < 200) )
	{
		count ++;
		SysTimer::vDelayUs(1);
	}
	
	if (count >= 200) {
		return false;
	}
	
	count = 0;
	
	while ( (!ReadDQ()) && (count < 240) )
	{
		count ++;
		SysTimer::vDelayUs(1);
	}
	
	if (count >= 240) {
		return false;
	}
	
	return true;
	
}

bool Ds18b20::xReadBit(void)
{
	bool v;
	ClrDQ();
	SysTimer::vDelayUs(2);
	SetDQ();
	SysTimer::vDelayUs(12);
	if (ReadDQ()) {
		v = true;
	} else {
		v = false;
	}
	SysTimer::vDelayUs(50);
	
	return v;
}

uint8_t Ds18b20::xReadByte(void)
{
	uint8_t byte = 0;
	for (uint32_t i = 0; i < 8; i ++)
	{
		byte >>= 1;
		if (xReadBit()) {
			byte |= 0x80;
		}
	}
	return byte;
}

void Ds18b20::vWriteByte(uint8_t data)
{
	for (uint32_t i = 0; i < 8; i ++)
	{
		if (data & 0x01) 
		{
			ClrDQ();
			SysTimer::vDelayUs(2);
			SetDQ();
			SysTimer::vDelayUs(90);
		}
		else 
		{
			ClrDQ();
			SysTimer::vDelayUs(90);
			SetDQ();
			SysTimer::vDelayUs(2);
		}
		
		data >>= 1;
	}
}

void Ds18b20::vStart(void)
{
	vReset();
	xCheckDevice();
	vWriteByte(0xCC);
	vWriteByte(0x44);
}

uint32_t Ds18b20::xGetValue(void)
{
	bool f;
	uint8_t tl, th;
	
	vStart();
	vReset();
	f = xCheckDevice();
	if (!f) {
		return 0x10000;
	}
	
	vWriteByte(0xCC);
	vWriteByte(0xBE);
	
	tl = xReadByte();
	th = xReadByte();
	
	return (th << 8) + tl;
}



















