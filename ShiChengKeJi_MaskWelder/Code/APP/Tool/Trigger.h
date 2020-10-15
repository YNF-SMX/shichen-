/**************************************
文件名称：触发器
作者	：
时间	：
功能描述	：
使用方法	：
	1、在函数中 static TrigDef	xxx
	
	2、在流程中	
		case 1:
			TrigRst(&xxx);
			break;
			
		case 2:
			if(TrigOne(&xxx, 触发条件, 保持时间))
			{
					干活语句
			｝
			break;
**************************************/
#ifndef	_TRIGGER_H_
#define	_TRIGGER_H_

	#include "interlayer_config.h"
	
	//一个触发器
	typedef struct
		{
		u32	TrigFlag;
		u32 TrigTimeStamp;
	}TrigDef;
	
					//定义的触发器         触发条件     保持时间
	extern s32	TrigOne(TrigDef *this, s32 Condition, u32 Time);
	extern void TrigRst(TrigDef *this);
	
#endif

