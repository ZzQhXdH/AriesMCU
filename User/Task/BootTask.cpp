#include "Task/BootTask.h"
#include "Task/MessageDispenseTask.h"
#include "Task/StatusUpdateTask.h"
#include "Task/DeliverTask.h"
#include "Driver/Led.h"
#include "Driver/MotorDriver.h"
#include "Driver/ELock.h"


BootTask::BootTask(void) : Task("Boot", mStack, sizeof (mStack))
{
}

void BootTask::run(void)
{
	SysTimer::vInit();

	MotorDriver::instance();  // DRV8704
	FlashMemory::instance(); // FLASH W25Q128
	Bluetooth::instance(); // BLUETOOTH
	Compressor::instance(); // ��ѹ��
	Ds18b20::instance(); // �¶ȴ�����
	ELock::vInit();
	
	LogUart::instance(); 
	MainUart::instance();
	
	PositionSwitch::instance(); // λ�ÿ���
	
	RobotArm1::instance(); // ��е�۵��1
	RobotArm2::instance(); // ��е�۵��2
	PickMotor::instance(); // �������
	Fridge::instance(); // �����ŵ��
	
	DeliverTask::instance().start();
	MessageDispenseTask::instance().start();
	StatusUpdateTask::instance().start();

	Led::instance().blink();
}




