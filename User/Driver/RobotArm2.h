#ifndef		__ROBOT_ARM2_H
#define		__ROBOT_ARM2_H

#include "Event/Sender.h"
#include "stm32f4xx.h"
#include "Driver/FlashMemory.h"
#include "Custom/PIDControl.h"

class RobotArm2 : public Sender
{
	public:
		enum Dir {
			CW = 0x01,
			CCW = 0x02,
			Brake = 0x03,
		};
		
		enum Error {
			Ok = 0x00,
			TimeOut = 0x01,
		};
	
	public:
		Error xRotationTo(uint16_t position, uint32_t timeOut, uint16_t range = 1);
		void vInversedBrake(void);
	
	public:
		static RobotArm2 &instance(void) {
			static RobotArm2 instance;
			return instance;
		}
	
		inline void vSetSpeed(uint16_t sp) { TIM3->CCR4 = sp;}
		
		inline int32_t xGetRawPosition(void) { return TIM5->CCR2;}
		
		void vSetDir(Dir dir);
		
		uint16_t xGetAbsolutePosition(void);
		
		inline void vCheckPosition(uint16_t p) { 
			nCheckData = p - xGetRawPosition();
			FlashMemory::instance().put( ROBOT_ARM2_CHECK_DATA_INDEX, nCheckData );
		}
		
	private:
		RobotArm2(void);
		void initMotoGpio(void);
		void initMotoDriver(void);
		void initCounterGpio(void);
		void initCounterDriver(void);
	
	private:
		int32_t nCheckData;
		PIDControl pid;
};




#endif
