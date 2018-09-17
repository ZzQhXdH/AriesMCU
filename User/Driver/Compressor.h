#ifndef		__COMPRESSOR_H
#define		__COMPRESSOR_H

#include "stm32f4xx.h"
#include "Driver/PositionSwitch.h"

class Compressor 
{
	public:
		enum Error {
			Ok = 0,
			TimeOut = 0x01,
		};
	
	public:
		static Compressor &instance(void) {
			static Compressor com;
			return com;
		}
		
		inline void on(void) { GPIOD->BSRRH = 0x10;}
		
		inline void off(void) { GPIOD->BSRRL = 0x10;}
		
		inline bool isChecked(void) { return (0 == (PositionSwitch::instance().xGetValue() & PositionSwitch::KYJ_SW6));}
		
		Error xWaitDropOut(const uint16_t timeOut);
		
	private:
		Compressor(void);
};




#endif
