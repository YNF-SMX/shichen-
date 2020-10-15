#include "IoUpdata.h"
#include "flashoperate.h"

__weak void ex_inputupdata(void)
{
	;
}
__weak void ex_outputstatusupdata(void)
{
	;
}
__weak void ex_outputupdata(void)
{
	;
}

extern u32* errocode;
extern u32* flahsave;
extern void* globaldata;
extern u32 sysdatalen;

extern u32 userwritelen;
extern u32 userreadlen;
extern u32 usersavelen;

void InputStatusUpdata()
{
    u8 i = 0;
    u32* InputStatus = (u32*)((u16*)globaldata+INPUTSTARTADDR);
    /****主卡输入****/
    InputStatus[0] = 0;
    InputStatus[1] = 0;
    for(i = 0; i < 32; i ++)
        InputStatus[0] |= (u32)	InPut_GetSta(i)	<<	i;
	for(i=0;i<32;i++)
		InputStatus[1] |= (u32)InPut_GetSta(i + 32) << i;
	
	/***主卡轴口输入信号****/
    InputStatus[2] = 0;
    InputStatus[3] = 0;
    for(i = 0; i < 16; i ++)
    {
        InputStatus[2] |= ALM_GetSta(i) << i;
        InputStatus[2] |= LIMIT_GetSta(i)<< (i+16) ;
    }
	
    for(i=0; i<4; i++)	//读AB向输入信号
    {
        InputStatus[3] |= EnCode_GetGpioVal(i,0)<<(i*2) ;
        InputStatus[3] |= EnCode_GetGpioVal(i,1)<<(i*2+1) ;
    }
    ex_inputupdata();
}

void OutputStatusUpdata()
{
    u8 i = 0;
    u32* OutputStatus = (u32*)((u16*)globaldata+OUTPUTSTARTADDR);
    OutputStatus[0] = 0;
	OutputStatus[1] = 0;
    for(i = 0; i < 32; i ++)
        OutputStatus[0] |= (u32)	OutPut_GetSta(i)	<<	i;
	for(i=0;i<32;i++)
		OutputStatus[1] |= (u32)	OutPut_GetSta(i+32)	<<  i;

    OutputStatus[2] = 0;
    for(i = 0; i < 32; i ++)
        OutputStatus[2]	|= (u32)	EN_GetSta(i)	<<	i;

    ex_outputstatusupdata();
}

void OutputUpdata()
{
    u8 i = 0;
    u32* OutputStatus = (u32*)((u16*)globaldata+OUTPUTSTARTADDR);
    for(i = 0; i < 32; i++)
        OutPut_SetSta(i,(OutputStatus[0]>>	i&1));
	for(i=0;i<32;i++)
		OutPut_SetSta(i+32,(OutputStatus[1]>> i&1));
	
    for(i = 0; i < 32; i ++)
        EN_SetSta(i,OutputStatus[2]>>i&1);

    ex_outputupdata();
}


