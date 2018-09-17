#include "stm32f4xx.h"
#include "Task/BootTask.h"


int main(void)
{
	osKernelInitialize();
	BootTask::instance().start();
	osKernelStart();
}







