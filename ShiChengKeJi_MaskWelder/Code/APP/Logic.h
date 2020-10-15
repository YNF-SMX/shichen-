/*
 * @Descripttion: 
 * @version: 
 * @Author: yfs
 * @Date: 2020-02-24 10:31:18
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2020-06-01 14:55:24
 */
#ifndef _LOGIC_H_
#define _LOGIC_H_

#include "DataDefine.h"
#include "bsplib.h"
#include "interlayer_config.h"
#include "FSM.h"
#include "string.h"
#include "logicdata.h"
//#include "iodefine.h"
#include "AxisMove.h"
#include "GpioManager.h"
#include "AxisManager.h"
#include "Timer.h"
#include "Trigger.h"
#include "Uph.h"


typedef struct //系统任务队列
{
	LogicParaDef AxisMoveTask[PULS_NUM];
	
	LogicParaDef	DeviceMainTask;
	LogicParaDef	PickTask;
	LogicParaDef	WeldTask;
	LogicParaDef	PullTask;
	LogicParaDef	BeltSupplyTask[2];
	LogicParaDef	BlowBeltUpTask;
	LogicParaDef	BlowBeltInTask;
	LogicParaDef	AxGohomeReady[PULS_NUM];
	LogicParaDef	UnloadHitTask;
	LogicParaDef	UnloadBeltTask;
} Task;
extern Task LogicTask;

typedef struct //系统程序自用变量
{
  int a;
} SysData;
extern SysData Data;

extern void Logic(void);


/*************Mr.Yiu define 2019.10.29********************/
//#define	my	(*p)
#define	LT	LogicTask

#define	TASK_PROCESS \
	if(my.execute == 1 && my.step == 0)\
	{\
		my.step = 1;\
		my.done = 0;\
	}\
	switch(my.step)
		
#define	FSM_STA	GUR.RunStatus				//状态机状态
#define	FSM_CMD	GUW.Button.RunCommand		//状态机命令
#define	GSPD	GUS.Config.Data.GlobalSpeed	//全局速度简称
#define	GPOS	GUS.Position.Data			//位置参数简称
#define	GDLY	GUS.Delay.Data				//延时参数简称
#define	GCFG	GUS.Config.Data				//配置参数简称
	
extern void AxisDataConfig(void);
/*****************************************************/

#endif
