#ifndef _JOGOPERATE_H_
#define	_JOGOPERATE_H_

#include "flashoperate.h"
#include "interlayer_config.h"
#include "can_master.h"

#define	_FORWARD		1
#define	_BACKWARD	2

#define	JOGLEVEL	0
#define	JOGPULSE	1
#define	JOGSPEED	2

#define MAXAXISNUM 50
typedef struct
{
    u8	step;
    u8	motion;
} JogOperateParaDef;

//extern void HZ_JogOperate(int M_Axnum);

#endif

