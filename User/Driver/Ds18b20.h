#ifndef		__DS18B20_H
#define		__DS18B20_H

#include "stm32f4xx.h"

class Ds18b20 {
	
	public:
		static Ds18b20 &instance(void) {
			static Ds18b20 device;
			return device;
		} 
		
		uint32_t xGetValue(void);
		
	private:
		Ds18b20(void);
		
		void vReset(void);
	
		bool xCheckDevice(void);
	
		bool xReadBit(void);
	
		uint8_t xReadByte(void);
	
		void vWriteByte(uint8_t byte);
	
		void vStart(void);
};







#endif
