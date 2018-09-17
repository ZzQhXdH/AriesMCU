#include "Task/StatusUpdateTask.h"
#include "Driver/RobotArm1.h"
#include "Driver/RobotArm2.h"
#include "Custom/Protocol.h"
#include "Driver/Ds18b20.h"
#include "Driver/PositionSwitch.h"
#include "Driver/Bluetooth.h"
#include "Driver/MainUart.h"
#include "Driver/MotorDriver.h"
#include "Driver/LogUart.h"



StatusUpdateTask::StatusUpdateTask(void) : Task("Status", mStack, sizeof (mStack))
{}

void StatusUpdateTask::run(void)
{
	uint16_t robot1Value, robot2Value;
	uint32_t temperature;
	uint8_t state;
	bool isUpdate = false;
	
	while (true)
	{
		temperature = Ds18b20::instance().xGetValue();
		
		for (uint32_t i = 0; i < 10; i ++)
		{
			robot1Value = RobotArm1::instance().xGetAbsolutePosition();
			robot2Value = RobotArm2::instance().xGetAbsolutePosition();
			state = PositionSwitch::instance().xGetValue();	
			
			if (state & PositionSwitch::DOOR_SW5) { 
				Bluetooth::instance().disable(); 
			} else {
				Bluetooth::instance().enable();
			}
			
			isUpdate = false;
			
			if (state != nState) { nState = state; isUpdate = true; }
			
			if (isUpdate)
			{
				MainUart::instance().setAction(0x88)
				.append2(temperature)
				.append2(robot1Value)
				.append2(robot2Value)
				.append1(state)
				.buildAndWrite();
			}
			
			if (robot1Value != nPosition1) { nPosition1 = robot1Value; isUpdate = true; }
			if (robot2Value != nPosition2) { nPosition2 = robot2Value; isUpdate = true; }
			if (temperature != nTemperature) { nTemperature = temperature; isUpdate = true; }
			
			if (Bluetooth::instance().isEnable() && isUpdate) 
			{ 
				Bluetooth::instance().setAction(0x88)
					.append2(temperature)
					.append2(robot1Value)
					.append2(robot2Value)
					.append1(state)
					.buildAndWrite();
			}
		
			osDelay(500);
		}
		
		MainUart::instance().setAction(0x88)
				.append2(temperature)
				.append2(robot1Value)
				.append2(robot2Value)
				.append1(state)
				.buildAndWrite();
		
		if (Bluetooth::instance().isEnable())
		{
			Bluetooth::instance().setAction(0x88)
				.append2(temperature)
				.append2(robot1Value)
				.append2(robot2Value)
				.append1(state)
				.buildAndWrite();
		}
	}
}






