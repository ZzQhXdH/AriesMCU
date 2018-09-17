#ifndef		__POSITION_SWITCH_H
#define		__POSITION_SWITCH_H

#include "stm32f4xx.h"
#include "Event/Sender.h"

class PositionSwitch : public Sender
{
	public:
		enum Mask {
			ICE_DOOR_SW1 = 0x02, // 冰箱门打开微动开关 低电平触发
			ICE_DOOR_SW2 = 0x04, // 冰箱门关闭微动开关 低电平触发
			CAT_SW3 = 0x04, 
			CAT_SW4 = 0x08, 
			CAT_SW = 0x10, // 商品抓取限位开关 低电平触发
			DOOR_SW5 = 0x20, // 门禁开关 高电平触发
			KYJ_SW6 = 0x40, // 空压机压力开关 低电平触发
			YL1 = 0x40,
			YL2 = 0x80,
		};
	
	public:
		static PositionSwitch &instance(void) 
		{
			static PositionSwitch instance;
			return instance;
		}
		
		uint8_t xGetValue(void);
		
		inline bool isFridgeOpenChecked(void) { return 0x00 == (xGetValue() & ICE_DOOR_SW1); }
		
		inline bool isFridgeCloseChecked(void) { return 0x00 == (xGetValue() & ICE_DOOR_SW2); }
		
		inline bool isPickMotorChecked(void) { return 0x00 == (xGetValue() & CAT_SW); }
		
		inline bool isDoorChecked(void) { return 0x00 != (xGetValue() & DOOR_SW5); }
		
		inline bool isCompressored(void) { return 0x00 == (xGetValue() & KYJ_SW6); }
		
	private:
		PositionSwitch(void);
};





#endif
