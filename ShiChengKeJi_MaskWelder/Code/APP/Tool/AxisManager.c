#include "AxisManager.h"
s32 InpVal[8] = {0,50,0,10,10,10,10,0};
s32 RecordPos[8];
s32	AxAbsUnit(u32 Num, float SpdRatio, float Pos)
{
    if(Num>PULS_NUM)
        return -1;
    SetAxisSpdRadio(Num, SpdRatio);	//设置速度
	RecordPos[Num] = Pos;
	return HZ_AxMoveAbs(Num,ConverToPulse(Num, Pos));
}

s32	AxRelUnit(u32 Num, float SpdRatio, float Pos)
{
    if(Num>PULS_NUM)
        return -1;
    SetAxisSpdRadio(Num, SpdRatio);	//设置速度
	RecordPos[Num] = GSR.AxisPosition[Num]+Pos;
	return HZ_AxMoveRel(Num,ConverToPulse(Num, Pos));
}

s32	AxGohome(u32 Num)
{
    if(Num>PULS_NUM)
        return -1;
    SetAxisSpdRadio(Num, 100);	//设置速度
	return HZ_AxHome(Num);
}

s32 AxStop(u32 Num)
{
    if(Num>PULS_NUM)
        return -1;
    SetAxisSpdRadio(Num, 100);	//设置速度
	return HZ_AxStop(Num);
}

s32	AxSta(u32 Num)
{
//	if(HZ_AxGetStatus(Num) == AXSTA_READY && LIMIT_GetSta(Num) == ON)
//	{
//		return 0;
//	}
//	else
//	{
//		return 1;
//	}
	return HZ_AxGetStatus(Num);
}	

//轴提前到位
s32	AxInpSta(u32 Num)
{
	if(HZ_AxGetStatus(Num) == AXSTA_READY || abs(RecordPos[Num] - GSR.AxisPosition[Num]) < InpVal[Num])
	{
		return 0;
	}
	return 1;
}

