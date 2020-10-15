#include "Uph.h"

s32	UphCal(UphDef *p)
{
	p->SampleIdx++;
	if(p->SampleIdx >= SAMPLENUM)
	{
		p->SampleIdx = 0;
	}
	p->SampleSum = p->SampleSum - p->Sample[p->SampleIdx] + ((SysTim_GetUser()-p->TimeStamp)/10);
	p->Sample[p->SampleIdx] = ((SysTim_GetUser()-p->TimeStamp)/10);
	p->TimeStamp = SysTim_GetUser();
	p->Cycle = p->SampleSum/SAMPLENUM;
	p->Uph = 3600000/p->Cycle;
	p->Upm = 60000/p->Cycle;
	return p->Uph;
}

