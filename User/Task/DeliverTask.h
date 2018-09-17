#ifndef		__DELIVER_TASK_H
#define		__DELIVER_TASK_H

#include "Driver/RobotArm1.h"
#include "Driver/RobotArm2.h"
#include "Driver/Fridge.h"
#include "Driver/PickMotor.h"
#include "Driver/Compressor.h"
#include "Task/Task.h"
#include "Event/Receiver.h"


#define DELIVER_MSG_COUNT		60
#define DELIVER_MSG_SIZE		sizeof ( Event )
#define DELIVER_MQ_SIZE			osRtxMessageQueueMemSize( DELIVER_MSG_COUNT, DELIVER_MSG_SIZE )

class DeliverTask : public Task, public Receiver
{
	public:
		static DeliverTask &instance(void) 
		{
			static DeliverTask INSTANCE;
			return INSTANCE;
		}
		
		void robotArmCheck(Event &env); // 校准机械手
		
		void deliver(Event &env); // 出货
		
		void testFridge(Event &env); // 测试冰箱门
		
		void testPickMotor(Event &env); // 测试升降电机
		
		void testCompressor(Event &env); // 测试空压机
		
		void testRobotArm(Event &env); // 测试机械手
		
		void setGoodsType(Event &env); // 设置货道
		
		void init(Event &env); // 初始化
		
		void robotArmTest(Event &env); // 测试机械手
		
		void readGoodsType(Event &env); // 读取货道
		
		void settingGoodsType(Event &env); // 设置货道
		
	protected:
		virtual void run(void);
	
	private:
		DeliverTask(void);
		
		uint8_t deliver(uint8_t row, uint8_t col, uint16_t timeOut, Event &env);
	
	private:
		uint64_t mStack[ 1024 / 8 ];
		uint64_t mMessageQueueMem[ (DELIVER_MQ_SIZE + 7) / 8 ];
	
		uint8_t mBuffer[100];
};


#endif
