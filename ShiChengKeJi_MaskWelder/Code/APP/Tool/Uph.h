#ifndef	_UPH_H_
#define	_UPH_H_


#include "DataDefine.h"

#define	SAMPLENUM	10

typedef struct
{
	s32	Cycle;					
	s32	Uph;
	s32	Upm;
	s32	Sample[SAMPLENUM];
	s32	SampleSum;
	s32	SampleIdx;
	u32	TimeStamp;
}UphDef;

extern s32	UphCal(UphDef *p);

#endif
