#include "Driver/SysTimer.h"



namespace SysTimer { // TIM10
	
	void vInit(void)
	{
		RCC->APB2ENR |= 0x20000; // 17
		
		TIM10->CR1 = 0x00;
		TIM10->CR2 = 0x00;
		TIM10->PSC = 168 - 1;
		TIM10->ARR = 0xFFFF;
		TIM10->CR1 = 0x01;
		TIM10->EGR = 0x01;
	}
	
	void vDelayUs(uint16_t us)
	{
		TIM10->CNT = 0x00;
		while (TIM10->CNT <= us);
	}
		
}








