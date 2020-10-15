/**************************************
文件名称：定时器
作者	：
时间	：
功能描述	：
使用方法	：
1、	在函数中  static u32 xxx
2、	在流程中使用
	case 1:
		TimerRst(&xxx);
		break;
		
	case 2:
		if(TimerCnt(&xxx, 使能条件) > 目标时间)
		{
			干活语句
		}
		break;
		
当当当当：使能 条件不满足时，会重新计时
**************************************/


#ifndef	_TIMER_H_
#define	_TIMER_H_

	#include "interlayer_config.h"
	
	extern u32 TimerCnt(u32 *tc, s32 en);
	extern void TimerRst(u32 *tc);

#endif
