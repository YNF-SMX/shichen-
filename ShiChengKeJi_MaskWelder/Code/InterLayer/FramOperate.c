#include "FramOperate.h"
#include "string.h"

extern void *FramData;
#if 0
/**************************************************************************
* @author: 
* @Date: 2020.03.21
* @Description: 铁电写加CRC校验
* @param: NULL
* @return : 0：写成功	-1：参数错误
***************************************************************************/
int HZ_FRAMDATA_Write(void)
{
	u8 *save;
	save = FramData;
	int crc;
	crc = crc16(save,8188);
	memcpy(&save[8188],&crc,4);
	if(0 == FRam_Write(0,save,8192))
	{
		return 0;
	}
	else
		return -1;
}
/**************************************************************************
* @author: 
* @Date: 2020.03.21
* @Description: 铁电读
* @param: null
* @return : 0：铁电读成功数据校验通过 -1：参数错误铁电未读取 -2：铁电读成功数据校验未通过
***************************************************************************/
int HZ_FRAMDATA_Read(void)
{
	u8 *read;
	read = FramData;
	int crc;
	static int readflag = 0;	//首次读标识
	if(0 == FRam_Read(0,read,8192))
	{
		if(0 == readflag)
		{
			readflag = 1;
			delay_ms(4);	
		}
		crc = crc16(read,8188);
		if(0 != memcmp(&crc,&read[8188],4))
		{
			return -2;
		}
		else 
		{
			return 0;
			
		}
	}
	else
		return -1;
}
#endif

#if 0
int HZ_FRAMDATA1_Write(void)
{
	u8 *save;
	save = FramData;
	int crc;
	crc = crc16(save,DATA1SAVELEN);
	memcpy(&save[DATA1SAVELEN],&crc,2);
	if(0 == FRam_Write(DATA1SAVEADDR,save,DATA1SAVELEN+CRCLEN))
	{
		return 0;
	}
	else
		return -1;
}

int HZ_FRAMDATA2_Write(void)
{
	u8 *save;
	save = FramData;
	int crc;
	crc = crc16(save+0x1000,DATA2SAVELEN);
	memcpy(&save[DATA2SAVELEN+0x1000],&crc,4);
	if(0 == FRam_Write(DATA2SAVEADDR,save+0x1000,DATA2SAVELEN+CRCLEN))
	{
		return 0;
	}
	else
		return -1;
}


int HZ_FRAMDATA1_Read(void)
{
	u8 *read;
	read = FramData;
	u16 crc;
	if( 0 == FRam_Read(DATA1SAVEADDR,read,DATA1SAVELEN+CRCLEN))
	{
		delay_ms(2);
		crc = crc16(read,DATA1SAVELEN);
		if(memcmp(&crc,&read[DATA1SAVELEN],2) == 0)
			return 0;
		else
			return -1;
	}
	else
	{
		return -1;
	}
}

int HZ_FRAMDATA2_Read(void)
{
	u8 *read;
	read = FramData;
	int crc;
	if( 0 == FRam_Read(DATA2SAVEADDR,read+0x1000,DATA2SAVELEN+CRCLEN))
	{
		delay_ms(2);
		crc = crc16(read+0x1000,DATA2SAVELEN);
		if(memcmp(&crc,&read[DATA2SAVELEN+4096],4) == 0)
			return 0;
		else
			return -1;
	}
	else
	{
		return -1;
	}
}
#endif


