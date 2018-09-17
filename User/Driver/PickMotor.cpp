#include "Driver/PickMotor.h"
#include "Driver/MotorDriver.h"
#include "Driver/LogUart.h"

PickMotor::Error PickMotor::xGoUp(uint16_t sp, uint32_t steps, uint32_t timeOut)
{
	if (isTop()) {
		return Ok;
	}
	MotorDriver::instance().enable();
	
	if (steps == 0x00)
	{	
		uint32_t time = 0;
		uint32_t count = 0;
		vResetCounter();
		uint32_t counter = xGetCounter();
		vSetSpeed(sp);
		vSetDir(Up);
		while ( (!isTop()) && (time < timeOut)) 
		{
			osDelay(20);
			time += 20;
			count ++;
			if (count > 25) 
			{
				count = 0;
				if (counter == xGetCounter()) 
				{
					MotorDriver::instance().enable();
				}
				counter = xGetCounter();
			}
		}
		vSetDir(Brake);
		LogUart::instance().format("counter=%d, isTop=%d\r\n", xGetCounter(), isTop());
		
		if (time < timeOut)
			return Ok;
		else 
			return TimeOut;
	}
	
	vResetCounter();
	uint32_t time = 0;
	uint32_t count = 0;
	uint32_t counter = xGetCounter();
	vSetSpeed(sp);
	vSetDir(Up);
	while ( (!isTop()) && (time < timeOut) && (xGetCounter() < steps)) 
	{
		osDelay(20);
		time += 20;
		count ++;
		if (count > 25) 
		{
			count = 0;
			if (counter == xGetCounter()) 
			{
				MotorDriver::instance().enable();
			}
			counter = xGetCounter();
		}
	}
	vSetDir(Brake);
	LogUart::instance().format("pickmotor counter=%d\r\n", xGetCounter());
	
	if (time < timeOut)
		return Ok;
	else 
		return TimeOut;
}

PickMotor::Error PickMotor::xGoDown(uint16_t sp, uint32_t position, uint32_t timeOut)
{
	uint32_t time = 0;
	MotorDriver::instance().enable();
	uint32_t count = 0;
	vResetCounter();
	uint32_t counter = xGetCounter();
	vSetSpeed(sp);
	vSetDir(Down);
	while ( (xGetCounter() < position) && (time < timeOut) )
	{
		osDelay(20);
		time += 20;
		count ++;
		if (count > 25) 
		{
			count = 0;
			if (counter == xGetCounter()) 
			{
				MotorDriver::instance().enable();
			}
			counter = xGetCounter();
		}
	}
	vSetDir(Brake);
	LogUart::instance().format("pickmotor counter=%d\r\n", xGetCounter());
	if (time > timeOut) {
		return TimeOut;
	}
	return Ok;	
}

PickMotor::Error PickMotor::xGoDownOfAbsolute(uint16_t sp, uint32_t position, uint32_t timeOut)
{
	bool ret = xGoUp(sp, 0, timeOut);
	if (ret != Ok) {
		return TimeOut;
	}
	return xGoDown(sp, position, timeOut);
}

PickMotor::Error PickMotor::xGoDownAndCheckCompressor(uint16_t sp, uint32_t position, uint32_t timeOut)
{
	uint32_t time = 0;
	uint32_t count = 0;
	vResetCounter();
	uint32_t counter = xGetCounter();
	MotorDriver::instance().enable();
	vSetSpeed(sp);
	vSetDir(Down);
	while ( (xGetCounter() < position) && (time < timeOut) && (!isChecked()) )
	{
		osDelay(20);
		time += 20;
		count ++;
		if (count > 25) 
		{
			count = 0;
			if (counter == xGetCounter()) 
			{
				MotorDriver::instance().enable();
			}
			counter = xGetCounter();
		}
	}
	vSetDir(Brake);
	if (isChecked()) {
		return Ok;
	}
	return TimeOut;		
}

void PickMotor::vSetDir(Dir dir)
{
	switch (dir)
	{
		case Down:
			GPIOD->BSRRH = 0x4000;
			GPIOD->BSRRL = 0x8000;
			break;
				
		case Up:
			GPIOD->BSRRL = 0x4000;
			GPIOD->BSRRH = 0x8000;
			break;
				
		case Brake:
			GPIOD->BSRRH = 0xC000;
			break;
		case Unknow:
			GPIOD->BSRRH = 0xC000;
			break;
	}
}

PickMotor::PickMotor(void) : Sender("PickMoto")
{
	initMotoGpio();
	initMotoDriver();
	initCounterGpio();
	initCounterDriver();
	xGoUp();
	vSetDir(Brake);
}

/**
 * @brief 初始化取货电机的编码器计数引脚
 * TIM1_CH1
 */
void PickMotor::initCounterDriver(void)
{
	RCC->APB2ENR   |= 		0x01;
	
	TIM1->CR1 		= 		0x00;
	TIM1->CR2 		= 		0x00;
	TIM1->SMCR 		= 		0x57;
	TIM1->CCMR1 	= 		0xF1;
	TIM1->CCER 	   &= 	   ~0x0A;
	TIM1->CR1 		= 		0x01;
}

/**
 * @brief 初始化取货电机的编码器计数引脚
 * PE9 <-> TIM1_CH1 <-> AF1
 */
void PickMotor::initCounterGpio(void)
{
	RCC->AHB1ENR 	|= 		 0x10; // Enable GPIOE
	
	GPIOE->MODER	&= 		~0xC0000;
	GPIOE->MODER 	|= 		 0x80000;
	GPIOE->PUPDR 	&= 		~0xC0000;
	GPIOE->PUPDR 	|= 		 0x40000;
	GPIOE->OSPEEDR 	|= 		 0xC0000;
	GPIOE->OTYPER 	&= 	 	~0x200;
	GPIOE->AFR[1] 	&= 		~0xF0;
	GPIOE->AFR[1] 	|= 		 0x10;
}

/**
 * @brief 初始化取货电机的控制引脚
 * PB9 <-> TIM11_CH1 <-> AF3
 * PD14 <-> OUT
 * PD15 <-> OUT
 */
void PickMotor::initMotoGpio(void)
{
	RCC->AHB1ENR |= 0x0A;
	
	GPIOB->MODER &= ~0xC0000;
	GPIOB->MODER |= 0x80000;
	GPIOB->PUPDR &= ~0xC0000;
	GPIOB->OSPEEDR |= 0xC0000;
	GPIOB->OTYPER &= ~0x200;
	GPIOB->AFR[1] &= ~0xF0;
	GPIOB->AFR[1] |= 0x30;
	
	GPIOD->MODER &= ~0xF0000000;
	GPIOD->MODER |= 0x50000000;
	GPIOD->PUPDR &= ~0xF0000000;
	GPIOD->OSPEEDR |= 0xF0000000;
	GPIOD->OTYPER &= ~0xC000;
}

/**
 * @brief 初始化PWM输出引脚
 * PWM频率=500
 */
void PickMotor::initMotoDriver(void)
{
	RCC->APB2ENR |= 0x40000; // Enable TIM11
	
	TIM11->CR1 = 0x00;
	TIM11->CR2 = 0x00;
	TIM11->PSC = 168 - 1;
	TIM11->ARR = 100 - 1;
	TIM11->CR1 = 100;
	TIM11->CCMR1 = 0x0060;
	TIM11->CCER = 0x0001;
	TIM11->CR1 = 0x01;
	TIM11->EGR = 0x01;
}










