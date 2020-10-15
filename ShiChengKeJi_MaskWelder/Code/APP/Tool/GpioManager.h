#ifndef	_GPIOMANAGER_H_
#define	_GPIOMANAGER_H_

	//输入输出编号
	//CardId:板卡号，0主卡，>0扩展卡
	//idx	:板卡输入输出丝印号，从1开始
	#define	GPIO(idx) (idx-1)
	#define	EGPIO(CardId,idx) (CardId*1000+idx-1)

	typedef enum
	{
		ON=0,
		OFF=1,
		ONR=1,
		OFFR=0,
	}GpioSta;
	
	typedef enum
	{
		I_Origin1		=	GPIO(1),	//位移轴原点
		I_Origin2		=	GPIO(2),	//升降轴原点
		I_Origin3		=	GPIO(3),	//拉耳轴原点
		I_Origin4		=	GPIO(4),	//左前轴原点
		I_Origin5		=	GPIO(5),	//右前轴原点
		I_Origin6		=	GPIO(6),	//左后轴原点
		I_Origin7		=	GPIO(7),	//右后轴原点
		I_Origin8		=	GPIO(8),	//预留轴原点
		I_BeltSupplyL	=	GPIO(9),	//左放带启动
		I_BeltSupplyR	=	GPIO(10),	//右放带启动
		I_WeldCydDown	=	GPIO(11),	//点焊气缸下
		I_WeldCydUp		=	GPIO(12),	//点焊气缸上
		I_EntranceInp	=	GPIO(13),	//入口来料感应
		I_FoldCheck		=	GPIO(14),	//折耳检测-暂时不用
		I_WeldInp		=	GPIO(15),	//焊前来料感应
		I_ExitCheck		=	GPIO(16),	//出料检测
		I_Start			=	GPIO(17),	//启动
		I_Stop			=	GPIO(18),	//停止
		I_Reset			=	GPIO(19),	//复位
		I_Scram			=	GPIO(20),	//急停
		I_AxHitOri		=	GPIO(21),	//打料轴原点+

		Q_CollectMotor	=	GPIO(1),	//集料电机
		Q_Weld			=	GPIO(2),	//超声波
//		Q_WeldLift		=	GPIO(3),	//点焊升降气缸
		Q_PickClamp		=	GPIO(4),	//双夹气缸
		Q_Cut			=	GPIO(5),	//剪带气缸
		Q_PullClamp		=	GPIO(6),	//拉夹气缸
		Q_Press			=	GPIO(7),	//压带气缸
		Q_BeltSupplyL	=	GPIO(8),	//左放带电机
		Q_BeltSupplyR	=	GPIO(9),	//右放带电机
		Q_ExitMotor		=	GPIO(10),	//出料电机（传送带）
		Q_UnloadCyd		=	GPIO(11),	//下料气缸
		Q_AlarmBeep		=	GPIO(12),	//蜂鸣器
		Q_MaskSupply	=	GPIO(13),	//口罩供应
		
	}Gpio;

	extern GpioSta I_Get(Gpio idx);
	extern void Q_Set(Gpio idx, GpioSta val);
	extern GpioSta Q_Get(Gpio idx);

#endif
