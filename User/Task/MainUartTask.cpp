#include "Task/MainUartTask.h"
#include "Event/MainEvent.h"


MainUartTask::MainUartTask(void) : Task("Main", mStack, sizeof (mStack))
{}

void MainUartTask::run(void)
{
	MainEvent env;
	MainUart &uart = MainUart::instance();
	while (true)
	{
		uart.recv(&env);
		env.dispose();
	}
}		

