#ifndef		__LOG_UART_H
#define		__LOG_UART_H

#include "stm32f4xx.h"
#include "Event/LogEvent.h"
#include "Custom/BaseOutput.h"


#define LOG_OBJ_NUMBER				sizeof (LogEvent)
#define LOG_OBJ_SIZE				20
#define LOG_SIZE					osRtxMessageQueueMemSize(LOG_OBJ_NUMBER, LOG_OBJ_SIZE)


class LogUart : public Sender, public Receiver, public BaseOutput 
{
	public:
		static LogUart &instance(void) {
			static LogUart uart;
			return uart;
		}
	
		void write(uint8_t byte);
		
		virtual void write(const uint8_t *byteArray, uint16_t len);
		
	private:
		LogUart(void);
	
	private:
		uint64_t qMqMem[ (LOG_SIZE + 7) / 8 ];
		uint8_t aBufferMemory[150];
};

#define LOG(format, ...)		LogUart::instance()->write(format, __VA_ARGS__)

#endif
