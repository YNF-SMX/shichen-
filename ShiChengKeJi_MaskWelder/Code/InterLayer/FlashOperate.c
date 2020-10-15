#include "FlashOperate.h"
#include "string.h"
extern u32* errocode;
extern void* flahsave;
extern void* globaldata;
extern u32 sysdatalen;

extern u32 userwritelen;
extern u32 userreadlen;
extern u32 usersavelen;


FlashOperateParaDef FlashOperatePara = {0};
//数据恢复
//return 0:读取正常校验通过  -1：flash读取错误未读取数据	-2：flash读取成功数据校验未通过
s32 HZ_Data_Read()
{
    u8* check;
    check = globaldata;	

	int crc ;
	
    if(Flash_ReadData(flahsave,usersavelen+SYSSAVELEN+4)==0)
    {
		crc = crc16((u8*)flahsave,usersavelen+SYSSAVELEN);
        if(memcmp(&crc,&check[sysdatalen-4],4)!=0)
        {
            return -2;
        } else {
            return 0;
        }
    } else
    {
        return -1;
    }
}
//读备份域数据
//return 0:读取正常校验通过  -1：flash读取错误未读取数据	-2：flash读取成功数据校验未通过
s32 HZ_BkData_Read()
{
    u8* check;
    check = globaldata;	

	int crc ;
	
    if( Flash_ReadBkData(flahsave,usersavelen+SYSSAVELEN+4)==0)
    {
		crc = crc16((u8*)flahsave,usersavelen+SYSSAVELEN);
        if(memcmp(&crc,&check[sysdatalen-4],4)!=0)
        {
            return -2;
        } else {
            return 0;
        }
    } else
    {
        return -1;
    }
}
//数据写flash备份区
//return 0:写flash成功	-1：写失败
s32 HZ_BkData_Write()
{
    u8* check;
    check = globaldata;
	int crc;
	crc = crc16((u8*)flahsave ,usersavelen+SYSSAVELEN);
	memcpy(&check[sysdatalen-4],&crc,4);
    if(Flash_WriteBkData(flahsave,usersavelen+SYSSAVELEN+4)==0)
        return 0;
    else
        return -1;
}

//数据写flash
//return 0:写flash成功	-1：写失败
s32 HZ_Data_Write()
{
    u8* check;
    check = globaldata;
	int crc;
	crc = crc16((u8*)flahsave ,usersavelen+SYSSAVELEN);
	memcpy(&check[sysdatalen-4],&crc,4);
    if(Flash_WriteData(flahsave,usersavelen+SYSSAVELEN+4)==0)
        return 0;
    else
        return -1;
}

void HZ_FlashOperate()
{
    u16* write;
    write = globaldata;

    switch(write[1100])
    {
    case FLASHREAD:
        HZ_Data_Read();
        write[1100] = 0;
        break;
    case FLASHWRITE:
        HZ_Data_Write();
		HZ_BkData_Write();
        write[1100] = 0;
        break;
    default:
        break;
    }
}


