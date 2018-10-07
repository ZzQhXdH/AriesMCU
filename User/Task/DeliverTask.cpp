#include "Task/DeliverTask.h"
#include "Custom/BaseProtocol.h"
#include "Custom/Protocol.h"
#include "Driver/MotorDriver.h"
#include "Driver/ELock.h"

#define FRIDGE_OPEN_SPEED				50
#define FRIDGE_CLOSE_SPEED				50

#define PICK_MOTOR_UP_SPEED				35
#define PICK_MOTOR_DOWN_SPEED			35

#define PICK_MOTOR_TEST_SETPS			600
#define PICK_MOTOR_START_STEPS			400
#define PICK_MOTOR_COMPRESS_STEPS		1000
#define PICK_MOTOR_DELIVER_STEPS		600

DeliverTask::DeliverTask(void) : Task("Deliver", mStack, sizeof (mStack)),
								 Receiver("Deliver", mMessageQueueMem, DELIVER_MSG_COUNT, DELIVER_MSG_SIZE)
{}

void DeliverTask::run(void)
{
	Event env;
	
	while (true)
	{
		recv(&env);
		
		switch (env.action())
		{
			case 0x01: robotArmCheck(env); break;
			case 0x02: testRobotArm(env); break;
			case 0x03: testFridge(env); break;
			case 0x04: testPickMotor(env); break;
			case 0x05: testCompressor(env); break;
			case 0x06: deliver(env); break;
			case 0x07: setGoodsType(env); break;
			case 0x09: init(env); break;
			case 0x0A: robotArmTest(env); break;
			case 0x0B: readGoodsType(env); break;
			case 0x0C: settingGoodsType(env); break;
			case 0x0F: ELock::vOn(); break;
		}
		MotorDriver::instance().disable();
	}
	
}

/**
 * @brief 校准机械手
 */
void DeliverTask::robotArmCheck(Event &env)
{
	uint16_t p1 = env.arg2(1);
	uint16_t p2 = env.arg2(3);
	RobotArm1::instance().vCheckPosition(p1);
	RobotArm2::instance().vCheckPosition(p2);
	FlashMemory::instance().save();
	uint16_t len = Protocol(mBuffer + 50).setAction(0x81).append1(0x00).build(mBuffer);
	env.onResult(mBuffer, len);
}

/**
 * @brief 一键设置货道
 */
void DeliverTask::settingGoodsType(Event &env)
{
	uint8_t row = env.arg(1);
	uint8_t col = env.arg(2);
	uint16_t p1 = env.arg2(3);
	uint16_t p2 = env.arg2(5);
	FlashMemory::instance().put(row, col, (p1 << 16) + p2);
	FlashMemory::instance().save();
	uint16_t len = Protocol(mBuffer + 50).setAction(0x8C).append1(row).append1(col).build(mBuffer);
	env.onResult(mBuffer, len);
}

/**
 * @brief 读取货道
 */
void DeliverTask::readGoodsType(Event &env)
{
	uint8_t row = env.arg(1);
	uint8_t col = env.arg(2);
	uint32_t value = FlashMemory::instance().get(row, col);
	uint16_t p1 = value >> 16;
	uint16_t p2 = value & 0xFFFF;
	uint16_t len = Protocol(mBuffer + 50)
			.setAction(0x8B)
			.append1(row)
			.append1(col)
			.append2(p1)
			.append2(p2)
			.build(mBuffer);	
	env.onResult(mBuffer, len);
}

/**
 * @brief 机械手测试
 */
void DeliverTask::robotArmTest(Event &env)
{
	uint8_t row = env.arg(1);
	uint8_t col = env.arg(2);
	uint32_t value = FlashMemory::instance().get(row, col);
	uint16_t p1 = value >> 16;
	uint16_t p2 = value & 0xFFFF;
	uint16_t timeOut = env.arg2(3);
	Fridge::instance().xOpen(FRIDGE_OPEN_SPEED, timeOut);
	PickMotor::instance().xGoUp(PICK_MOTOR_UP_SPEED, 0, timeOut);
	RobotArm1::instance().xRotationTo(518, timeOut, 10);
	RobotArm2::instance().xRotationTo(p2, timeOut);
	RobotArm1::instance().xRotationTo(p1, timeOut);
	RobotArm2::instance().vInversedBrake();
	
	PickMotor::instance().xGoDown(PICK_MOTOR_DOWN_SPEED, PICK_MOTOR_TEST_SETPS, timeOut);
}

/**
 * @brief 设备初始化
 */
void DeliverTask::init(Event &env)
{
	uint16_t timeOut = env.arg2(1);
	PickMotor::instance().xGoUp(PICK_MOTOR_UP_SPEED, 0, timeOut);
	RobotArm1::instance().xRotationTo(518, timeOut, 10);
	RobotArm2::instance().xRotationTo(518, timeOut, 10);
	Fridge::instance().xClose(FRIDGE_CLOSE_SPEED, timeOut);
}

/**
 * @brief 增加货道
 */
void DeliverTask::setGoodsType(Event &env)
{
	uint16_t p1 = RobotArm1::instance().xGetAbsolutePosition();
	uint16_t p2 = RobotArm2::instance().xGetAbsolutePosition();
	uint8_t row = env.arg(1);
	uint8_t col = env.arg(2);
	FlashMemory::instance().put(row, col, (p1 << 16) + p2);
	FlashMemory::instance().save();
	
	uint16_t len = Protocol(mBuffer + 50).setAction(0x87).append1(row).append1(col).append2(p1).append2(p2).build(mBuffer);
	env.onResult(mBuffer, len);
}

/**
 * @brief 多货道出货
 */
void DeliverTask::deliver(Event &env)
{
	uint8_t count = env.argCount() / 2 - 1;
	uint16_t timeOut = env.arg2(1);
	uint8_t row, col;
	uint8_t ret;

	for (uint8_t i = 0; i < count; i ++)
	{
		row = env.arg(i * 2 + 3); // row i
		col = env.arg(i * 2 + 4); // col i
		
		for (uint8_t i = 0; i < 2; i ++)
		{
			ret = deliver(row, col, timeOut, env);
			if (ret == 0x00) 
			{
				uint16_t len = Protocol(mBuffer + 50).setAction(0x86).append1(row).append1(col).append1(0x00).build(mBuffer);
				env.onResult(mBuffer, len);
				RobotArm1::instance().xRotationTo(518, timeOut, 10);
				RobotArm2::instance().xRotationTo(518, timeOut, 10);
				PickMotor::instance().xGoUp(PICK_MOTOR_UP_SPEED, 0, timeOut);
				Fridge::instance().xClose(FRIDGE_CLOSE_SPEED, timeOut);
				return;
			}
		}
		uint16_t len = Protocol(mBuffer + 50).setAction(0x86).append1(row).append1(col).append1(0x01).build(mBuffer);
		env.onResult(mBuffer, len);
	}
	
	uint16_t len = Protocol(mBuffer + 50).setAction(0x86).append1(row).append1(col).append1(0x02).build(mBuffer);
	env.onResult(mBuffer, len);
	RobotArm1::instance().xRotationTo(518, timeOut, 10);
	RobotArm2::instance().xRotationTo(518, timeOut, 10);
	PickMotor::instance().xGoUp(PICK_MOTOR_UP_SPEED, 0, timeOut);
	Fridge::instance().xClose(FRIDGE_CLOSE_SPEED, timeOut);
}

/**
 * @brief 单货道出货
 */
uint8_t DeliverTask::deliver(uint8_t row, uint8_t col, uint16_t timeOut, Event &env)
{
	uint32_t v = FlashMemory::instance().get(row, col);
	uint16_t p1 = v >> 16;
	uint16_t p2 = v & 0xFFFF;
	
	{
		PickMotor::Error e = PickMotor::instance().xGoUp(PICK_MOTOR_UP_SPEED, 0, timeOut); // 机械臂上升
		if (e != PickMotor::Ok) 
		{
			return 1;
		}
	}
	
	
	{
		RobotArm1::Error e1 = RobotArm1::instance().xRotationTo(518, timeOut, 10);
		if (e1 != RobotArm1::Ok) 
		{
			env.onStatusChange(row, col, 0x01); // 机械臂电机1回原点的时候超时
			return 1;
		}
	}
	
	{
		Fridge::Error e = Fridge::instance().xOpen(FRIDGE_OPEN_SPEED, timeOut);
		if (e != Fridge::Ok) 
		{
			MotorDriver::instance().enable();
			e = Fridge::instance().xOpen(FRIDGE_OPEN_SPEED, timeOut);
			if (e != Fridge::Ok) 
			{
				return 1;
			}
		}
	}
	
	{
		RobotArm2::Error e2 = RobotArm2::instance().xRotationTo(p2, timeOut);
		if (e2 != RobotArm2::Ok) 
		{
			RobotArm2::instance().xRotationTo(518, timeOut, 10);
			env.onStatusChange(row, col, 0x02); // 机械臂电机2去目标点的时候超时
			return 1;
		}
	}
	
	{
		RobotArm1::Error e1 = RobotArm1::instance().xRotationTo(p1, timeOut);
		if (e1 != RobotArm1::Ok) 
		{
			RobotArm1::instance().xRotationTo(518, timeOut, 10);
			env.onStatusChange(row, col, 0x03); // 机械臂电机1去目标点的时候超时
			return 1;
		}
	}
	
	{
		PickMotor::Error e = PickMotor::instance().xGoDownOfAbsolute(PICK_MOTOR_DOWN_SPEED, PICK_MOTOR_START_STEPS, timeOut);
		if (e != PickMotor::Ok) 
		{
			MotorDriver::instance().enable();
			PickMotor::instance().xGoUp(PICK_MOTOR_UP_SPEED, 0, timeOut);
			env.onStatusChange(row, col, 0x04); // 
			return 1;
		}
		
		Compressor::instance().on(); // 打开空压机
		e = PickMotor::instance().xGoDownAndCheckCompressor(PICK_MOTOR_DOWN_SPEED, PICK_MOTOR_COMPRESS_STEPS, timeOut);
		if (e != PickMotor::Ok) 
		{
			MotorDriver::instance().enable();
			Compressor::instance().off(); // 关闭空压机
			PickMotor::instance().xGoUp(PICK_MOTOR_UP_SPEED, 0, timeOut); // 机械臂上升
			return 1;
		}
	}
	
	{
		PickMotor::Error e = PickMotor::instance().xGoUp(PICK_MOTOR_UP_SPEED, 0, timeOut);
		if (e != PickMotor::Ok) 
		{
			MotorDriver::instance().enable();
			Compressor::instance().off(); // 关闭空压机
			return 1;
		}
	}

	{
		if (!Compressor::instance().isChecked()) // 物品掉落 出货失败
		{
			env.onStatusChange(row, col, 0x09);
			Compressor::instance().off();
			return 2;
		}
	}
	
	{
		RobotArm1::Error e1 = RobotArm1::instance().xRotationTo(518, timeOut, 5);
		if (e1 != RobotArm1::Ok) 
		{
			env.onStatusChange(row, col, 0x07); // 机械臂电机1回原点的时候超时
			Compressor::instance().off();
			Fridge::instance().xClose(FRIDGE_CLOSE_SPEED, timeOut);
			return 1;
		}
	}
	
	{
		RobotArm2::Error e2 = RobotArm2::instance().xRotationTo(518, timeOut, 5);
		if (e2 != RobotArm2::Ok) 
		{
			env.onStatusChange(row, col, 0x08); // 机械臂电机2回原点的时候超时
			Compressor::instance().off();
			Fridge::instance().xClose(FRIDGE_CLOSE_SPEED, timeOut);
			return 1;
		}
	}
	
	{
		if (!Compressor::instance().isChecked()) // 物品掉落 出货失败
		{
			env.onStatusChange(row, col, 0x09);
			Compressor::instance().off();
			return 2;
		}
	}
	
	{
		PickMotor::instance().xGoDown(PICK_MOTOR_DOWN_SPEED, PICK_MOTOR_DELIVER_STEPS, timeOut);
		Compressor::instance().off();
		osDelay(5000);
		env.onStatusChange(row, col, 0x0B); // 出货成功
		PickMotor::instance().xGoUp(PICK_MOTOR_UP_SPEED, 0, timeOut);
		Fridge::instance().xClose(FRIDGE_CLOSE_SPEED, timeOut);
		
		return 0;
	}
}

/**
 * @brief 控制空压机
 */
void DeliverTask::testCompressor(Event &env)
{
	uint8_t action = env.arg(1);
	switch (action)
	{
		case 0x00: Compressor::instance().on(); break;
		case 0x01: Compressor::instance().off(); break;
	}
	env.onReturn(0x01);
}

/**
 * @brief 控制冰箱
 */
void DeliverTask::testFridge(Event &env)
{
	uint8_t action = env.arg(1);
	uint8_t sp = env.arg(2);
	uint16_t timeOut = env.arg2(3);
	switch (action)
	{
		case 0x00: Fridge::instance().xOpen(sp, timeOut); break;
		case 0x01: Fridge::instance().xClose(sp, timeOut); break;
	}
	env.onReturn(0x01);
}

/**
 * @brief 控制升降电机
 */
void DeliverTask::testPickMotor(Event &env)
{
	uint8_t action = env.arg(1);
	uint8_t sp = env.arg(2);
	uint16_t step = env.arg2(3);
	uint16_t timeOut = env.arg2(5);
	switch (action)
	{
		case 0x00: PickMotor::instance().xGoUp(sp, step, timeOut); break;
		case 0x01: PickMotor::instance().xGoDown(sp, step, timeOut); break;
	}
	env.onReturn(0x01);
}

/**
 * @brief 控制机械手
 */
void DeliverTask::testRobotArm(Event &env)
{
	uint16_t p1 = env.arg2(1);
	uint16_t p2 = env.arg2(3);
	uint16_t t = env.arg2(5);
	RobotArm2::instance().xRotationTo(p2, t);
	RobotArm1::instance().xRotationTo(p1, t);
	env.onReturn(0x01);
}


