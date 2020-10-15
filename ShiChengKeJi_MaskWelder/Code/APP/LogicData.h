/*
 * @Author: your name
 * @Date: 2020-05-26 14:03:11
 * @LastEditTime: 2020-05-29 15:41:48
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \YD_KZJ\Code\APP\LogicData.h
 */ 
#ifndef __LOGICDATA_H
#define __LOGICDATA_H
#include "bsplib.h"

typedef struct
{
    u8 execute;
    u8 step;
    u8 done;
	u8 Taskflag;//使用合乎情理的命名
    u32 count;	//用于记录圈数，必须放大单位
    u32 tm;
} LogicParaDef;

#define PARAINIT(a)  memset(&a,0,sizeof(a));
#define INITTASK(a) if(a.execute==1&&a.step==0){a.step = 1;}


#define TRST(T) T->tm = SysTim_GetUser()
#define TCNT(T) ((SysTim_GetUser() - (T->tm)) / 10)
#define INITT(a)                      \
	if (a->execute == 1 && a->step == 0) \
	{                                    \
		a->step = 1;                      \
		a->done = 0;						\
	}
#define STEP(a) a->step 


#endif
