#include "Driver/FlashMemory.h"

#define READ_STATUS			0x05 // 读状态寄存器指令
#define WRITE_ENABLE		0x06 // 写使能
#define READ_ID				0x90 // 读ID
#define SECTOR_EARSE		0x20 // 扇区擦除
#define READ_DATA			0x03 // 读数据
#define PAGE_PROGRAM		0x02 // 页编程

FlashMemory::FlashMemory(void)
{
	initSpiInterface();
	load();
}

uint16_t FlashMemory::xGetId(void)
{
	uint16_t id;
	
	vCsLow();
	
	xTranslateByte(READ_ID);
	xTranslateByte(0x00);
	xTranslateByte(0x00);
	xTranslateByte(0x00);
	id = xTranslateByte(0x00);
	id <<= 8;
	id |= xTranslateByte(0x00);
	
	vCsHigh();
	
	return id;
}

void FlashMemory::vWriteData(uint32_t address, const uint8_t *data, uint32_t size)
{
	uint32_t page;
	
	if (address & 0xFF) // 地址没有256字节对齐
	{
		uint32_t alignAddr = (address + 0xFF) & (~0xFF);
		uint32_t radius = alignAddr - address; 
		if (radius > size) {
			vPageProgram(address, data, size);
			return;
		}
		vPageProgram(address, data, radius);
		address += radius;
		data += radius;
		size -= radius; // 这里已经保证字节对齐了
	}
	page = size / 0x100;
	
	while (page > 0) 
	{
		vPageProgram(address, data, 0x100);
		data += 0x100;
		address += 0x100;
		size -= 0x100;
		page --;
	}
	
	if (size > 0) {
		vPageProgram(address, data, size);
	}
}

void FlashMemory::vReadData(uint32_t address, uint8_t *data, uint32_t size)
{
	vCsLow();
	xTranslateByte(READ_DATA);
	xTranslateByte(address >> 16);
	xTranslateByte(address >> 8);
	xTranslateByte(address & 0xFF);
	while (size --)
	{
		*data = xTranslateByte(0xFF);
		data ++;
	}
	vCsHigh();
}

void FlashMemory::vPageProgram(uint32_t address, const uint8_t *data, uint32_t size)
{
	vWriteEnable();
	
	vCsLow();
	xTranslateByte(PAGE_PROGRAM);
	xTranslateByte(address >> 16);
	xTranslateByte(address >> 8);
	xTranslateByte(address & 0xFF);
	while (size --) {
		xTranslateByte(*data ++);
	}
	vCsHigh();
	
	while (xQueryStatus() & 0x01) {
		osDelay(10);
	}
}

void FlashMemory::vSectorEarse(uint32_t address)
{
	vWriteEnable();
	
	vCsLow();
	xTranslateByte(SECTOR_EARSE);
	xTranslateByte(address >> 16);
	xTranslateByte(address >> 8);
	xTranslateByte(address & 0xFF);
	vCsHigh();
	
	while (xQueryStatus() & 0x01) {
		osDelay(10);
	}
}

void FlashMemory::vWriteEnable(void)
{
	vCsLow();
	xTranslateByte(WRITE_ENABLE);
	vCsHigh();
}

uint8_t FlashMemory::xQueryStatus(void)
{
	uint8_t ret;
	vCsLow();
	xTranslateByte(READ_STATUS);
	ret = xTranslateByte(0xFF);
	vCsHigh();
	return ret;
}

uint8_t FlashMemory::xTranslateByte(uint8_t byte)
{
	while (0 == (SPI3->SR & 0x02));
	SPI3->DR = byte;
	while (0 == (SPI3->SR & 0x01));
	return SPI3->DR;
}

/**
 * @brief 
 * PB6 <-> CS
 * PB3 <-> SPI3_SCK
 * PB4 <-> SPI3_MISO
 * PB5 <-> SPI3_MOSI
 */
void FlashMemory::initSpiInterface(void)
{
	RCC->AHB1ENR |= 0x02;
	
	GPIOB->MODER &= ~0x3FC0;
	GPIOB->MODER |= 0x1A80;
	GPIOB->PUPDR &= ~0x3FC0;
	GPIOB->PUPDR |= 0x1540;
	GPIOB->OSPEEDR |= 0x3FC0;
	GPIOB->OTYPER &= ~0x78;
	GPIOB->AFR[0] &= ~0xFFF000;
	GPIOB->AFR[0] |= 0x666000;
	
	RCC->APB1ENR |= 0x8000;
	
	SPI3->CR1 = 0x00;
	SPI3->I2SCFGR = 0x00;
	SPI3->CR2 = 0x00;
	SPI3->CR1 = 0x0344;
	
	vCsHigh();
}











