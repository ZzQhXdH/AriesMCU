#include "Custom/OTAManager.h"

#include "Custom/Protocol.h"

void copy(void *data, const void *src, uint32_t len)
{
	uint8_t *p = (uint8_t *) data;
	const uint8_t *s = (const uint8_t *) src;
	for (uint32_t i = 0; i < len; i ++)
	{
		p[i] = s[i];
	}
}

OTAManager::OTAManager(void)
{
	
}

void OTAManager::setOTAMessage(uint32_t size)
{
	FlashMemory::instance().vSectorEarse(OTA_MESSAGE_ADDRESS);
	uint8_t buf[4] = {size >> 24, size >> 16, size >>8, size & 0xFF};
	
	{
		uint16_t len = Protocol(this->buffer + 50)
			.setAction(0x7c)
			.append1(size >> 21)
			.append1(size >> 14)
			.append1(size >> 7)
			.append1(size).build(this->buffer);
		MainUart::instance().write(this->buffer, len);
	}
	
	FlashMemory::instance().vWriteData(OTA_MESSAGE_ADDRESS, (const uint8_t *)buf, 4);
	FlashMemory::instance().vReadData(OTA_MESSAGE_ADDRESS, buf, 4);
	{
		uint16_t len = Protocol(this->buffer + 50)
			.setAction(0x7b)
			.append1(buf[0])
			.append1(buf[1])
			.append1(buf[2])
			.append1(buf[3]).build(this->buffer);
		MainUart::instance().write(this->buffer, len);
	}
}

bool OTAManager::xWrite(uint16_t id, const uint8_t *buffer, uint32_t size)
{
	if ((id == 0) || ((nLastId + 1) == id))
	{
		nLastId = id;
		uint8_t *p = (uint8_t *) mBuffer;
		copy(p + nAlreadyBytes, buffer, size);
		nAlreadyBytes += size;
		
		if (nAlreadyBytes == sizeof (mBuffer))
		{
			FlashMemory::instance().vWriteData(OTA_ADDRESS + nSectorId * SECTOR_SIZE, (const uint8_t *) mBuffer, nAlreadyBytes);
			nSectorId ++;
			nAlreadyBytes = 0;
		}
		
		uint32_t all_size = nSectorId * SECTOR_SIZE + nAlreadyBytes;
		
		if (all_size == nPackageAllSize)
		{
			FlashMemory::instance().vWriteData(OTA_ADDRESS + nSectorId * SECTOR_SIZE, (const uint8_t *) mBuffer, nAlreadyBytes);
			nSectorId ++;
			nAlreadyBytes = 0;
			
			{
				uint16_t len = Protocol(this->buffer + 50)
				.setAction(0x7a)
				.append1(all_size >> 24)
				.append1(all_size >> 16)
				.append1(all_size >> 8)
				.append1(all_size).build(this->buffer);
				MainUart::instance().write(this->buffer, len);
			}
			
			NVIC_SystemReset(); // 重启
		}
	}
	else 
	{
		return false;
	}
	return true;
}




