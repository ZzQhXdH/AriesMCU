#ifndef		__BASE_PROTOCOL_H
#define		__BASE_PROTOCOL_H


#include <stdint.h>
#include "Event/Event.h"
#include "Driver/LogUart.h"
#include "Driver/MainUart.h"
#include "Driver/Bluetooth.h"

class BaseProtocol
{
	public:
		BaseProtocol(uint8_t *buffer, const Event &env, uint8_t status);
		BaseProtocol(uint8_t *buffer, uint8_t action, const uint8_t *args, uint16_t size);
		
		inline void write(void) {
			LogUart::instance().write((const uint8_t *) mBuffer, mLength);
		}
		
		inline void writeOfLog(void) {
			LogUart::instance().write((const uint8_t *) mBuffer, mLength);
		}
		
		inline void writeOfMain(void) {
			MainUart::instance().write((const uint8_t *) mBuffer, mLength);
		}
		
		inline void writeOfBluetooth(void) {
			Bluetooth::instance().write( (const uint8_t *) mBuffer, mLength );
		}
	
	private:
		uint8_t *mBuffer;
		uint16_t mLength;
};



#endif
