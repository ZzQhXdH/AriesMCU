#include "Task/Task.h"


void Task::startThreadFun(void *arg)
{
	Task* t = (Task *) arg;
	t->run();
	t->sId = NULL;
	osThreadExit();
}


Task::Task(const char *name, uint64_t *stack, uint32_t stackSize, osPriority_t priority)
{
	sAttr.name = name;
	sAttr.attr_bits = osThreadDetached;
	sAttr.cb_mem = aCbMem;
	sAttr.cb_size = osRtxThreadCbSize;
	sAttr.stack_mem = stack;
	sAttr.stack_size = stackSize;
	sAttr.priority = priority;
	sAttr.tz_module = 0;
	sAttr.reserved = 0;
	sId = NULL;
}

void Task::start(void)
{
	if (sId != NULL) {
		stop();
	}
	sId = osThreadNew(startThreadFun, this, &sAttr);
}

void Task::stop(void)
{
	osThreadTerminate(sId);
	sId = NULL;
}




