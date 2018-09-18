#include "Event/LogEvent.h"
#include "Driver/LogUart.h"
#include "Driver/RobotArm2.h"
#include "Driver/RobotArm1.h"
#include "Custom/BaseProtocol.h"
#include "Custom/Protocol.h"
#include "Task/DeliverTask.h"

void LogEvent::dispose(void)
{
	if (!isCorrect()) {
		
		return;
	}
	BaseProtocol(buffer, *this, 0).write();
	
	switch (action())
	{
		case 0x01:
			onCheckRobotArm();
			break;
		
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x0F:
			DeliverTask::instance().put(this);
			break;
	}
	
}

void LogEvent::onCheckRobotArm(void)
{
	uint16_t p1 = (arg(1) << 7) + arg(2);
	uint16_t p2 = (arg(3) << 7) + arg(4);
	RobotArm1::instance().vCheckPosition(p1);
	RobotArm2::instance().vCheckPosition(p2);
	FlashMemory::instance().save();
	
	BaseProtocol(buffer, *this, 1).write();
}

void LogEvent::onReturn(uint8_t status)
{
	BaseProtocol(buffer, *this, status).writeOfLog();
}

void LogEvent::onStatusChange(uint8_t row, uint8_t col, uint8_t status)
{
	uint16_t len = Protocol(buffer)
					.setAction(0x8F)
					.append1(row)
					.append1(col)
					.append1(status)
					.build(buffer + 50);
	LogUart::instance().write(buffer + 50, len);
}




