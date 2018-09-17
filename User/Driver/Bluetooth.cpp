#include "Driver/Bluetooth.h"


static uint64_t RecvBuffer[512 / 8];
static uint64_t RecvSingleBuffer[64 / 8];

static BluetoothEvent BleEnv;

Bluetooth::Bluetooth(void) : 
	Receiver("Bluetooth", aMqMem, BLUETOOTH_MESSAGE_OBJ_NUMBER, BLUETOOTH_MESSAGE_OBJ_SIZE),
	Sender("Bluetooth"),
	BaseOutput(aBufferMemory)
{
	initDriver();
}

void Bluetooth::write(uint8_t byte) 
{
	while (0 == (USART2->SR & 0x80));
	USART2->DR = byte;
}

void Bluetooth::write(const uint8_t *byteArray, uint16_t length) 
{
	acquire();
	while (length --)
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
	BleEnv.set(p1, k);
}

extern "C" void USART2_IRQHandler(void)
{
	static uint32_t lastIndex;
	uint32_t index;
	if (0 == (USART2->SR & 0x10)) {
		return ;
	}
	index = USART2->DR;
	index = sizeof(RecvBuffer) - DMA1_Stream5->NDTR;
	if (index > lastIndex) {
		BleEnv.set((const uint8_t *) (RecvBuffer) + lastIndex, index - lastIndex);
	} else {
		vBufferOver(index, lastIndex);
	}
	Bluetooth::instance().put(&BleEnv);
	lastIndex = index;
}


/**
 * @brief PD5 <-> USART2_TX
 * PD6 <-> USART2_RX <-> DMA1_Stream5_Channel4
 * PD8 <-> STATE
 * PD7 <-> EN
 */
void Bluetooth::initDriver(void)
{
	RCC->APB1ENR |= 0x20000;
	RCC->AHB1ENR |= 0x08;
	
	GPIOD->MODER &= ~0x3FC00;
	GPIOD->MODER |= 0x06800;
	GPIOD->PUPDR &= ~0x3FC00;
	GPIOD->PUPDR |= 0x1400;
	GPIOD->OSPEEDR |= 0xFC00;
	GPIOD->OTYPER &= ~0xE0;
	GPIOD->OTYPER |= 0x80;
	GPIOD->AFR[0] &= ~0xFF00000;
	GPIOD->AFR[0] |= 0x7700000;
	enable();
	
	USART2->CR1 = 0x00;
	USART2->CR2 = 0x00;
	USART2->CR3 = 0x40;
	USART2->BRR = 42000000 / 9600;
	USART2->CR1 = 0x201C;
	
	RCC->AHB1ENR |= 0x200000;
	DMA1->HIFCR = 0x0FC0;
	
	DMA1_Stream5->CR = 0x00;
	DMA1_Stream5->M0AR = (uint32_t) &RecvBuffer;
	DMA1_Stream5->PAR = (uint32_t) &USART2->DR;
	DMA1_Stream5->NDTR = sizeof (RecvBuffer);
	DMA1_Stream5->CR = 	(0x04 << 25) | // 第四通道
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
	NVIC_EnableIRQ(USART2_IRQn);
}







