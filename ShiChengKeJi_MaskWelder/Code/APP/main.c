/*
 * @Author: your name
 * @Date: 2020-05-26 14:03:11
 * @LastEditTime: 2020-05-27 14:03:30
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \YD_KZJ\Code\APP\main.c
 */ 
#include "interlayer.h"
#include "bsp_config.h"
#include "appinit.h"
#include "maintask.h"
#include "can_master.h"
/*我就不信邪了 这个傻逼玩意，第一次上传*/
const uint32_t Code_Versions __attribute__((at(0x08040800))) = 0xA12;

int main()
{
    //sys_init();
	extern void Hz_AutoVision(void);
	Hz_AutoVision();
 	sys_init_IAP();
    bsp_init();
    bsp_exec();

    AppInit();
	while(2 != HZ_ExCanNetStateGet())
	{
        InterLayer();
        bsp_exec();
	}
    while(1)
    {
		MainTask();
        InterLayer();
        bsp_exec();
    }
}


extern void GusToFram(void);
int pwr_Task(void)
{
	GusToFram();
    return 0;
}

u32 testtt;
u32 Buffer[250];
//参数文件导出
int File_export(void)
{
	testtt =  File_Write("Config",&GUS,0,1200,0);
	return testtt;
}

//参数文件导入
int File_import(void)
{
	testtt = File_Read("Config",&GUS,0,1200,0);
	return testtt;
}




