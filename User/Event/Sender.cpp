#include "Event/Sender.h"


Sender::Sender(const char *name)
{
	sMutexAttr.name = name;
	sMutexAttr.attr_bits = osMutexRobust;
	sMutexAttr.cb_mem = aMutextCbMem;
	sMutexAttr.cb_size = osRtxMutexCbSize;
	
	sMutexId = osMutexNew(&sMutexAttr);
}



















