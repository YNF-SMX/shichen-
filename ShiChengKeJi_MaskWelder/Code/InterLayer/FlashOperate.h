#ifndef _FLASHOPERATE_H_
#define	_FLASHOPERATE_H_

#include "crc16.h"
#include "Tools.h"
#include "Alarm.h"
#include "bsplib.h"

typedef struct
{
    u8	step;
    u8	temp;
} FlashOperateParaDef;

#define	SYSREADLEN		1000
#define	SYSWRITELEN		1000
#define	SYSSAVELEN		4000

#define	INPUTSTARTADDR	310
#define	INPUTENDADDR	389
#define	OUTPUTSTARTADDR	1020
#define	OUTPUTENDADDR	1099

#define	FLASHREAD	1
#define	FLASHWRITE	2
#define	FLASHCLEAR	3

extern s32 HZ_Data_Read(void);
extern s32 HZ_BkData_Read(void);
extern s32 HZ_Data_Write(void);
extern s32 HZ_BkData_Write(void);

#endif


