#include "lock.h"
extern void LockAlarm(void);
/**
* @author 19/02/21  yang
* @Description: 上电刷新本机id码
* @param -- 客户码
* @param --
* @return --
*/
void CID_init(u32 ClientId,unsigned int* CID)
{
    int rng_num;
    srand(SysTim_GetSys()); //提高随机性
    rng_num = rand()%1001;	//产生随机数，需要上位机配合固定长度4位
    CID[0] = rng_num;
    CID[1] = ClientId;
}
/**
* @author 19/02/21  yang
* @Description: 客户码刷新随机数
* @param --
* @param --
* @return --
*/
void Refresh(u32* refreshid,u32* cid)
{
    int rng_num;
    if ( *refreshid == 1)
    {
        *refreshid = 0;
        srand(SysTim_GetSys());
        rng_num = rand()%1001;
        cid[0] = rng_num;
    }
}

//设置日期
/*
decode 注册码
save 解锁完成的日期存入区(是加密显示的)
cid 客户码
rng 本次注册的随机数
return -1 设置日期失败 0 设置日期成功
*/
s32 unlock(int* decode,lockdataflashsave* save,unsigned int cid,unsigned int rng)
{
    char decodedate[24];
    char Expiration[6];	//到期日
    char Enstring[6];	//加密后字符串
    save->flag = UNLOCK; //将标志写入
	Date temp1,temp2;
    if(Decode(decode,decodedate,cid,rng)==0)
    {
        //将解出的年月日加密保存
        for(int i=0; i<6; i++)
        {
            Expiration[i]= KB2ASC(decodedate[18+i]);
        }
        //对解出的码进行加密
        Encrypt(Expiration,Enstring,6);
		
        //将加密数据保存
		
		temp1.year = ASC2KB(Enstring[0]) *10+ASC2KB( Enstring[1]);
		temp1.month =ASC2KB( Enstring[2])*10+ASC2KB(Enstring[3]);
		temp1.day = ASC2KB(Enstring[4])*10+ASC2KB(Enstring[5]);
		
		DeDate(&temp1,&temp2);
		
		if(temp2.year<20||temp2.year>40||temp2.month>12||temp2.day>31)
		{
			return -1;
		}
		save->lockdata = temp1;
        //存入备份区
        Flash_WriteData(save,sizeof(lockdataflashsave));
        Flash_WriteBkData(save,sizeof(lockdataflashsave));
        return 0;
    }
    return -1;
}
//读取加密信息
void ReadDate(lockdataflashsave* save)
{
    Flash_ReadBkData(save,sizeof(lockdataflashsave));
}

//将加密显示的日期解成正确日期
/*
EnDate 加密的日期
DeDate 解密后的日期
*/
void DeDate(Date* EnDate,Date* DeDate)
{
    char word[6];
    char s[6];
    word[0]=KB2ASC( EnDate->year/10);
    word[1]=KB2ASC(EnDate->year%10);

    word[2] = KB2ASC(EnDate->month/10);
    word[3] = KB2ASC(EnDate->month%10);

    word[4] = KB2ASC(EnDate->day/10);
    word[5] = KB2ASC(EnDate->day%10);

    Decrypt(word,s,6);

    DeDate->year= ASC2KB(s[0])*10+ASC2KB(s[1]);
    DeDate->month = ASC2KB(s[2])*10 + ASC2KB(s[3]);
    DeDate->day = ASC2KB(s[4])*10 + ASC2KB(s[5]);
}

