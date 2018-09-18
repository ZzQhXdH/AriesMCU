#include "Event/MainEvent.h"
#include "Driver/LogUart.h"
#include "Driver/RobotArm2.h"
#include "Driver/RobotArm1.h"
#include "Custom/BaseProtocol.h"
#include "Custom/Protocol.h"
#include "Task/DeliverTask.h"
#include "Custom/OTAManager.h"


void MainEvent::write(uint8_t action, uint8_t arg1)
{
	uint16_t len = Protocol(buffer + 50).setAction(action).append1(arg1).build(buffer);
	MainUart::instance().write(buffer, len);
}

void MainEvent::write(uint8_t action, uint8_t arg1, uint8_t arg2)
{
	uint16_t len = Protocol(buffer + 50).setAction(action).append1(arg1).append1(arg2).build(buffer);
	MainUart::instance().write(buffer, len);
}

void MainEvent::onResult(const uint8_t *buffer, uint16_t length)
{
	MainUart::instance().write(buffer, length);
}

void MainEvent::dispose(void)
{
	uint8_t ret = isCorrectOfResult();
	if (ret != 0) {
		write(0x6F, ret); // 收到的数据不对, 被干扰了
		return;
	}
	write(0x6F, 0); // 收到的数据格式正确
	switch (action())
	{
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
			DeliverTask::instance().put(this);
			break;
		
		case 0x0D:
			onOTANotify(); 
			break;
		
		case 0x0E:
			onPackage();
			break;
		
		case 0x0F:
			DeliverTask::instance().put(this);
			break;
	}
}

bool MainEvent::isCorrectNoCheckLength(void)
{
	if (0xE1 != pMsg[0] || 0xEF != pMsg[nMsgCount - 1]) {
		
		return false;
	}
	
	if (check() != pMsg[nMsgCount - 2]) {
		
		return false;
	}
	
	return true;
}



void MainEvent::onPackage(void)
{
	uint16_t id = arg2(1);
	uint16_t l = size() - 7;
	const uint8_t *p = msg() + 5;
	bool flag = OTAManager::instance().xWrite(id, p, l);
	if (flag)
	{
		uint16_t len = Protocol(buffer + 50).setAction(0x8e).append2(id).append1(0).build(buffer);
		MainUart::instance().write(buffer, len);
	}
	else 
	{
		uint16_t len = Protocol(buffer + 50).setAction(0x8e).append2(id).append1(1).build(buffer);
		MainUart::instance().write(buffer, len);
	}
}

void MainEvent::onOTANotify(void)
{
	uint32_t size = arg4(1);

	OTAManager::instance().init(size);
	uint16_t len = Protocol(buffer + 50).setAction(0x8d).append1(0).build(buffer);
	MainUart::instance().write(buffer, len);
}






