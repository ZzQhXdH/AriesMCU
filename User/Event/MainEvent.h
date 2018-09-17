#ifndef		__MAIN_EVENT_H
#define		__MAIN_EVENT_H

#include "Event/Event.h"


/**
 * @brief 串口接收 事件处理
 */
class MainEvent : public Event
{
	public:
		void dispose(void);
		
		void onOTANotify(void);
	
		void onPackage(void);
	
		virtual void onResult(const uint8_t *buffer, uint16_t length);
	
	private:
		bool isCorrectNoCheckLength(void);
	
		void write(uint8_t action, uint8_t arg1);
	
		void write(uint8_t action, uint8_t arg1, uint8_t arg2);
	
	private:
		uint8_t buffer[100];
};



#endif
