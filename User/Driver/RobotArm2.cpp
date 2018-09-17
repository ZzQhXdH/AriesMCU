#include "Driver/RobotArm2.h"
#include "Driver/MotorDriver.h"

RobotArm2::Error RobotArm2::xRotationTo(uint16_t position, uint32_t timeOut, uint16_t range)
{
	int32_t v;
	uint32_t count = 0;
	int32_t a = xGetAbsolutePosition() - position;
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
			vSetDir(CW);
			vSetSpeed(v);
		} else if (v < 0) {
			vSetDir(CCW);
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


void RobotArm2::vInversedBrake(void)
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
	

uint16_t RobotArm2::xGetAbsolutePosition(void)
{
	int32_t v = xGetRawPosition() + nCheckData;
	if (v < 0) {
		return v + 1036;
	} else if (v > 1036) {
		return v - 1036;
	}
	return v;
}

void RobotArm2::vSetDir(Dir dir)
{
	switch (dir)
	{
		case CW:
			GPIOE->BSRRH = 0x20;
			GPIOE->BSRRL = 0x10;
			break;
				
		case CCW:
			GPIOE->BSRRL = 0x20;
			GPIOE->BSRRH = 0x10;
			break;
				
		case Brake:
			GPIOE->BSRRH = 0x30;
			//vSetSpeed(120);
			break;
	}
}

RobotArm2::RobotArm2(void) : Sender("RobotArm2")
{
	initMotoGpio();
	initMotoDriver();
	initCounterGpio();
	initCounterDriver();
	vSetDir(Brake);
	nCheckData = FlashMemory::instance().get ( ROBOT_ARM2_CHECK_DATA_INDEX );
}

void RobotArm2::initCounterDriver(void)
{
	RCC->APB1ENR |= 0x08;
	
	TIM5->CR1 = 0x00;
	TIM5->CR2 = 0x00;
	TIM5->PSC = 84 - 1;
	TIM5->ARR = 0xFFFFFFFF;
	TIM5->CCMR1 = 0x0201;
	TIM5->CCER = 0x31;
	TIM5->SMCR = 0x54;
	TIM5->CR1 = 0x01;
	TIM5->EGR = 0x01;
}

/**
 * @brief Timer5 CH1 <-> PA0 <-> AF2 磁编码器2
 */
void RobotArm2::initCounterGpio(void)
{
	RCC->AHB1ENR |= 0x01;
	
	GPIOA->MODER &= ~0x03;
	GPIOA->MODER |= 0x02;
	GPIOA->PUPDR &= ~0x03;
	GPIOA->PUPDR |= 0x01;
	GPIOA->OSPEEDR |= 0x03;
	GPIOA->OTYPER &= ~0x01;
	GPIOA->AFR[0] &= ~0x0F;
	GPIOA->AFR[0] |= 0x02;
}

void RobotArm2::initMotoDriver(void) 
{
	
}

/**
 * @brief 初始化机械臂电机2的控制引脚
 * PE4 <-> OUT
 * PE5 <-> OUT
 */
void RobotArm2::initMotoGpio(void)
{
	RCC->AHB1ENR |= 0x10;
	
	GPIOE->MODER &= ~0xF00;
	GPIOE->MODER |= 0x500;
	GPIOE->PUPDR &= ~0xF00;
	GPIOE->OSPEEDR |= 0xF00;
	GPIOE->OTYPER &= ~0x30;
}







