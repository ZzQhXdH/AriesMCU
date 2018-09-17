#ifndef		__PID_CONTROL_H
#define		__PID_CONTROL_H

#include <stdint.h>

class PIDControl
{	
	private:
		int32_t nTargetPosition; // 目标位置
		int32_t nSumError; // 积分累加值
		float vPropotrion; // 比例系数
		float vIntegral; // 积分系数
		float vDerivative; // 微分系数
		int32_t nLastError; // 上一次的误差
		int32_t nOutMaxValue; // 输出的最大值
	public:
		void init(int32_t target, float kp, float ki, float kd);
	
		int32_t nextValue(int32_t p);
	
		int32_t nextValueOfFridge(int32_t p);
};


#endif
