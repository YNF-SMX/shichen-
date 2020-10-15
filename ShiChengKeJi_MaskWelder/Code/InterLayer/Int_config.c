#include "flashoperate.h"
#include "framoperate.h"
extern void AlarmSetBit(u8 level, u8 word, u8 bit);
u32* errocode;
u32* flahsave;
void* globaldata;
u32 sysdatalen;

u32 userwritelen;
u32 userreadlen;
u32 usersavelen;

void *FramData;

void init_config(void* GlobalData,u32* err,void* flah,u32 datalen,u32 writelen,u32 readlen,u32 savelen)
{
    globaldata = GlobalData;
    errocode = err;
    flahsave = flah;
    sysdatalen = datalen;
    userwritelen = writelen;
    userreadlen = readlen;
    usersavelen = savelen;
	
}

void init_config_fram(void *framdata)
{
	FramData = framdata;
}


