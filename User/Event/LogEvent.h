#ifndef		__LOG_EVENT_H
#define		__LOG_EVENT_H


#include "Event/Sender.h"
#include "Event/Receiver.h"

class LogEvent : public Event
{
	public:
		void dispose(void);
	
	private:
		void onTestRobotArm(void);
	
		void onCheckRobotArm(void);
	
	protected:
		virtual void onReturn(uint8_t status);
	
		virtual void onStatusChange(uint8_t row, uint8_t col, uint8_t status);
	
	private:
		uint8_t buffer[100];
};




#endif
