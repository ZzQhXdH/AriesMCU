#ifndef		__PICK_MOTOR_H
#define		__PICK_MOTOR_H

#include "Event/Sender.h"
#include "stm32f4xx.h"
#include "Driver/PositionSwitch.h"
#include "Driver/Compressor.h"


class PickMotor : public Sender
{
	public:
		enum Dir {
			Up = 0x01,
			Down = 0x02,
			Brake = 0x03,
			Unknow = 0x04,
		};
		
		enum Error {
			Ok = 0x00,
			TimeOut = 0x01,
		};
	
	public:
		Error xGoUp(uint16_t sp = 35, uint32_t steps = 0, uint32_t timeOut = 10 * 1000);
		Error xGoDown(uint16_t sp, uint32_t position, uint32_t timeOut);
		Error xGoDownOfAbsolute(uint16_t sp, uint32_t position, uint32_t timeOut);
		Error xGoDownAndCheckCompressor(uint16_t sp, uint32_t position, uint32_t timeOut);
		
	public:
		static PickMotor &instance(void) 
		{
			static PickMotor pickMotor;
			return pickMotor;
		}
	
		inline void vResetCounter(void) { TIM1->CNT = 0;}
		
		inline uint16_t xGetCounter(void) { return TIM1->CNT;}
		
		inline void vSetSpeed(uint16_t sp) { TIM11->CCR1 = sp;}
		
		void vSetDir(Dir dir);
		
		inline bool isTop(void) { return PositionSwitch::instance().isPickMotorChecked(); }
		
		inline bool isChecked(void) { return Compressor::instance().isChecked();}
		
	private:
		PickMotor(void);
		void initMotoGpio(void);
		void initMotoDriver(void);
		void initCounterGpio(void);
		void initCounterDriver(void);
};



#endif
