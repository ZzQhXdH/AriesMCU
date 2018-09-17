#ifndef		__BLUETOOTH_EVENT_H
#define		__BLUETOOTH_EVENT_H

#include "Event/Event.h"
#include <rtx_os.h>


/**
 * @brief
 * 蓝牙数据接收处理
 */
class BluetoothEvent : public Event
{	
	public:
		void dispose(void);
		
	private:
		void onTestRobotArm(void);
	
		void onCheckRobotArm(void);
	
	protected:
		virtual void onReturn(uint8_t status);
	
		virtual void onStatusChange(uint8_t row, uint8_t col, uint8_t status);
	
		virtual void onResult(const uint8_t *buffer, uint16_t length);
	
	private:
		uint8_t buffer[100];
		
};

#endif









