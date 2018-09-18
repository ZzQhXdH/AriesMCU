#ifndef		__E_LOCK_H
#define		__E_LOCK_H

#include "stm32f4xx.h"
#include <rtx_os.h>

namespace ELock {
	
	void vInit(void);
	
	inline void vOff(void)
	{
		GPIOD->BSRRL = 0x08;
	}
	
	inline void vOn(void)
	{
		GPIOD->BSRRH = 0x08;
		osDelay(1000);
		vOff();
	}
}






#endif
