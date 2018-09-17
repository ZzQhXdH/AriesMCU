#ifndef		__BLUETOOTH_H
#define		__BLUETOOTH_H

#include "Event/Receiver.h"
#include "Event/Sender.h"
#include "Event/BluetoothEvent.h"
#include "Custom/BaseOutput.h"
#include "stm32f4xx.h"

#define BLUETOOTH_MESSAGE_OBJ_SIZE			sizeof(BluetoothEvent)
#define BLUETOOTH_MESSAGE_OBJ_NUMBER		100
#define BLUETOOTH_MESSAGE_SIZE				osRtxMessageQueueMemSize(BLUETOOTH_MESSAGE_OBJ_NUMBER, BLUETOOTH_MESSAGE_OBJ_SIZE)

#define MASTER	"AT+ROLE1\r\n"
#define SLAVE	"AT+ROLE0\r\n"

class Bluetooth : public Receiver, public Sender, public BaseOutput
{
	public:
		static Bluetooth &instance(void) {
			static Bluetooth ble;
			return ble;
		}
	
	public:
		Bluetooth(void);
		
		virtual void write(const uint8_t *byteArray, uint16_t length);
	
		void write(uint8_t byte);
		
		inline void enable(void) { 
			GPIOD->BSRRH = 0x80; 
			enableFlag = true;
		}
		
		inline void disable(void) { 
			GPIOD->BSRRL = 0x80; 
			enableFlag = false;
		}
		
		inline bool isEnable(void) { return enableFlag; }
		
	private:
		void initDriver(void);
	
	private:
		uint64_t aMqMem[ (BLUETOOTH_MESSAGE_SIZE + 7) / 8 ];
		uint8_t aBufferMemory[64];
		bool enableFlag;
};

#endif
