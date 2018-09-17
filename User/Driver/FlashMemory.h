#ifndef		__FLASH_MEMORY_H
#define		__FLASH_MEMORY_H

#include "Event/Sender.h"
#include "stm32f4xx.h"

#define ROBOT_ARM1_CHECK_DATA_INDEX			0
#define ROBOT_ARM2_CHECK_DATA_INDEX			1

/**
 * @brief 软件上FLASH存储器将抽象成一个32位数的二维数组
 * FlashArray[0][1024] 其中索引表示扇区位置
 */
class FlashMemory 
{
	public:
		static FlashMemory &instance(void) {
			static FlashMemory m;
			return m;
		}
		
		void vReadData(uint32_t address, uint8_t *data, uint32_t size);
		void vWriteData(uint32_t address, const uint8_t *data, uint32_t size);
		void vSectorEarse(uint32_t address);
		uint16_t xGetId(void);
		
		inline void put(uint16_t index, uint32_t value) { MemoryArrayMap[index] = value; }
		inline uint32_t get(uint16_t index) { return MemoryArrayMap[index]; }
		
		inline void put(uint8_t row, uint8_t col, uint32_t v) {
			put(row * 10 + col, v);
		}
		
		inline uint32_t get(uint8_t row, uint8_t col) {
			return get(row * 10 + col);
		}
		
		inline void save(void) { // 将MemoryArrayMap中的数据保存到FLASH
			vSectorEarse(0);
			vWriteData(0, (const uint8_t *) MemoryArrayMap, sizeof (MemoryArrayMap));
		}
		void load(void) { // 将Flash中的数据加载到MemoryArrayMap
			vReadData(0, (uint8_t *) MemoryArrayMap, sizeof (MemoryArrayMap) );
		}
	private:
		FlashMemory(void);
		void initSpiInterface(void);
	
		uint8_t xTranslateByte(uint8_t byte);
		uint8_t xQueryStatus(void);
		void vWriteEnable(void);
		void vPageProgram(uint32_t address, const uint8_t *data, uint32_t size);
	
		inline void vCsLow(void) { GPIOB->BSRRH = 0x40;}
		
		inline void vCsHigh(void) { GPIOB->BSRRL = 0x40;}
		
	private:
		uint32_t MemoryArrayMap[1024];
};





#endif
