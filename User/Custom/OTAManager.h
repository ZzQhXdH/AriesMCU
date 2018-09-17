#ifndef		__OTA_MANAGER_H
#define		__OTA_MANAGER_H

#include "Driver/FlashMemory.h"
#include "Custom/Protocol.h"
#include "Driver/MainUart.h"

#define OTA_MESSAGE_ADDRESS		(9 * 4 * 1024)
#define OTA_ADDRESS				(10 * 4 * 1024)
#define SECTOR_SIZE				(4 * 1024)

class OTAManager
{
	public:
		static OTAManager &instance(void) {
			static OTAManager t;
			return t;
		}
		
		void init(uint32_t size) 
		{
			nPackageAllSize = size;
			nAlreadyBytes = 0;
			nLastId = 0;
			nSectorId = 0;
			uint16_t count = (size + SECTOR_SIZE - 1) / SECTOR_SIZE;
			setOTAMessage(size);
			{
				uint16_t len = Protocol(buffer + 50).setAction(0x7e).append2(count).build(buffer);
				MainUart::instance().write(buffer, len);
			}
			
			uint32_t address = OTA_ADDRESS;
			for (uint16_t i = 0; i < count; i ++)
			{
				FlashMemory::instance().vSectorEarse(address);
				address += SECTOR_SIZE;
			}
		}
		
		bool xWrite(uint16_t id, const uint8_t *buffer, uint32_t size);
		
	private:
		void setOTAMessage(uint32_t size);
		
	private:
		OTAManager(void);
		uint8_t buffer[100];
		uint64_t mBuffer[1024 * 4 / 8];
		uint32_t nAlreadyBytes;
		uint32_t nPackageAllSize; // 升级包的大小 单位BYTE
		uint16_t nLastId;
		uint16_t nSectorId;
};







#endif
