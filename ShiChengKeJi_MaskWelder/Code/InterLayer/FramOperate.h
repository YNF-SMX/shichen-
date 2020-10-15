#ifndef __FRAMOPERATE_H__
#define __FRAMOPERATE_H__

#include "crc16.h"
#include "bsplib.h"

#if 0
#define CRCLEN				4

#define DATA1SAVEADDR		0
#define DATA1SAVELEN		4092

#define DATA2SAVEADDR		DATA1SAVEADDR+DATA1SAVELEN+CRCLEN
#define DATA2SAVELEN		4092

#define ENDADDR				DATA2SAVEADDR+DATA2SAVELEN+CRCLEN		

int HZ_FRAMDATA1_Write(void);
int HZ_FRAMDATA1_Read(void);
int HZ_FRAMDATA2_Write(void);
int HZ_FRAMDATA2_Read(void);
#endif

int HZ_FRAMDATA_Write(void);
int HZ_FRAMDATA_Read(void);

#endif
