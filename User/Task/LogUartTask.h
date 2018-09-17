#ifndef		__LOG_UART_TASK_H
#define		__LOG_UART_TASK_H

#include "Task/Task.h"

class LogUartTask : public Task
{
	public:
		static inline LogUartTask &instance(void) {
			static LogUartTask task;
			return task;
		}
		
	protected:
		virtual void run(void);
		
	private:
		LogUartTask(void);
	
	private:
		uint64_t mStack[ 512 / 8 ];
};	





#endif
