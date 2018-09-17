#ifndef		__EVENT_SENDER_H
#define		__EVENT_SENDER_H

#include <rtx_os.h>
#include "Event/Event.h"


class Sender
{
	public:
		Sender(const char *name);
		
		inline osStatus_t acquire(void) {
			return osMutexAcquire(sMutexId, osWaitForever);
		}
		
		inline osStatus_t release(void) {
			return osMutexRelease(sMutexId);
		}
		
	private:
		osMutexId_t sMutexId;
		osMutexAttr_t sMutexAttr;
		uint64_t aMutextCbMem[ (osRtxMutexCbSize + 7) / 8 ];
};



#endif
