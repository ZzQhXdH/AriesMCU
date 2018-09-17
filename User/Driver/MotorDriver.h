#ifndef		__MOTOR_DRIVER_H
#define		__MOTOR_DRIVER_H

#include "stm32f4xx.h"
#include <rtx_os.h>

class MotorDriver
{
	public:
		static MotorDriver &instance(void)
		{
			static MotorDriver driver;
			return driver;
		}
		
		void enable(void);
		
		void disable(void);
		
		void clear(void);
		
	private:
		MotorDriver(void);
};






#endif
