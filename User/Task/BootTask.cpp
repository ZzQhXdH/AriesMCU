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
	Compressor::instance(); // 空压机
	Ds18b20::instance(); // 温度传感器
	ELock::vInit();
	
	LogUart::instance(); 
	MainUart::instance();
	
	PositionSwitch::instance(); // 位置开关
	
	RobotArm1::instance(); // 机械臂电机1
	RobotArm2::instance(); // 机械臂电机2
	PickMotor::instance(); // 升降电机
	Fridge::instance(); // 冰箱门电机
	
	DeliverTask::instance().start();
	MessageDispenseTask::instance().start();
	StatusUpdateTask::instance().start();

	Led::instance().blink();
}




