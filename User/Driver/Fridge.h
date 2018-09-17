#ifndef		__REF_MOTO_H
#define		__REF_MOTO_H

#include "stm32f4xx.h"
#include "Event/Sender.h"
#include "Driver/PositionSwitch.h"
#include "Custom/PIDControl.h"

/**
 * @brief 冰箱门管理类
 */
class Fridge : public Sender
{
	public:
		enum Dir {
			Open = 0x01,
			Close = 0x02,
			Brake = 0x03,
			Unknow = 0x04,
		};
		
		enum Error {
			Ok = 0x00,
			TimeOut = 0x01,
		};
	
	public:
		Error xOpen(uint16_t sp, uint32_t timeOut);
		Error xClose(uint16_t sp = 20, uint32_t timeOut = 10 * 1000);
	
	public:
		static Fridge &instance(void) {
			static Fridge fridge;
			return fridge;
		}
		
		/**
		 * @brief 复位冰箱门电机的编码器计数器
		 */
		inline void vResetCounter(void) { TIM4->CNT = 0;}
		
		/**
		 * @brief 获取冰箱门电机编码器的计数值
		 */
		inline uint16_t xGetCounter(void) { return TIM4->CNT;}
		
		/**
		 * @brief 设置冰箱门电机的转速 sp 转速百分比
		 */
		inline void vSetSpeed(uint16_t sp) { TIM9->CCR2 = sp;}
		
		inline bool isOpen(void) { return PositionSwitch::instance().isFridgeOpenChecked(); }
		
		inline bool isClose(void) { return PositionSwitch::instance().isFridgeCloseChecked(); }
		
		void vSetDir(Dir dir);
		
	private:
		Fridge(void);
		void initMotoGpio(void);
		void initMotoDriver(void);
		void initCounterGpio(void);
		void initCounterDriver(void);
	
	private:
		PIDControl pid;
	
};

#endif
