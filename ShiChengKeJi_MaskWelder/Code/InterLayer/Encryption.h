#ifndef __ENCRYPTION_H
#define __ENCRYPTION_H

//十进制转ASCII码
#define KB2ASC(x)	 (unsigned char )(0x30+x)

//ASCII码转十进制
#define ASC2KB(x)	 (unsigned char )(x%(0x30))
    



//只能对规定的四个字节进行解码
extern int Decode(int* decode,char* decodeData,unsigned int cid,unsigned int rng);

extern int Decrypt(char* word,char* s,int len);
	
extern int Encrypt( char* word, char* s,int len);

#endif
