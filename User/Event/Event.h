#ifndef		__EVENT_H
#define		__EVENT_H

#include <rtx_os.h>

class Event
{
	public:
		Event(void) : pMsg(NULL), nMsgCount(0) {}
		Event(const uint8_t *msg, uint32_t count) : pMsg(msg), nMsgCount(count) {}
	
		inline void set(const uint8_t *msg, uint32_t count) {
			pMsg = msg;
			nMsgCount = count;
		}
		
		inline const uint8_t *msg(void) const { return pMsg;}
		
		inline uint8_t size(void) const { return nMsgCount;}
		
		inline uint8_t argCount(void) const { return nMsgCount - 5;}
		
		inline uint8_t arg(uint8_t index) const { return pMsg[index + 2];}
		
		inline uint16_t arg2(uint8_t index) const { return (pMsg[index + 2] << 7) + pMsg[index + 3]; }
		
		inline uint32_t arg3(uint8_t index) const { return (pMsg[index + 2] << 14) + (pMsg[index + 3] << 7) + pMsg[index + 4]; }
		
		inline uint32_t arg4(uint8_t index) const { return (pMsg[index + 2] << 21) + (pMsg[index + 3] << 14) + (pMsg[index + 4] << 7) + pMsg[index + 5]; }
			
		inline uint8_t action(void) const { return pMsg[2]; }
		
		/**
		 * 判断接收的数据是否正确
		 */
		bool isCorrect(void) const;
		
		uint8_t isCorrectOfResult(void) const;
		
	public:
		virtual void onReturn(uint8_t status) ;
		// Only Deliver
		virtual void onStatusChange(uint8_t row, uint8_t col, uint8_t status);
	
		virtual void onResult(const uint8_t *buffer, uint16_t length);
		
	protected:
		const uint8_t *pMsg;
		uint32_t nMsgCount;
		uint8_t check(void) const;
};


#endif
