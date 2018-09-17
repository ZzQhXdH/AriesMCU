#include "Driver/MotorDriver.h"


/**
 * PB7 <=> SCLK
 * PE8, PB8 <=> SCS
 * PA1 <=> SDATI
 * PA2 <=> SDATO
 */
#define xGetSDO()	(GPIOA->IDR & 0x04) //PA2
 
#define vSetSDI()	GPIOA->BSRRL = 0x02 // PA1 
#define vClrSDI()	GPIOA->BSRRH = 0x02 // PA1

#define vSetCS1()	GPIOE->BSRRL = 0x100 // PE8
#define vSetCS2()	GPIOB->BSRRL = 0x100 // PB8
 
#define vClrCS1()	GPIOE->BSRRH = 0x100 // PE8
#define vClrCS2()	GPIOB->BSRRH = 0x100 // PB8

#define vSetSCLK()	GPIOB->BSRRL = 0x80 // PB7
#define vClrSCLK()	GPIOB->BSRRH = 0x80 // PB7

 
static void vSpiWrite(uint8_t address, uint16_t data)
{
	uint16_t value = ((address & 0x07) << 12) + (data & 0xFFF);
	vSetCS1();
	vSetCS2();
	for (uint32_t i = 0; i < 16; i ++)
	{
		if (value & 0x8000) {
			vSetSDI();
		} else{
			vClrSDI();
		}
		vSetSCLK();
		value <<= 1;
		vClrSCLK();
	}
	vClrCS1();
	vClrCS2();
}

static void vSpiInit(void)
{
	RCC->AHB1ENR |= 0x01 | 0x02 | 0x10;
	
	// PA2
	GPIOA->MODER &= ~0x30;
	GPIOA->PUPDR &= ~0x30;
	
	// PA1 SDATI
	GPIOA->MODER &= ~0x0C;
	GPIOA->MODER |= 0x04;
	GPIOA->OSPEEDR |= 0x0C;
	GPIOA->PUPDR &= ~0x0C;
	GPIOA->OTYPER &= ~0x02;
	
	// PB8
	GPIOB->MODER &= ~0x30000;
	GPIOB->MODER |= 0x10000;
	GPIOB->OSPEEDR |= 0x30000;
	GPIOB->OTYPER &= ~0x100;
	GPIOB->PUPDR &= ~0x30000;
	
	// PE8 SCS
	GPIOE->MODER &= ~0x30000;
	GPIOE->MODER |= 0x10000;
	GPIOE->OSPEEDR |= 0x30000;
	GPIOE->OTYPER &= ~0x100;
	GPIOE->PUPDR &= ~0x30000;

	// PB7 SCLK
	GPIOB->MODER &= ~0xC000;
	GPIOB->MODER |= 0x4000;
	GPIOB->PUPDR &= ~0xC000;
	GPIOB->OSPEEDR |= 0xC000;
	GPIOB->OTYPER &= ~0x80;
	
	vClrCS1();
	vClrCS2();
	vClrSCLK();
}

void vDrv8704Reset(void) // DRV8704复位
{
	GPIOC->BSRRL = 0x10; // 开始工作
	
	GPIOC->BSRRL = 0x20; // 复位
	osDelay(10);
	GPIOC->BSRRH = 0x20;
	osDelay(10);
	
	vSpiWrite(0x00, 0x01);
	vSpiWrite(0x07, 0x00);
}


void vDrv8704Init(void)
{
	// 初始化DRV8704
	// PC4 <=> SLEEPn
	// PC5 <=> RESET
	RCC->AHB1ENR |= 0x04;
	
	GPIOC->MODER &= ~0xF00;
	GPIOC->MODER |= 0x500;
	GPIOC->PUPDR &= ~0xF00;
	GPIOC->OSPEEDR |= 0xF00;
	GPIOC->OTYPER &= ~0x30;
	
	vSpiInit();
	
	GPIOC->BSRRH = 0x10; // DRV8704进入休眠模式
}


MotorDriver::MotorDriver(void)
{
	vDrv8704Init();
	disable();
}

void MotorDriver::clear(void)
{
	vSpiWrite(0x07, 0x00);
}

void MotorDriver::enable(void)
{
	GPIOC->BSRRH = 0x10; // 休眠
	osDelay(10);
	GPIOC->BSRRL = 0x10; // 退出休眠
	
	GPIOC->BSRRL = 0x20; // 复位
	osDelay(10);
	GPIOC->BSRRH = 0x20;
	osDelay(10);
	
	vSpiWrite(0x00, 0xC01);
}

void MotorDriver::disable(void)
{
	GPIOC->BSRRH = 0x10; // 休眠
}




