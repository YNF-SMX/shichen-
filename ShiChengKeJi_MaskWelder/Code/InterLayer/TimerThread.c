#include "TimerThread.h"
#include "bsplib.h"

void HZ_TimerThread()
{
    u8 i = 0,flg_ms = 0;
    static u32 oldtime = 0;
    if(SysTim_GetSys() - oldtime >= 10)
    {
        oldtime += 10;
        flg_ms = 1;
    }
    if(flg_ms)
    {
        flg_ms = 0;
        for(i=0; i<TIMER_NUM; i++)
        {
            ton_et[i]++;
        }
        for(i=0; i<SYSTIMERNUM; i++)
        {
            if(sysTimer[i])sysTimer[i]--;
        }
        for(i=0; i<USERTIMERNUM; i++)
        {
            if(UserTimer[i])UserTimer[i]--;
        }
        for(i=0; i<TRIGERNUM; i++)
        {
            if(TrigerTimer[i])TrigerTimer[i]--;
        }
    }
}

