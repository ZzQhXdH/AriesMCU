#include "Event/Receiver.h"



Receiver::Receiver(const char *name, uint64_t *mq_mem, uint32_t msg_count, uint32_t msg_size)
{
	sMqAttr.name = name;
	sMqAttr.attr_bits = 0;
	sMqAttr.cb_mem = aMqCbMem;
	sMqAttr.cb_size = osRtxMessageQueueCbSize;
	sMqAttr.mq_mem = mq_mem;
	sMqAttr.mq_size = osRtxMessageQueueMemSize(msg_count, msg_size);
	
	sMqId = osMessageQueueNew(msg_count, msg_size, &sMqAttr);
}




