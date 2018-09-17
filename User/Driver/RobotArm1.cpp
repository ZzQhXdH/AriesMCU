#include "Driver/RobotArm1.h"
#include "Driver/MotorDriver.h"

RobotArm1::Error RobotArm1::xRotationTo(uint16_t position, uint32_t timeOut, uint16_t range)
{
	int32_t v;
	uint32_t count = 0;
	int32_t a;
	
	a = xGetAbsolutePosition() - position;
	if ( (a >= -range) && (a <= range) ) {
			vSetDir(Brake);
			return Ok;
	}
	
	pid.init(position, 3, 0.35, 5);
	MotorDriver::instance().enable();
	
	while (true)
	{
		if (xGetAbsolutePosition() == position) {
			osDelay(100);
			count += 100;
			a = xGetAbsolutePosition() - position;
			if ( (a >= -range) && (a <= range) ) {
				vSetDir(Brake);
				return Ok;
			}
		}
		v = pid.nextValue( xGetAbsolutePosition() );
		if (v > 0) {
			vSetDir(CCW);
			vSetSpeed(v);
		} else if (v < 0) {
			vSetDir(CW);
			vSetSpeed(-v);
		}
		osDelay(20);
		count += 20;
		if (count >= timeOut) {
			vSetDir(Brake);
			return TimeOut;
		}
	}
}

void RobotArm1::vInversedBrake(void)
{
	int32_t value = xGetAbsolutePosition();
	if (value > 518) {
		vSetSpeed(2);
		vSetDir(CCW);
		return;
	}
	if (value < 518) {
		vSetSpeed(2);
		vSetDir(CW);
	}
}
	

uint16_t RobotArm1::xGetAbsolutePosition(void)
{
	int32_t v = xGetRawPosition() + nCheckData;
	if (v < 0) {
		return v + 1036;
	} else if (v > 1036) {
		return v - 1036;
	}
	return v;
}

RobotArm1::RobotArm1(void) : Sender("RobotArm1")
{
	initMotoGpio();
	initMotoDriver();
	initCounterGpio();
	initCounterDriver();
	vSetDir(Brake);
	nCheckData = FlashMemory::instance().get( ROBOT_ARM1_CHECK_DATA_INDEX );
}

void RobotArm1::vSetDir(Dir dir) 
{
	switch (dir)
	{
		case CCW:
			GPIOE->BSRRL = 0x04;
			GPIOE->BSRRH = 0x08;
			break;
				
		case CW:
			GPIOE->BSRRH = 0x04;
			GPIOE->BSRRL = 0x08;
			break;
				
		case Brake:
			GPIOE->BSRRH = 0x0C;
		//	vSetSpeed(120);
			break;
		}
}

/**
 * @brief 初始化机械臂电机1编码器输入引脚
 * Timer2 CH1 <-> PA15 PWM输入模式
 */
void RobotArm1::initCounterDriver(void)
{
	RCC->APB1ENR |= 0x01;
	
	TIM2->CR1 = 0x00;
	TIM2->CR2 = 0x00;
	TIM2->PSC = 84 - 1;
	TIM2->ARR = 0xFFFFFFFF;
	TIM2->CCMR1 = 0x0201;
	TIM2->CCER = 0x0031;
	TIM2->SMCR = 0x54;
	TIM2->CR1 = 0x01;
	TIM2->EGR = 0x01;
}

/**
 * @brief 初始化机械臂电机1编码器输入引脚
 * Timer2 CH1 <-> PA15 <-> AF1 磁编码器1
 */
void RobotArm1::initCounterGpio(void)
{
	RCC->AHB1ENR |= 0x01; // Enable GPIOA
	
	GPIOA->MODER &= ~0xC0000000;
	GPIOA->MODER |= 0x80000000;
	GPIOA->PUPDR &= ~0xC0000000;
	GPIOA->PUPDR |= 0x40000000;
	GPIOA->OSPEEDR |= 0xC0000000;
	GPIOA->OTYPER &= ~0x8000;
	GPIOA->AFR[1] &= ~0xF0000000;
	GPIOA->AFR[1] |= 0x10000000;
}

/**
 * @brief 初始化机械臂电机1和机械臂电机2(仅仅速度控制引脚)
 * PC6 <-> TIM3_CH1 AF2 频率500Hz
 * PC9 <-> TIM3_CH4 AF2 频率500Hz
 */
void RobotArm1::initMotoDriver(void)
{
	RCC->APB1ENR |= 0x02;
	
	TIM3->CR1 = 0x00;
	TIM3->CR2 = 0x00;
	TIM3->PSC = 84 - 1;
	TIM3->ARR = 100 - 1;
	TIM3->CCR1 = 100;
	TIM3->CCR4 = 100;
	TIM3->CCMR1 = 0x0060;
	TIM3->CCMR2 = 0x6000;
	TIM3->CCER = 0x1001;
	TIM3->CR1 = 0x01;
	TIM3->EGR = 0x01;
}


/**
 * @brief 初始化机械臂电机1和机械臂电机2(仅仅速度控制引脚)
 * PC6 <-> TIM3_CH1 AF2
 * PC9 <-> TIM3_CH4 AF2
 * PE2 <-> OUT
 * PE3 <-> OUT
 */
void RobotArm1::initMotoGpio(void)
{
	RCC->AHB1ENR |= 0x14;
	
	GPIOC->MODER &= ~0xC3000;
	GPIOC->MODER |= 0x82000;
	GPIOC->PUPDR &= ~0xC3000;
	GPIOC->OSPEEDR |= 0xC3000;
	GPIOC->OTYPER &= ~0x240;
	GPIOC->AFR[0] &= ~0x0F000000;
	GPIOC->AFR[0] |= 0x02000000;
	GPIOC->AFR[1] &= ~0xF0;
	GPIOC->AFR[1] |= 0x20;
	
	GPIOE->MODER &= ~0x30000;
	GPIOE->MODER |= 0x10000;
	GPIOE->PUPDR &= ~0x30000;
	GPIOE->OTYPER &= ~0x100;
	GPIOE->OSPEEDR |= 0x30000;
	GPIOE->BSRRH = 0x100;
	
	GPIOE->MODER &= ~0xF0;
	GPIOE->MODER |= 0x50;
	GPIOE->PUPDR &= ~0xF0;
	GPIOE->OSPEEDR |= 0xF0;
	GPIOE->OTYPER &= ~0xC0;
}





