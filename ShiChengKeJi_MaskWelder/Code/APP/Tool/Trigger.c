#include "Trigger.h"
//触发器
s32	TrigOne(TrigDef *this, s32 Condition, u32 Time)  //满足条件时 改变TrigDef型变量的TrigFlag值  ，TrigTimeStamp 时获取一个时间值，  TrigRst复位函数获取
{
	if(Condition == 0)
	{
		this->TrigFlag = 1;
		this->TrigTimeStamp = SysTim_GetUser();
	}
	if(Condition != 0 && this->TrigFlag == 1 && (SysTim_GetUser() - this->TrigTimeStamp)/10 > Time)
	{
		this->TrigFlag = 0;
		return 1;
	}
	return 0;
}
//触发器复位
void TrigRst(TrigDef *this)
{
	this->TrigFlag = 0;
	this->TrigTimeStamp = SysTim_GetUser();
}



