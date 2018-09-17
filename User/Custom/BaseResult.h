#ifndef		__BASE_RESULT_H
#define		__BASE_RESULT_H

#include "Event/Event.h"

class BaseResult
{
	public:
		BaseResult(const Event *env);
	
	private:
		const Event *env;
};




#endif
