#include "Tools.h"
u32	sysTimer[SYSTIMERNUM] = {0};
u32	UserTimer[USERTIMERNUM] = {0};

unsigned char  flagbuf[TIMER_NUM];
//上升缘检测，clk由0变为1时，该函数返回值为1，当下次再执行，返回值为0; 形参 num：上升沿检测编号
unsigned char r_trig(char num,unsigned char clk)
{
    if(num>=(char)TIMER_NUM)
        num = TIMER_NUM - 1;
    if((char)clk!=(flagbuf[num]))
    {
        flagbuf[num] = clk;
        if (clk)
            return 1;
    }
    return 0;
}

unsigned int  ton_et[TIMER_NUM];
unsigned char ton(unsigned int num,unsigned char ex,unsigned int pt)
{
    if(ton_et[num] >= pt && ex > 0 )
    {
        ton_et[num] = pt;
        return 1;
    }

    if(ex == 0)
        ton_et[num] = 0;

    return 0;
}

u8	TrigerFlag[TRIGERNUM] = {0};
u32	TrigerTimer[TRIGERNUM] = {0};
u8 Triger(u8 Num, s32 TrigValue, s32 TrigerInput ,u32 ContTime)
{
    if(Num<TRIGERNUM)
    {
        if(TrigerInput != TrigValue)
        {
            TrigerTimer[Num] = ContTime;
        }
        if(TrigerInput != TrigValue || TrigerFlag[Num])
        {
            TrigerFlag[Num] = 1;
            if(TrigerInput == TrigValue && TrigerTimer[Num] == 0)
            {
                TrigerFlag[Num] = 0;
                return 1;
            }
        }
    }
    return 0;
}




