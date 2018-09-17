#ifndef		__MAIN_UART_TASK_H
#define		__MAIN_UART_TASK_H

#include "Task/Task.h"
#include "Driver/MainUart.h"

class MainUartTask : public Task
{
	public:
		static inline MainUartTask &instance(void) {
			static MainUartTask INSTANCE;
			return INSTANCE;
		}
		
	protected:
		virtual void run(void);
		
	private:
		MainUartTask(void);
	
	private:
		uint64_t mStack[512 / 8];
};





#endif
