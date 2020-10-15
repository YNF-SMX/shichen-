#include "encryption.h"

char key[]= "sdfujujhgggvh";	//用于加密的随机字符串，和上位机保持一致

/**
* @author 19/02/21  yang
* @Description: 解码
* @param --
* @param --
* @return --
*/
char DecryptChar(char a, char b)
{
    int c = a - b + 'a';
    if (a >= '0' && a <= '9') //字符0-9的转换
    {
        while (c < '0') c += 10;
    }
    else if (a >= 'a' && a <= 'z') //字符a-z的转换
    {
        while (c < 'a') c += 26;
    }
    else if (a >= 'A' && a <= 'Z') //字符A-Z的转换
    {
        while (c < 'A') c += 26;
    }
    else return a; //不再上面的范围内，不转换直接返回
    return (char)c; //返回转换后的字符
}
/**
* @author  yang 2019/ 9/26 yang
* @Description: 加密
* @param --
* @param --
* @return --
*/
char EncryptChar(char a, char b)
{
    int c = a + b - 'a';
    if (a >= '0' && a <= '9') //字符0-9的转换
    {
        while (c > '9') c -= 10;
    }
    else if (a >= 'a' && a <= 'z') //字符a-z的转换
    {
        while (c > 'z') c -= 26;
    }
    else if (a >= 'A' && a <= 'Z') //字符A-Z的转换
    {
        while (c > 'Z') c -= 26;
    }
    else return a; //不再上面的范围内，不转换直接返回
    return (char)c; //返回转换后的字符
}

/**
* @author  yang 2019/2/26 yang
* @Description: 解密函数，按照指定规则解码
* @param --decode 需要解码的字 decodeData 解码后的结果字符串,cid 厂家的id编码
* @return -- 0 解码成功，-1 失败
*/
int Decode(int* decode,char* decodeData,unsigned int cid,unsigned int rng)
{
    char unlock[24];	
    //接收字大于6位，无效解锁码
    if( *(decode)/1000000 != 0 ||*(decode+1)/1000000!=0 || *(decode+2)/1000000!=0||*(decode+3)/1000000!=0 )
    {
        return -1;
    }

    for(int i = 0; i<4; i++)	//将收到的解锁码转成字符串 4*6 个字符长度 将24个字符放入
    {
        unlock[0+i*6] = KB2ASC(*(decode+i)/100000);
        unlock[1+i*6] = KB2ASC(*(decode+i)/10000%10);
        unlock[2+i*6] = KB2ASC(*(decode+i)/1000%10);
        unlock[3+i*6] = KB2ASC(*(decode+i)/100%10);
        unlock[4+i*6] = KB2ASC(*(decode+i)/10%10);
        unlock[5+i*6] = KB2ASC(*(decode+i)%10);
    }

    for (int i = 0; i < 24; i++) //对24个字符进行解码操作
    {
        char a = unlock[i];
        char b = key[i % 13];
        decodeData[i] = ASC2KB(DecryptChar(a, b));
    }

    //判断客户码是否与本程序吻合 1234 [0][1][2][3]
    if(decodeData[0]== (cid/1000)&&
            decodeData[1]== (cid/100%10)&&
            decodeData[2]==(cid/10%10)&&
            decodeData[3]==(cid%10)&&
			decodeData[4] == rng/100 &&
			decodeData[5] == rng/10%10 &&
			decodeData[6] == rng%10 
		)
    {
        return 0;	//解码成功
    }
    return -1;
}
/**
* @author  yang 2019/2/27
* @Description: 加密字符串 
* @param -- word 需要加密的字符串  s 加密之后的字符串 字符串长度
* @return -- 0 加密成功 -1 解密成功
*/
int Encrypt( char* word, char* s,int len)
{
	if(len<=0)
	{
		return -1;
	}
    //逐个字符加密字符串
    for (int i = 0; i < len; i++)
    {
        char a = word[i];
        char b = key[i % 13];
        s[i] = EncryptChar(a, b);
    }
    return 0;
}
/**
* @author  yang 2019/2/27
* @Description: 解密字符串 
* @param -- word 需要解密的字符串  s 解密之后的字符串 字符串长度
* @return -- 0 解密成功 -1 解密成功
*/
int Decrypt(char* word,char* s,int len)
{
	if(len<=0)
	{
		return -1;
	}
	for (int i = 0; i < len; i++) //对24个字符进行解码操作
    {
        char a = word[i];
        char b = key[i % 13];
        s[i] = DecryptChar(a, b);
    }
	return 0;
}
