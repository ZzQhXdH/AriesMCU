#ifndef		__SYS_TIMER_H
#define		__SYS_TIMER_H

#include "stm32f4xx.h"

namespace SysTimer {
	
	void vInit(void);
	
	void vDelayUs(uint16_t ms);
}





#endif
