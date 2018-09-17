#ifndef		__MAIN_UART_H
#define		__MAIN_UART_H

#include "Event/Sender.h"
#include "Event/Receiver.h"
#include "stm32f4xx.h"
#include "Event/MainEvent.h"
#include "Custom/BaseOutput.h"


#define MAIN_UART_OBJ_SIZE		sizeof (MainEvent)
#define MAIN_UART_OBJ_NUMBER	20
#define MAIN_UART_SIZE			osRtxMessageQueueMemSize(MAIN_UART_OBJ_NUMBER, MAIN_UART_OBJ_SIZE)

class MainUart : public Sender, public Receiver, public BaseOutput
{
	public:
		static MainUart &instance(void) {
			static MainUart uart;
			return uart;
		}
		
		void write(uint8_t byte);
		
		virtual void write(const uint8_t *byteArray, uint16_t len);
		
	private:
		MainUart(void);
		
	private:
		uint64_t qMqMem[ (MAIN_UART_SIZE + 7) / 8 ];
		uint8_t aBufferMemory[150];
};





#endif
