#include "bsplib.h"
#include "GpioManager.h"
#include "can_master.h"

GpioSta I_Get(Gpio idx)
{
	if((int)idx < 1000)
	{
		return (GpioSta)InPut_GetSta(idx);
	}
	else
	{
		return (GpioSta)HZ_ExInPutGet(idx/1000-1,idx%1000);
	}
}

void Q_Set(Gpio idx, GpioSta val)
{
	if((int)idx < 1000)
	{
		OutPut_SetSta(idx,val);
	}
	else
	{
		HZ_ExOutPutSet(idx/1000-1,idx%1000,val);
	}
}

GpioSta Q_Get(Gpio idx)
{
	if((int)idx < 1000)
	{
		return (GpioSta)OutPut_GetSta(idx);
	}
	else
	{
		return (GpioSta)HZ_ExOutPutGet(idx/1000-1,idx%1000);
	}
}

