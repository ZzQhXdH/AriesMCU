#ifndef		__MESSAGE_DISPENSE_TASK_H
#define		__MESSAGE_DISPENSE_TASK_H

#include "Driver/LogUart.h"
#include "Driver/MainUart.h"
#include "Driver/Bluetooth.h"
#include "Event/BluetoothEvent.h"
#include "Event/LogEvent.h"
#include "Event/MainEvent.h"
#include "Task/Task.h"

class MessageDispenseTask : public Task
{
	public:
		static MessageDispenseTask &instance(void) {
			static MessageDispenseTask INSTANCE;
			return INSTANCE;
		}
		
	protected:
		virtual void run(void);
	
	private:
		MessageDispenseTask(void);
	
	private:
		uint64_t mStack[ 1024 / 8 ];
};


#endif
