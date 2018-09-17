#ifndef		__LED_H
#define		__LED_H

#include "stm32f4xx.h"

class Led
{
	public:
		static Led& instance(void) {
			static Led l;
			return l;
		}
		
		void blink(void);
	
	private:
		Led(void);
};




#endif
