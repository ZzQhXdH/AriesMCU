#include "Custom/PIDControl.h"

int32_t PIDControl::nextValueOfFridge(int32_t p)
{
	int32_t error = nTargetPosition - p;
	int32_t dError = error - nLastError;
	
	if ( (dError < 5) && (dError > -5) ) {
		nSumError += error;
	}
	nLastError = error;
	return (int32_t) (vPropotrion * error 
			+ vIntegral * nSumError);
}

int32_t PIDControl::nextValue(int32_t p)
{
	int32_t error = nTargetPosition - p;
	
	int32_t dError = error - nLastError;
	
	if ( (error < 20) && (error > -20) ) {
		nSumError += error;
	}
	
	nLastError = error;
	
	return  (int32_t) (vPropotrion * error 
			+ vIntegral * nSumError
			+ vDerivative * dError);
}

void PIDControl::init(int32_t target, float kp, float ki, float kd)
{
	nTargetPosition = target;
	nSumError = 0;
	vPropotrion = kp;
	vIntegral = ki;
	vDerivative = kd;
	nLastError = 0;
	nOutMaxValue = 0;
}



