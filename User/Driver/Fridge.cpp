#include "Driver/Fridge.h"
#include "Driver/MotorDriver.h"
#include "Driver/LogUart.h"


Fridge::Error Fridge::xOpen(uint16_t sp, uint32_t timeOut)
{
	if (isOpen()) {
		return Ok;
	}
	MotorDriver::instance().enable();
	
	vResetCounter();
	uint32_t time = 0;
	uint32_t count = 0;
	uint32_t counter = xGetCounter();
	uint32_t lastCounter = 0;
	
	vSetSpeed(sp);
	vSetDir(Open);
	
	while ( (!isOpen()) && (time < timeOut) ) 
	{
		
		
		osDelay(20);
		
		time += 20;
		
		count ++;
		
		counter = xGetCounter();
		
		if (count > 25) 
		{
			count = 0;
			if (counter == lastCounter) 
			{
				MotorDriver::instance().enable();
			}
			lastCounter = counter;
		}
	}
	vSetDir(Brake);
	LogUart::instance().format("open counter=%d\r\n", xGetCounter());
	if (isOpen()) {
		return Ok;
	}
	return TimeOut;
}

Fridge::Error Fridge::xClose(uint16_t sp, uint32_t timeOut)
{
	if (isClose()) {
		return Ok;
	}
	MotorDriver::instance().enable();
	vResetCounter();
	
	uint32_t count = 0;
	uint32_t lastCounter = 0;
	uint32_t counter = xGetCounter();
	uint32_t time = 0;
	vSetSpeed(sp);
	vSetDir(Close);
	while ( (!isClose()) && (time < timeOut) ) 
	{
		osDelay(20);
		
		time += 20;
		
		count ++;
		
		counter = xGetCounter();
		
		if (count > 10) 
		{
			count = 0;
			if (counter == lastCounter) 
			{
				MotorDriver::instance().enable();
			}
			lastCounter = counter;
		}
	}
	vSetDir(Brake);
	LogUart::instance().format("close counter=%d\r\n", xGetCounter());

	if (isClose()) {
		return Ok;
	}
	return TimeOut;
}

void Fridge::vSetDir(Dir dir) 
{
	switch (dir)
	{
		case Close:
			GPIOE->BSRRH = 0x40;
			GPIOE->BSRRL = 0x80;
			break;
		case Open:
			GPIOE->BSRRL = 0x40;
			GPIOE->BSRRH = 0x80;
			break;
		case Brake:
			GPIOE->BSRRH = 0xC0;
			break;
		case Unknow:
			GPIOE->BSRRH = 0xC0;
			break;
	}
}


Fridge::Fridge(void) : Sender("Fridge")
{
	initMotoGpio();
	initMotoDriver();
	initCounterGpio();
	initCounterDriver();
	vSetDir(Brake);
}

/**
 * @brief 冰箱门电机控制引脚
 * PA3 <-> TIM9_CH2 <-> AF3
 * PE6 <-> OUT
 * PE7 <-> OUT
 */
void Fridge::initMotoGpio(void)
{
	RCC->AHB1ENR |= 0x01 | 0x10;
	
	GPIOA->MODER &= ~0xC0;
	GPIOA->MODER |= 0x80;
	GPIOA->PUPDR &= ~0xC0;
	GPIOA->OSPEEDR |= 0xC0;
	GPIOA->OTYPER &= ~0x08;
	GPIOA->AFR[0] &= ~0xF000;
	GPIOA->AFR[0] |= 0x3000;
	
	GPIOE->MODER &= ~0xF000;
	GPIOE->MODER |= 0x5000;
	GPIOE->PUPDR &= ~0xF000;
	GPIOE->OSPEEDR |= 0xF000;
	GPIOE->OTYPER &= ~0xC0;
}

/**
 * @brief 冰箱门电机速度控制初始化
 * TIM9_CH2 <-> PWM = 500k
 */
void Fridge::initMotoDriver(void)
{
	RCC->APB2ENR |= 0x10000; // Enable TIM9
	
	TIM9->CR1 = 0x00;
	TIM9->CR2 = 0x00;
	TIM9->PSC = 168  - 1;
	TIM9->ARR = 100 - 1;
	TIM9->CCR2 = 100;
	TIM9->CCMR1 = 0x6000;
	TIM9->CCER = 0x0010;
	TIM9->CR1 = 0x01;
	TIM9->EGR = 0x01;
}

/**
 * @brief 冰箱门电机计数器初始化
 * PD12 <-> TIM4_CH1 <-> AF2
 */
void Fridge::initCounterGpio(void)
{
	RCC->AHB1ENR |= 0x08;
	
	GPIOD->MODER &= ~0x3000000;
	GPIOD->MODER |= 0x2000000;
	GPIOD->PUPDR &= ~0x3000000;
	GPIOD->PUPDR |= 0x1000000;
	GPIOD->OSPEEDR |= 0x3000000;
	GPIOD->OTYPER &= ~0x1000;
	GPIOD->AFR[1] &= ~0xF0000;
	GPIOD->AFR[1] |= 0x20000;
}

/**
 * @brief 冰箱门电机计数器设置
 * TIM4_CH1 
 */
void Fridge::initCounterDriver(void)
{
	RCC->APB1ENR |= 0x04;
	
	TIM4->CR1 = 0x00;
	TIM4->CR2 = 0x00;
	TIM4->SMCR = 0x57;
	TIM4->CCMR1 = 0xF1;
	TIM4->CCER &= ~0x0A;
	TIM4->CR1 = 0x01;
}






