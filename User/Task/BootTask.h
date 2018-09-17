#ifndef		__BOOT_TASK_H
#define		__BOOT_TASK_H

#include "Driver/Bluetooth.h"
#include "Driver/Compressor.h"
#include "Driver/Ds18b20.h"
#include "Driver/FlashMemory.h"
#include "Driver/Fridge.h"
#include "Driver/LogUart.h"
#include "Driver/MainUart.h"
#include "Driver/PositionSwitch.h"
#include "Driver/RobotArm1.h"
#include "Driver/RobotArm2.h"
#include "Driver/SysTimer.h"
#include "Driver/PickMotor.h"

#include "Task/Task.h"


class BootTask : public Task
{
	public:
		static BootTask &instance(void) {
			static BootTask task;
			return task;
		}
	
	protected:
		virtual void run(void);
		
	private:
		BootTask(void);
	
	private:
		uint64_t mStack[512 / 8];
};



#endif
