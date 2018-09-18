#include "Driver/ELock.h"




namespace ELock {
	
	void vInit(void)
	{
		RCC->AHB1ENR |= 0x08;
		
		GPIOD->MODER &= ~0xC0;
		GPIOD->MODER |= 0x40;
		GPIOD->OSPEEDR |= 0xC0;
		GPIOD->PUPDR &= ~0xC0;
		GPIOD->OTYPER &= ~0x08;
		vOff();
	}
}






