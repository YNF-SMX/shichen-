#ifndef _AXISMANAGER_H_
#define	_AXISMANAGER_H_

	#include "AxisMove.h"
	#include "usertool.h"

	typedef enum
	{
		AxFlow,		//传送轴0
		AxPickZ,	//上下轴1
		AxPull,		//拉线轴2
		AxPickLF,	//左前  3
		AxPickRF,	//右前  4
		AxPickLB,	//左后  5
		AxPickRB,	//右后  6
		AxWeld,		//焊接轴7
		AxHit,		//打料轴 8
		AxBelt_1,	//放带轴1 9
		AxBelt_2,	//放带轴2  10
	}AxName;
	
	
	extern s32 AxAbsUnit(u32 Num, float SpdRatio, float Pos);
	extern s32 AxRelUnit(u32 Num, float SpdRatio, float Pos);
	extern s32 AxGohome(u32 Num);
	extern s32 AxStop(u32 Num);
	extern s32 AxSta(u32 Num);
	extern s32	AxInpSta(u32 Num);

#endif
