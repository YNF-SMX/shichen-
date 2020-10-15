#include "JogOperate.h"
//#include "axislib.h"
#include "axis.h"

const u32 JogPerPulseBuf[8] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};
extern u32* errocode;
extern u32* flahsave;
extern void* globaldata;
extern u32 sysdatalen;
extern u32 userwritelen;
extern u32 userreadlen;
extern u32 usersavelen;

extern void JogGo(u8 axisnum,s32 pos,u32 spd);
extern void jogstop(u32 axisnum);
extern void joghome(u32 axisnum);

extern AxParaDef AxPara[AXIS_NUM] ;

typedef union
{
    u32	Data_32[10];
    struct
    {
        s32	StartSpeed;		    //初速度
        s32	Acctime;			//加速时间
        s32	RunSpeed;			//点动速度
        s32	Dectime;			//减速时间
        s32	EndSpeed;			//末速度
        s32	HomeSpeedFast;		//回原点快速速度
        s32	HomeSpeedSlow;		//回原点慢速速度
        s32	HomeOffset;			//回原点偏移坐标
        s32	SoftMinLimit;		//软负限位
        s32	SoftMaxLimit;		//软正限位
    } Data;
} AxisDef;

typedef struct
{
    u32	JogMode;			//1106-	点动模式
    u32	JogPositionLevel;	//1108-	点动行程级别
    u32	JogPosition;		//1110-	点动设定行程
    u32	JogForward[2];		//1112-1115	正向点动
    u32	JogBackward[2];		//1116-1119	反向点动
    u32	JogGohome[2];		//1120-1123	点动回零
    u32	JogStop[2];			//1124-1127	点动立即停止
    u32	jogSpd;				//1128  轴点动百分比速度
} jogPara;



JogOperateParaDef JogOperatePara[MAXAXISNUM] = {0};
void HZ_JogOperate(int M_Axnum)
{
    u8 i;
    jogPara* jog;
    jog = (jogPara*)((u16*)globaldata+1106);
    for(i=0; i<(MAXAXISNUM); i++)
    {
        if(jog->JogStop[0] & (1<<i))
        {
            jog->JogStop[0] &= ~(1<<i);
            jog->JogForward[0] &= ~(1<<i);
            jog->JogBackward[0] &= ~(1<<i);
            jog->JogGohome[0] &= ~(1<<i);		
            jogstop(i);
            JogOperatePara[i].step = 0;
        }
        else
        {
            if(JogOperatePara[i].step == 0)
            {
                if(jog->JogForward[0] & (1<<i))
                {
                    JogOperatePara[i].motion = _FORWARD;
                    JogOperatePara[i].step = 1;
                }
                if(jog->JogBackward[0] & (1<<i))
                {
                    JogOperatePara[i].motion = _BACKWARD;
                    JogOperatePara[i].step = 1;
                }
                if(jog->JogGohome[0] & (1<<i))
                {
                    //SetAxisSpdRadio(i,jog->jogSpd);
                    joghome(i);
                    jog->JogGohome[0] &= ~(1<<i);
                }
            }

            if(jog->JogMode != JOGSPEED)
            {
                jog->JogForward[0] &= ~(1<<i);
                jog->JogBackward[0] &= ~(1<<i);
            }

            switch(JogOperatePara[i].step)
            {
            case 1:
                JogOperatePara[i].step = 2;
                switch(JogOperatePara[i].motion)
                {

                case _FORWARD:
                    switch(jog->JogMode)
                    {
                    case JOGLEVEL:
                        JogGo(i,JogPerPulseBuf[jog->JogPositionLevel],jog->jogSpd);
                        break;

                    case JOGPULSE:
                        JogGo(i,jog->JogPosition,jog->jogSpd);
                        break;

                    case JOGSPEED:
						JogGo(i,0xfffffff,jog->jogSpd);
                        break;
                    }
                    break;

                case _BACKWARD:
                    switch(jog->JogMode)
                    {
                    case JOGLEVEL:
                        JogGo(i,-(JogPerPulseBuf[jog->JogPositionLevel]),jog->jogSpd);
                        break;

                    case JOGPULSE:
                        JogGo(i,-(jog->JogPosition),jog->jogSpd);
                        break;

                    case JOGSPEED:
						JogGo(i,-1*0xfffffff,jog->jogSpd);
                        break;
                    }
                    break;
                }
                break;

            case 2:
                switch(jog->JogMode)
                {
                case JOGLEVEL:
                case JOGPULSE:
					if( ((i<M_Axnum) && (HZ_AxGetStatus(i) == AXSTA_READY || HZ_AxGetStatus(i) == AXSTA_ERRSTOP)) \
						||((i>=M_Axnum) && (HZ_ExAxGetStatus(i-M_Axnum) == AXSTA_READY || HZ_ExAxGetStatus(i-M_Axnum) == AXSTA_ERRSTOP)))
					{
						JogOperatePara[i].step = 0;
						jog->JogStop[0] &= ~(1<<i);
						jog->JogForward[0] &= ~(1<<i);
						jog->JogBackward[0] &= ~(1<<i);
						jog->JogGohome[0] &= ~(1<<i);
					}
                    break;

                case JOGSPEED:
                    if((jog->JogForward[0] & (1<<i)) == 0 && (jog->JogBackward[0] & (1<<i)) == 0)
                    {
                        JogOperatePara[i].step = 3;
                        jogstop(i);
                    }
                    break;
                }
                break;
            case 3:
                switch(jog->JogMode)
                {
                case JOGLEVEL:
                case JOGPULSE:
                    if( ((i<M_Axnum) && (HZ_AxGetStatus(i) == AXSTA_READY || HZ_AxGetStatus(i) == AXSTA_ERRSTOP)) \
						||((i>=M_Axnum) && (HZ_ExAxGetStatus(i-M_Axnum) == AXSTA_READY || HZ_ExAxGetStatus(i-M_Axnum) == AXSTA_ERRSTOP)))
                    {
                        JogOperatePara[i].step = 0;
                        jog->JogStop[0] &= ~(1<<i);
                        jog->JogForward[0] &= ~(1<<i);
                        jog->JogBackward[0] &= ~(1<<i);
                        jog->JogGohome[0] &= ~(1<<i);
                    }
                    break;

                case JOGSPEED:
                    if( ((i<M_Axnum) && (HZ_AxGetStatus(i) == AXSTA_READY || HZ_AxGetStatus(i) == AXSTA_ERRSTOP)) \
						||((i>=M_Axnum) && (HZ_ExAxGetStatus(i-M_Axnum) == AXSTA_READY || HZ_ExAxGetStatus(i-M_Axnum) == AXSTA_ERRSTOP)))
                    {
                        JogOperatePara[i].step = 0;
                    }
                    break;
                }
                break;
            }
        }
    }
}
__weak s32  HZ_ExAxGetStatus(u8 ExAxNum)
{
	return AXSTA_READY;
}

