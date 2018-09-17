#ifndef		__ROBOT_ARM1_H
#define		__ROBOT_ARM1_H

#include "Event/Sender.h"
#include "stm32f4xx.h"
#include "Custom/PIDControl.h"
#include "Driver/FlashMemory.h"


/**
 * @brief 机械臂电机1和机械臂电机2的速度控制引脚都是TIM3的PWM输出引脚
 * 所以在此类中将会同时初始化这两个电机的速度控制引脚
 */

class RobotArm1 : public Sender
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
		static RobotArm1 &instance(void) {
			static RobotArm1 r;
			return r;
		}
		
		inline void vSetSpeed(uint16_t sp) { TIM3->CCR1 = sp;}
		
		inline int32_t xGetRawPosition(void) { return TIM2->CCR2;}
		
		void vSetDir(Dir dir) ;
		
		uint16_t xGetAbsolutePosition(void);
		
		inline void vCheckPosition(uint16_t p) { 
			nCheckData = p - xGetRawPosition();
			FlashMemory::instance().put( ROBOT_ARM1_CHECK_DATA_INDEX, nCheckData );
		}
		
		
	private:
		RobotArm1(void);
		void initMotoGpio(void);
		void initMotoDriver(void);
		void initCounterGpio(void);
		void initCounterDriver(void);
	
	private:
		int32_t nCheckData;
		PIDControl pid;
};





#endif
