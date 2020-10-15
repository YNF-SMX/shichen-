#ifndef _DATADEFINE_H_
#define _DATADEFINE_H_

//#include "logicdata.h"
#include "lock.h"

#define DATACHAECK 0x01237654               //校验码
#define SYSREADLEN 1000                     //系统读区寄存器长度
#define USERREADLEN 1000                    //用户读区寄存器长度
#define READLEN (SYSREADLEN + USERREADLEN)  //系统存区寄存器总长度

#define SYSWRITELEN 1000                      //系统写区长度
#define USERWRITELEN 1000                     //用户写区长度
#define WRITELEN (SYSWRITELEN + USERWRITELEN) //总写区长度

#define SYSSAVELEN 4000                         //系统寄存器长度
#define USERSAVELEN 10000                       //用户存储寄存器长度
#define SAVELEN (SYSSAVELEN + USERSAVELEN + 4) //用户存储区总长

#define GLOBALDATALEN (READLEN + WRITELEN + SAVELEN) //modbus总地址长度

#define MAXAXISNUM 50           //最大轴数
#define INPUTSTARTADDR 310      //modbus输入检测地址
#define INPUTENDADDR 389        //
#define OUTPUTSTARTADDR 1020    //modbus输出更新地址
#define OUTPUTENDADDR 1099      //


/************************************************************************/
/* define the user type									                */
/************************************************************************/
#define TAR(num,pos)  (GSR.AxisUnitPosition[num]-pos < 0.01f)

/******************  Bit definition for PointPara  *******************/
#define  OPDL                       ((uint16_t)0x0001)
#define  ELOP                       ((uint16_t)0x0002)
#define  CLDL                       ((uint16_t)0x0004)

/************************************************************************/
/* sysmodbus data define                                		        */
/************************************************************************/
typedef struct
{
    u8 hour;		//范围0-23
    u8 min;			//0-59
    u8 sec;			//0-59
    u8 ampm;		//这个值没用，禁止使用
} Time;

//轴速度配置
typedef struct AxSpd_t
{
	float  startspeed;  //unit/s
	float  acctime;		//ms
	float  runspeed;	//unit/s
	float  dectime;		//ms
	float  endspeed;	//unit/s
}AxspdDef;

//轴回零配置
typedef struct AxHome_t	
{
	u32	orgnum;			 	//原点
	u16 orglev;			 	//原点信号电平
    u16 homemode;		 	//回零模式
	float homespeedfast;    //unit/s 回零快速
	float homespeedslow;  	//unit/s 回零慢速
	float homespeedoffset; 	//unit/s 零点偏移
}AxHomeDef;

//轴限位配置
typedef struct AxLimit_t
{
	u16 limitMode;		//限位模式：0 没限位 1 软件限位 2 硬件限位 3 软硬都限
	u16	alarmmode;		//轴报警口配置  0 1 2报警电平信号
    u16 poslimitsig;    //正限位信号
    u16 poslimitlev;    //正限位信号电平
    u16 neglimitsig;    //负限位信号
    u16 neglimitlev;    //负限位信号电平
    float softminlimit;   //软负限位 unit
    float softmaxlimit;   //软正限位 unit 
} AxislimitDef;

//轴单位转换
typedef struct AxisConversion_t
{
    u32 PPR;	//每转脉冲数
    float MPR;	//每转毫米__导程
} AxisConversion; 

//轴参数类
typedef struct AxisPara_t
{
    AxspdDef AxSpd;             //10个寄存器 
    AxHomeDef Axhomecfg;        //10个寄存器
    AxislimitDef Axlimitcfg;    //10个寄存器
    AxisConversion Axconver;    //4个寄存器
}AxisData;



//modbus 地址交互表
typedef union {
    u8 Data_8[GLOBALDATALEN];
    u16 Data_16[GLOBALDATALEN / 2];
    struct
    {
        struct //READ ADDR 0-999
        {
            union //SYS READ ADDR 0-499 系统只读寄存器
            {
                u16 Data_16[SYSREADLEN / 2];
                struct
                {
                    u32 HardWare_Ver[10];		  //0-19	硬件版本
                    u32 SoftWare_Ver[10];		  //20-39	软件版本
                    u32 res[5];                   //40-49
                    u16 AXSTA[MAXAXISNUM];		  //50-99	轴状态
                    float AxisUnitPosition[MAXAXISNUM];  //100-199 用户单位的当前位置
                    s32 AxisPosition[MAXAXISNUM]; //200-299	轴当前位置
                    u32 AxisEncoder[5];			  //300-309	轴当前状态
                    u32 InputStatus[40];		  //310-389	输入口状态
                    u32 ErrorCode[20];			  //390-	错误码
                    u32 ErrorLevel;				  //430-	错误等级
                    Time time;	//432	rtc 时间
                    Date date;	//434	rtc 日期
                    u32 CID[2];	//436 438 本机生成的客户随机码

                } Data;
            } Sys;

            union //USER READ ADDR 500-999 用户只读寄存器
            {
                u16 Data_16[USERREADLEN / 2];
                struct
                {
                    u32 RunStatus; 		//500 运行状态显示
                    u32 HaveToReset;    //502 复位标志
                    u32 HaveToRepower;  //504 重启标志
					u32	HitReady;		//506 打料预备，复位之后才能打料
                } Data;
            } User;
        } Read;

        struct //WRITE ADDR 1000-1999 读写寄存器区，掉电数据丢失为0
        {
            union //SYS WRITE ADDR 1000-1499 系统读写寄存器，掉电不保存
            {
                u16 Data_16[SYSWRITELEN / 2];
                struct
                {
                    u32 res[10];		  	//1000-1019 保留
                    u32 OutputStatus[40]; 	//1020-1099	输出口状态
                    u32 FlashOperate;	 	//1100-	Flash操作
                    u32 FilesOperate;	 	//1102-	文件操作
                    u32 FramOperate;	  	//1104-	铁电操作
                    u32 JogMode;		  	//1106-	点动模式 0 1 2
                    u32 JogPositionLevel; 	//1108-	点动行程级别0 1 2 3 4
                    u32 JogPosition;	  	//1110-	点动设定行程
                    u32 JogForward[2];		//1112-1115	正向点动
                    u32 JogBackward[2];   	//1116-1119	反向点动
                    u32 JogGohome[2];	 	//1120-1123	点动回零
                    u32 JogStop[2];		  	//1124-1127	点动立即停止
                    u32 jogSpd;			  	//1128  轴点动百分比速度hz
                    u32 ClearAlarm;		  	//1130-	清除错误
                    struct {
                        u32 refreshId;	//	1132 随机数刷新
                        s32 decode[4];	// 1134 1136 1138 1140  4个解锁码 *6个数 一共24个数
                    } LockPara;
                    //GoPosPara pos[PULS_NUM]; //1142+32*2 = 1206
                    //end
                } Data;
            } Sys;

            union //USER WRITE ADDR 1500-1999 用户读写寄存器
            {
                u16 Data_16[USERWRITELEN / 2];
                struct
                {
                    struct
                    {
                        u32 RunCommand;		//1500  0 初始状态 1停止 2 运行 3 复位 4 急停 5暂停 6下载态
                        u32 res[9];
                    } Button; //留10个int 的空间 
					u32 TeachWord;	//1520
					u32	GotoPos;	//1522
					u32	JogFeed;	//1524
					u32	JogSpd;		//1526
					u32	HmiStart;	//1528
                } Data;
            } User;
        } Write;

        struct //SAVE ADDR 存储区地址寄存器
        {
            union //SYS SAVE ADDR 2000-3999 //系统存储寄存器地址
            {
                u16 Data_16[SYSSAVELEN / 2];
                struct
                {
                    AxisData axis[MAXAXISNUM];       //轴的运行参数 34*50  2000-3699 
                    struct 
                    {   //3700-3719
                        u32 checkflag;  //初始化检测，当新板块没写过数据或是数据不是规定值时，需要初始化modbus的存储寄存器初值，防止出现新卡上电全ff或数据清空后，导致的撞机等意外情况。初始化过数据后写0xea
                        u32 res[9];     //预留，用做锁机时间等
                    }SaveCheck;
                    
                } Data;
            } Sys;

            union //USER SAVE ADDR 4000- 8999 //用户存储寄存器地址
            {
                u16 Data_16[USERSAVELEN / 2];
                struct
                {
					union//4000
					{
						u32	Data_32[50];
						struct
						{
							s32	PullAlmEn;		//4000	拉线报警启用
							s32	WeldEn;			//4002	超声波启用
							s32	TailingsEn;		//4004	尾料模式启用
							s32	GlobalSpeed;	//4006	全局速度比例
							u32	UnloadPackNum;	//4008	单组集料数量
							s32	EntranceInpEn;	//4010	入口来料检测启用
							s32	WeldInpEn;		//4012	焊耳来料检测启用，改成用入口来料感应
							u32	HitMode;		//4014	0:打料电机	1：打料气缸
						}Data;
					}Config;
					
					union//4100
					{
						u32	Data_32[50];
						struct
						{
							u32	WeldOnDelay;		//4100	焊接时间
							u32	WeldOffDelay;		//4102	固话时间
							u32	EntranceInpDelay;	//4104	入口来料感应延时
							u32	WeldInpDelay;		//4106	焊耳来料感应延时
							u32	SupplyDelay;		//4108	供带电机启动延时
							u32	PickClampOnDelay;	//4110	取带气缸On时间（双夹气缸）
							u32	PickClampOffDelay;	//4112	取带气缸Off时间（双夹气缸）
							u32	PullClampOnDelay;	//4114	拉带气缸On时间
							u32	PullClampOffDelay;	//4116	拉带气缸Off时间
							u32	PressOnDelay;		//4118	压带气缸On时间
							u32	PressOffDelay;		//4120	压带气缸Off时间
							u32	CutOnDelay;			//4122	切带气缸On时间
							u32	CutOffDelay;		//4124	切带气缸Off时间
							u32	WeldLiftOnDelay;	//4126	焊头气缸On时间
							u32	WeldLiftOffDelay;	//4128	焊头气缸Off时间
							u32	UnloadOnDelay;		//4130	出料气缸On时间
							u32	UnloadOffDelay;		//4132	出料气缸Off时间
							u32	BlowBeltUpInp;		//4134	吹带上翻感应延时
							u32	BlowBeltUpDelay;	//4136	吹带上翻时间
							u32	BlowBeltInInp;		//4138	吹带内翻感应延时
							u32	BlowBeltInDelay;	//4140	吹带内翻时间
							u32	UnloadInp;			//4142	下料感应延时
							u32	UnloadBeltDelay;	//4144	下料皮带延迟启动
							u32	UnloadBeltTime;		//4146	下料皮带动时间
						}Data;
					}Delay;
					
					union//4200
					{
						u32	Data_32[50];
						float FData_32[50];
						struct
						{
							float FlowReady;		//4200	传送预备位
							float FlowFeed;         //4202	传送给进位
							float PickZTake;		//4204	上下取线位
							float PickZPut;			//4206	上下放线位
							float PullBack;         //4208	拉线回退位
							float PullTake;         //4210	拉线取线位
							float PickTakeLF;       //4212	左前取线位
							float PickPutLF;        //4214	左前放线位
							float PickTakeRF;       //4216	右前取线位
							float PickPutRF;        //4218	右前放线位
							float PickTakeLB;       //4220	左后取线位
							float PickPutLB;        //4222	左后放线位
							float PickTakeRB;       //4224	右后取线位
							float PickPutRB;        //4226	右后放线位
							float PickZReady;		//4228	上下轴预备位
							float WeldReady;		//4230	焊接轴上位置
							float WeldWork;			//4232	焊接轴下位置
							float UnloadUp;			//4234	下料打料上
							float UnloadDown;		//4236	下料打料下
						}Data;
					}Position;
					
					union//4300
					{
						u32	Data_32[50];
						struct
						{
							float	Pcs_Min;		//4300	分钟产量
							u32		Cnt;			//4302	总产量
						}Data;
					}Product;
					
					union//4400
					{
						u32	Data_32[100];
						struct
						{
							float	StartSpd[20];	//4400
							int		AccTime[20];	//4440
							float	RunSpd[20];		//4480
							float	HomeSpd[20];	//4520
							int		Direction[20];	//4560
						}Data;
					}AxisCfg;
                } Data;
            } User;
        } Save;
        u32 Check; //校验码
    } SpaceType;
} GlobalDataDef;
extern GlobalDataDef GlobalData;

#define GSR GlobalData.SpaceType.Read.Sys.Data
#define GSW GlobalData.SpaceType.Write.Sys.Data
#define GSS GlobalData.SpaceType.Save.Sys.Data
#define GUR GlobalData.SpaceType.Read.User.Data
#define GUW GlobalData.SpaceType.Write.User.Data
#define GUS GlobalData.SpaceType.Save.User.Data

#endif
