#include "Alarm.h"

extern u32* errocode;
extern void* flahsave;
extern void* globaldata;
extern u32 sysdatalen;

extern u32 userwritelen;
extern u32 userreadlen;
extern u32 usersavelen;

void AlarmSetBit(u8 level, u8 word, u8 bit)
{
    if(level>=1 && level<=5 && word<=3 && bit<=31)
    {
        errocode[level*4-4+word] |= 1<<bit;
    }
}

void AlarmClearBit(u8 level, u8 word, u8 bit)
{
    if(level>=1 && level<=5 && word<=3 && bit<=31)
    {
        errocode[level*4-4+word] &= ~(1<<bit);
    }
}
//level 
s32 AlarmGetBit(u8 level, u8 word, u8 bit)
{
	if(level>=1 && level<=5 && word<=3 && bit<=31)
	{
		return errocode[level*4-4+word] & (1<<bit);
	}
	return -1;
}

void AlarmClear()
{
    u8 i;
    u16* clear;
    clear = globaldata;
    if(clear[1130]==1)
    {
        clear[1130] = 0;
        for(i=0; i<20; i++)
        {
            errocode[i] = 0;
        }
        errocode[20] = 0;
    }
}

void AlarmCatch()
{
    int i;
    //errocode[20] = 0;
    for(i=19; i >= 0; i--)
    {
        if(errocode[i])
        {
            errocode[20] = i/4 + 1;
            break;
        }
    }
}

void HZ_Alarm()
{
    AlarmClear();
    AlarmCatch();
}

