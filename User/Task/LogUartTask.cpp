#include "Task/LogUartTask.h"
#include "Driver/LogUart.h"

LogUartTask::LogUartTask(void) : Task("LogUart", mStack, sizeof (mStack))
{
	
}

void LogUartTask::run(void)
{
	LogEvent env;
	LogUart &uart = LogUart::instance();
	while (true)
	{
		uart.recv(&env);
		env.dispose();
	}
}


