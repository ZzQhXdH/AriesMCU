#include "Driver/Compressor.h"

/**
 * @brief 空气压缩机 PD4(控制)
 */
Compressor::Compressor(void)
{
	RCC->AHB1ENR |= 0x08;
	
	GPIOD->MODER &= ~0x300;
	GPIOD->MODER |= 0x100;
	GPIOD->PUPDR &= ~0x300;
	GPIOD->OSPEEDR |= 0x300;
	GPIOD->OTYPER &= ~0x10;
	GPIOD->BSRRL = 0x10;
	
	off();
}

Compressor::Error Compressor::xWaitDropOut(const uint16_t timeOut)
{
	uint16_t counter = 0;
	
	while (true)
	{
		if (!isChecked()) {
			return Ok;
		}
		counter += 20;
		osDelay(20);
		
		if (counter >= timeOut) {
			return TimeOut;
		}
	}
}



