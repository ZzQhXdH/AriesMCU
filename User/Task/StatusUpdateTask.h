#ifndef		__STATUS_UPDATE_TASK_H
#define		__STATUS_UPDATE_TASK_H

#include "Task/Task.h"

class StatusUpdateTask : public Task
{
	public:
		static StatusUpdateTask &instance(void) {
			static StatusUpdateTask task;
			return task;
		}
	
	protected:
		virtual void run(void);
		
	private:
		StatusUpdateTask(void);
	
	private:
		uint64_t mStack[512 / 8];

		uint16_t nTemperature;
		uint16_t nPosition1;
		uint16_t nPosition2;
		uint16_t nState;
};





#endif
