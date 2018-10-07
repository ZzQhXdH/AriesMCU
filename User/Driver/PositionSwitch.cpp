#include "Driver/PositionSwitch.h"

#define OLD_BOARD		

#define PL_LOW()		GPIOE->BSRRH = 0x8000
#define PL_HIGH()		GPIOE->BSRRL = 0x8000

#define SD_READ()		(GPIOE->IDR & 0x4000)

#define SCK_LOW()		GPIOE->BSRRH = 0x2000
#define SCK_HIGH()		GPIOE->BSRRL = 0x2000


/**
 * @brief
 * PL <-> PE15
 * Q7 <-> PE14
 * CP <-> PE13
 */
PositionSwitch::PositionSwitch(void) : Sender("PositionSwitch")
{
	RCC->AHB1ENR |= 0x10;
	
	GPIOE->MODER &= ~0xFC000000;
	GPIOE->MODER |= 0x44000000;
	GPIOE->PUPDR &= ~0xFC000000;
	GPIOE->PUPDR |= 0x54000000;
	GPIOE->OTYPER &= ~0xA000;
	GPIOE->OSPEEDR |= 0xFC000000;
	xGetValue();
}


uint8_t PositionSwitch::xGetValue(void)
{
	uint8_t byte = 0;
	
	acquire();
	
	PL_LOW();
	PL_HIGH();
	if (SD_READ()) 
	{
		byte ++;
	}
	
	for (uint32_t i = 0; i < 7; i ++) 
	{
		SCK_HIGH();
		byte <<= 1;
		if (SD_READ()) 
		{
			byte ++;
		}
		SCK_LOW();
	}
	
	release();
	
	return byte << 1 ;
}









