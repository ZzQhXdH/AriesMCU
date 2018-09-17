#include "Driver/LogUart.h"
#include <stdio.h>
#include <stdarg.h>


static uint64_t RecvBuffer[512 / 8];
static uint64_t RecvSingleBuffer[64 / 8];

static LogEvent MsgRecv;

/**
 * @brief LogUart Init
 * TX <-> PC12 <-> UART5_TX <-> AF8
 * RX <-> PD2 <-> UART5_RX <-> AF8 <-> DMA1_Stream0 Channel4
 */
LogUart::LogUart(void) : 
	Sender("Log"), 
	Receiver("Log", qMqMem, LOG_OBJ_NUMBER, LOG_OBJ_SIZE),
	BaseOutput(aBufferMemory)
{
	RCC->AHB1ENR |= 0x0C;
	
	/**
	 * @brief PC12 <-> UART5_TX <-> AF8
	 */
	GPIOC->MODER &= ~0x03000000;
	GPIOC->MODER |= 0x02000000;
	GPIOC->PUPDR &= ~0x03000000;
	GPIOC->PUPDR |= 0x01000000;
	GPIOC->OSPEEDR |= 0x03000000;
	GPIOC->OTYPER &= ~0x1000;
	GPIOC->AFR[1] &= ~0xF0000;
	GPIOC->AFR[1] |= 0x80000;
	
	/**
	 * @brief PD2 <-> UART5_RX <-> AF8
	 */
	GPIOD->MODER &= ~0x30;
	GPIOD->MODER |= 0x20;
	GPIOD->PUPDR &= ~0x30;
	GPIOD->PUPDR |= 0x10;
	GPIOD->OSPEEDR |= 0x30;
	GPIOD->OTYPER &= ~0x04;
	GPIOD->AFR[0] &= ~0xF00;
	GPIOD->AFR[0] |= 0x800;
	
	/**
	 * @brief 
	 */
	RCC->APB1ENR |= 0x100000;
	
	UART5->CR1 = 0x00;
	UART5->CR2 = 0x00;
	UART5->CR3 = 0x40;
	UART5->BRR = 42000000 / 9600;
	UART5->CR1 = 0x201C;
	
	RCC->AHB1ENR |= 0x200000; // DMA1
	
	DMA1->LIFCR = 0x3F;
	DMA1_Stream0->CR = 0x00;
	DMA1_Stream0->M0AR = (uint32_t) &RecvBuffer;
	DMA1_Stream0->PAR = (uint32_t) &UART5->DR;
	DMA1_Stream0->NDTR = sizeof(RecvBuffer);
	DMA1_Stream0->CR = 	(0x04 << 25) | // 第四通道
						(0x00 << 23) | // 存储器单次传输
						(0x00 << 21) | // 外设单次传输
						(0x00 << 19) | // 使用M0
						(0x00 << 18) | // 不使用双缓冲区
						(0x02 << 16) | // 优先级高
						(0x00 << 15) | // 外设偏移量与PSIZE相关
						(0x00 << 13) | // 存储器大小为8位
						(0x00 << 11) | // 外设大小为8位
						(0x01 << 10) | // 每次数据传输以后存储器地址递增
						(0x00 << 9) | // 外设地址不递增
						(0x01 << 8) | // 循环模式
						(0x00 << 6) | // 数据传输方向: 外设到存储器
						(0x00 << 5) | // 外设是流控制器
						(0x01 << 0) ; // 使能DMA
	NVIC_EnableIRQ(UART5_IRQn);
}

void LogUart::write(uint8_t byte)
{
	while (0 == (UART5->SR & 0x80));
	UART5->DR = byte;
}

void LogUart::write(const uint8_t *byteArray, uint16_t len)
{
	acquire();
	while (len --)
	{
		write(*byteArray);
		byteArray ++;
	}
	release();
}


static inline void vBufferOver(uint32_t index, uint32_t lastIndex)
{
	uint32_t k = 0;
	uint8_t *p1 = (uint8_t *) RecvSingleBuffer;
	uint8_t *p2 = (uint8_t *) RecvBuffer;
	for (uint32_t i = lastIndex; i < sizeof (RecvBuffer); i ++, k ++) {
		p1[k] = p2[i];
	}
	for (uint32_t i = 0; i < index; i ++, k ++) {
		p1[k] = p2[i];
	}
	MsgRecv.set(p1, k);
}


extern "C" void UART5_IRQHandler(void)
{
	static uint32_t lastIndex;
	uint32_t index;
	if (0 == (UART5->SR & 0x10)) {
		return ;
	}
	index = UART5->DR;
	index = sizeof(RecvBuffer) - DMA1_Stream0->NDTR;
	if (index > lastIndex) {
		MsgRecv.set((const uint8_t *) (RecvBuffer) + lastIndex, index - lastIndex);
	} else {
		vBufferOver(index, lastIndex);
	}
	LogUart::instance().put(&MsgRecv);
	lastIndex = index;	
}





