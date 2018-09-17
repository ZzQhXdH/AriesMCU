#include "Task/MessageDispenseTask.h"





MessageDispenseTask::MessageDispenseTask(void) : Task("Message", mStack, sizeof (mStack))
{}

void MessageDispenseTask::run(void)
{
	MainUart &mainUart = MainUart::instance();
	LogUart &logUart = LogUart::instance();
	Bluetooth &ble = Bluetooth::instance();
	MainEvent mainEvent;
	LogEvent logEvent;
	BluetoothEvent bleEvent;
	osStatus_t ret;
	
	while (true)
	{
		ret = mainUart.recv(&mainEvent, 100);
		if (ret == osOK) {
			mainEvent.dispose();
		}
		ret = logUart.recv(&logEvent, 100);
		if (ret == osOK) {
			logEvent.dispose();
		}
		ret = ble.recv(&bleEvent, 100);
		if (ret == osOK) {
			bleEvent.dispose();
		}
	}
}



