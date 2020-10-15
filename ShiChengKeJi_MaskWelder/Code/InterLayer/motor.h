/*
 * @Descripttion: 
 * @version: 
 * @Author: yfs
 * @Date: 2020-02-04 14:05:46
 * @LastEditors  : yfs
 * @LastEditTime : 2020-02-04 16:15:39
 */
#ifndef __MOTOR_H
#define __MOTOR_H

#include "bsplib.h"

typedef enum
{
    INIT,
    NORSTOP,  //正常静止态
    PAUSE,    //暂停态
    UNIRUN,   //单轴运动
    MULTIRUN, //多轴同步态
    ERRSTOP,  //错误
} MOTORSTATE;

//轴参数
typedef struct axispara
{
    //速度曲线
    u32 start;
    u32 acc;
    u32 runspd;
    u32 dec;
    u32 end;
    //限位 使能判断
    u32 AlmLev;    //报警信号电平，0,1,其他无效
    u32 HlimitEN_Up;   //
    u32 HlimitEN_Down; //
    u32 SlimitEN_Up;
    u32 SlimitEN_Down;
    //限位值
    u32 HlimitNum;
    u32 Hlimitlev; 
    //原点
}Paratype;

//typedef struct axiscmd
//{
//    //当前运动指令类型
//    //当前指令目标位置
//    //当前指令状态
//}CMDtype;


typedef struct motor
{
    MOTORSTATE gstate;  //轴的状态
    Paratype    para;   //轴的参数

} motortype;

#endif
