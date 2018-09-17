#ifndef		__RECEIVER_H
#define		__RECEIVER_H

#include <rtx_os.h>
#include "Event/Event.h"

class Receiver
{
	public:
		Receiver(const char *name, uint64_t *mq_mem, uint32_t msg_count, uint32_t msg_size);
		
		inline osStatus_t recv(void *env) {
			return osMessageQueueGet(sMqId, env, NULL, osWaitForever);
		}
	
		inline osStatus_t recv(Event *env) {
			return osMessageQueueGet(sMqId, env, NULL, osWaitForever);
		}
		
		inline osStatus_t put(const Event *env) {
			return osMessageQueuePut(sMqId, env, 0, 0);
		}
		
		inline osStatus_t put(const void *env) {
			return osMessageQueuePut(sMqId, env, 0, 0);
		}
		
		inline osStatus_t recv(Event *env, uint32_t timeOut) {
			return osMessageQueueGet(sMqId, env, NULL, timeOut);
		}
		
	private:
		osMessageQueueId_t sMqId;
		osMessageQueueAttr_t sMqAttr;
		uint64_t aMqCbMem[ (osRtxMessageQueueCbSize + 7) / 8 ];
};



#endif
