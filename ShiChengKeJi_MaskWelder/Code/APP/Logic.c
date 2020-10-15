 /*
 * File: Logic.c
 * File Created: Tuesday, 9th October 2018 2:43:29 pm
 * version: v00.00.01 8��ײ�
 * Description:�û��߼�
 * Modified By:yang
 * -----
 * Copyright 2018 - 2018, <<hzzh>>
 */

#include "Logic.h"
#include "math.h"

#define PI 3.1415926

/*ȫ�ֱ�����ͳһ����*/
Task LogicTask = { 0 }; //ͳһ��������
SysData Data = { 0 };  //ͳһ��������
Gpio I_BeltSupply[2] = {I_BeltSupplyL,I_BeltSupplyR};
Gpio Q_BeltSupply[2] = {Q_BeltSupplyL,Q_BeltSupplyR};
u32	AxBelt[2] = {AxBelt_1,AxBelt_2};
LogicParaDef my;

//�豸������
UphDef DeveceUph;
void DeviceMain(LogicParaDef *p)
{
	static u32 Tm_I_EntranceInp,I_EntranceInp_Ok;
	//�ⲿ�����ṩ�豸�������źſ���
	if(FSM_STA == RUN)//��̨����״̬
	{
		Q_Set(Q_MaskSupply,ON);//�����ⲿ���ֻ�����
		Q_Set(Q_ExitMotor,ON);
		my.execute = 1;
	}
	//һֱ������״̬��������˲�����Ҫ�ȴ����̿�ʼ�ܵ�ʱ���ټ�⣬����ʱ��
	if(TimerCnt(&Tm_I_EntranceInp,I_Get(I_EntranceInp) == ON) > GDLY.EntranceInpDelay)
	{
		I_EntranceInp_Ok = 1;
	}
	else
	{
		I_EntranceInp_Ok = 0;
	}
	if(FSM_STA != RUN)return;
	TASK_PROCESS
	{
		case 1://�ȴ�����п��֣�λ�����ƶ�һ��
			if((I_EntranceInp_Ok == 1 || GCFG.EntranceInpEn == 0))
			{
				AxRelUnit(AxFlow,GSPD,GPOS.FlowFeed);
				my.step = 2;
			}
			break;
			
		case 2://λ�Ƶ�λ��ִ�����Ϻͺ��Ӷ���
			if(AxSta(AxFlow) == AXSTA_READY)
			{
				LT.WeldTask.execute = 1;
				my.step = 3;
			}
			break;
			
		case 3://������ɺ󣬼�����ܣ�����
			if(LT.WeldTask.execute == 0)
			{
				UphCal(&DeveceUph);
				GUS.Product.Data.Pcs_Min = DeveceUph.Upm;
				GUS.Product.Data.Cnt++;
				my.execute = 0;
				my.step = 0;
			}
			break;
	}
}

//ȡ�ߣ�ȡ���õ��ߣ�����λ�Ϸ��ȴ�
void Pick(LogicParaDef *p)
{
	static u32 Tm,Tm2;
	TASK_PROCESS
	{
		case 1:
			Q_Set(Q_PickClamp,OFFR);//�ſ�����
			TimerRst(&Tm);//��ʱ
			my.step = 2;
			break;
			
		case 2://�����ᵽԤ��λ
			if(AxSta(AxPickZ) == AXSTA_READY && TimerCnt(&Tm,1) >= GDLY.PickClampOffDelay)
			{
				AxAbsUnit(AxPickZ,GSPD,GPOS.PickZReady);
				my.step = 3;
			}
			break;
		
		case 3://�����ᵽλ��ǰ�����ҵ�ȡ��λ
			if(AxInpSta(AxPickZ) == AXSTA_READY &&
			   AxSta(AxPickLB) == AXSTA_READY &&
			   AxSta(AxPickLF) == AXSTA_READY &&
			   AxSta(AxPickRB) == AXSTA_READY &&
			   AxSta(AxPickRF) == AXSTA_READY)
			{
				AxAbsUnit(AxPickLB,GSPD,GPOS.PickTakeLB);
				AxAbsUnit(AxPickLF,GSPD,GPOS.PickTakeLF);
				AxAbsUnit(AxPickRB,GSPD,GPOS.PickTakeRB);
				AxAbsUnit(AxPickRF,GSPD,GPOS.PickTakeRF);
				my.step = 4;
			}
			break;
			
		case 4://ǰ�����ҵ�λ
			if(AxInpSta(AxPickLB) == AXSTA_READY &&
			   AxInpSta(AxPickLF) == AXSTA_READY &&
			   AxInpSta(AxPickRB) == AXSTA_READY &&
			   AxInpSta(AxPickRF) == AXSTA_READY)
			{
				//�������û��ɣ���һ����
				if(LT.PullTask.done != 1)
				{
					LT.PullTask.execute = 1;
				}
				my.step = 5;
			}
			break;
			
		case 5:
			if(LT.PullTask.execute == 0)//���߽���
			{
				if(LT.PullTask.done == 1)//�ж����߽����OK����
				{
					LT.PullTask.done = 0;
					my.step = 6;
				}
				else//NG����������ʧ�ܣ��������
				{
					my.execute = 0;
					my.step = 0;
				}
			}
			break;
			
		case 6://�����ᵽȡ��λ
			if(AxSta(AxPickZ) == AXSTA_READY)
			{
				AxAbsUnit(AxPickZ,GSPD,GPOS.PickZTake);
				my.step = 7;
			}
			break;
			
		case 7://�����ᵽλ
			if(AxSta(AxPickZ) == AXSTA_READY &&
			   AxSta(AxPickLB) == AXSTA_READY &&
			   AxSta(AxPickLF) == AXSTA_READY &&
			   AxSta(AxPickRB) == AXSTA_READY &&
			   AxSta(AxPickRF) == AXSTA_READY)
			{
				TimerRst(&Tm);
				Q_Set(Q_PickClamp,ONR);//ȡ�߼мн�
				my.step = 8;
			}
			break;
			
		case 8://��ʱ��
			if(TimerCnt(&Tm,1) >= GDLY.PickClampOnDelay)
			{
				Q_Set(Q_PullClamp,OFFR);//���߼����ſ�
				Q_Set(Q_Cut,ON);//��������
				TimerRst(&Tm);
				TimerRst(&Tm2);
				my.step = 9;
			}
			break;
			
		case 9://��ʱ��
			if(TimerCnt(&Tm,1) >= GDLY.CutOnDelay)
			{
				Q_Set(Q_Cut,OFF);//�����ſ�
				TimerRst(&Tm);
				my.step = 10;
			}
			break;
			
		case 10://��ʱ��
			if(TimerCnt(&Tm2,1) >= GDLY.PullClampOffDelay && TimerCnt(&Tm,1) >= GDLY.CutOffDelay)
			{
				AxAbsUnit(AxPickZ,GSPD,GPOS.PickZReady);//�����ᵽ��λ��
				my.step = 11;
			}
			break;
			
		case 11://��������λ�ú�
			if(AxInpSta(AxPickZ) == AXSTA_READY)
			{
				LT.PullTask.execute = 1;//����
				my.step = 12;
			}
			break;
			
		case 12://�ᶼ��Ԥ��״̬���ҵ㺸��������λ��
			if(AxInpSta(AxPickZ) == AXSTA_READY &&
			   AxSta(AxPickLB) == AXSTA_READY &&
			   AxSta(AxPickLF) == AXSTA_READY &&
			   AxSta(AxPickRB) == AXSTA_READY &&
			   AxSta(AxPickRF) == AXSTA_READY &&
			   AxSta(AxWeld) == AXSTA_READY)
			{
				//ǰ�����ҵ�����λ
				AxAbsUnit(AxPickLB,GSPD,GPOS.PickPutLB);
				AxAbsUnit(AxPickLF,GSPD,GPOS.PickPutLF);
				AxAbsUnit(AxPickRB,GSPD,GPOS.PickPutRB);
				AxAbsUnit(AxPickRF,GSPD,GPOS.PickPutRF);
				my.step = 13;
			}
			break;
			
		case 13://ǰ�����ҵ�λ�󣬽���
			if(AxSta(AxPickZ) == AXSTA_READY &&
			   AxInpSta(AxPickLB) == AXSTA_READY &&
			   AxInpSta(AxPickLF) == AXSTA_READY &&
			   AxInpSta(AxPickRB) == AXSTA_READY &&
			   AxInpSta(AxPickRF) == AXSTA_READY)
			{
				my.step = 0;
				my.execute = 0;
				my.done = 1;
			}
			break;
	}
}

//����
void Weld(LogicParaDef *p)
{
	static u32 Tm;
//	static u32 Tm_WeldUp,Tm_WeldDown;
	//�㺸���ױ���
//	if(TimerCnt(&Tm_WeldDown,Q_Get(Q_WeldLift) == ON && I_Get(I_WeldCydDown) != ON) > 500)
//	{
//		AlarmSetBit(2,0,4);//�㺸�����¸�Ӧ�쳣
//	}
//	if(TimerCnt(&Tm_WeldUp,Q_Get(Q_WeldLift) == OFF && I_Get(I_WeldCydUp) != ON) > 500)
//	{
//		AlarmSetBit(2,0,5);//�㺸�����ϸ�Ӧ�쳣
//	}
	
	TASK_PROCESS
	{
		case 1://��ʱ
			TimerRst(&Tm);
			my.step = 2;
			break;
		
		case 2://��ǰ�������
			if(my.count || GCFG.EntranceInpEn == 0)
			{
				if(my.count > 0)//my.count������Ϊ��ǰ��⵽�ϵı�־
				{
					my.count--;
				}
				my.step = 3;//ȥ����
			}
			else
			{
				my.step = 9;//ȥ����
			}
			break;
		
		case 3://��ѯ�Ƿ�ȡ������
			if(LT.PickTask.done != 1)
			{
				LT.PickTask.execute = 1;//û�о�ȥȡ
			}
			my.step = 4;
			break;
			
		case 4://ȷ��ȡ�����
			if(LT.PickTask.execute == 0 && AxSta(AxWeld) == AXSTA_READY)
			{
				if(LT.PickTask.done == 1)
				{
					LT.PickTask.done = 0;
					AxAbsUnit(AxPickZ,GSPD,GPOS.PickZPut);//�������½�
					AxAbsUnit(AxWeld,GSPD,GPOS.WeldWork);//�㺸�����½�
					TimerRst(&Tm);
					my.step = 5;
				}
				else//ȡ��ʧ�ܣ��������������
				{
					my.execute = 0;
					my.step = 0;
				}
			}
			break;
			
		case 5://�ȴ�������͵㺸�����µ�λ
			if(AxSta(AxPickZ) == AXSTA_READY && AxSta(AxWeld) == AXSTA_READY)
			{
				LT.PickTask.execute = 1;//ȡ���ֿ���ȥȡ����
				if(GCFG.WeldEn == 1)
				{
					Q_Set(Q_Weld,ON);//��������
				}
				TimerRst(&Tm);
				my.step = 6;
			}
			break;
			
		case 6://�ȴ���������ʱ����
			if(TimerCnt(&Tm,1) >= GDLY.WeldOnDelay)
			{
				Q_Set(Q_Weld,OFF);//�رճ�����
				TimerRst(&Tm);
				my.step = 7;
			}
			break;
			
		case 7://�ȴ��̻�ʱ��
			if(TimerCnt(&Tm,1) >= GDLY.WeldOffDelay)
			{
				AxAbsUnit(AxWeld,GSPD,GPOS.WeldReady);
				TimerRst(&Tm);
				my.step = 8;
			}
			break;
			
		case 8://�ȴ�����������ߵ�ǰλ����Ԥ��λС���г̵�1/3ʱ
//			if(I_Get(I_WeldCydUp) == ON && FSM_STA == RUN)
//			if(TimerCnt(&Tm,1) > GDLY.WeldLiftOffDelay)
			if(AxSta(AxWeld) == AXSTA_READY || fabs(GPOS.WeldReady - GSR.AxisUnitPosition[AxWeld]) <= fabs(GPOS.WeldReady - GPOS.WeldWork)/2)
			{
				my.step = 9;//ȥ����
			}
			break;
			
		case 9:
			if(I_Get(I_WeldInp) == ON)//���һ�º�ǰ���ϼ��
			{
				my.count++;//���Ͼ�++
			}
			my.execute = 0;
			my.step = 0;
			break;
	}
}

//����
void Pull(LogicParaDef *p)
{
	static u32 Tm;
	static u32 I_BeltSupply_Off[2];
	//������߸�Ӧ�����߹�������û��OFF�źţ��оʹ�����������
	for(int i=0; i<2; i++)
	{
		if(I_Get(I_BeltSupply[i]) == OFF)
		{
			I_BeltSupply_Off[i] = 1;
		}
	}
	TASK_PROCESS
	{
		case 1://���߼����ſ��������ᵽȡ��λ
			Q_Set(Q_PullClamp,OFFR);
			AxAbsUnit(AxPull,GSPD,GPOS.PullTake);
			my.step = 2;
			break;
		
		case 2://���Ӽн�
			if(AxSta(AxPull) == AXSTA_READY)
			{
				Q_Set(Q_PullClamp,ONR);
				TimerRst(&Tm);
				my.step = 3;
			}
			break;
			
		case 3://ѹ���ɿ�
			if(TimerCnt(&Tm,1) >= GDLY.PullClampOnDelay)
			{
				Q_Set(Q_Press,ON);
				TimerRst(&Tm);
				my.step = 4;
			}
			break;
			
		case 4://�����ᵽ���ߺ�λ
			if(TimerCnt(&Tm,1) >= GDLY.PressOnDelay)
			{
				TimerRst(&Tm);
				AxAbsUnit(AxPull,GSPD,GPOS.PullBack);
				my.step = 5;
			}
			break;
			
		case 5://����һ��ʱ��֮�󣬻�������������ˣ���ѹ��
			if(AxSta(AxPull) == AXSTA_READY)
			{
				TimerRst(&Tm);
				my.step = 6;
			}
			break;
			
		case 6://ѹ�����׶���ʱ�� �� ��������������̽���
			if(TimerCnt(&Tm,1) >= GDLY.PressOffDelay && AxSta(AxPull) == AXSTA_READY)
			{
				Q_Set(Q_Press,OFF);
				LT.BeltSupplyTask[0].execute = 1;
				LT.BeltSupplyTask[1].execute = 1;
				my.execute = 0;
				my.step = 0;
				if(GCFG.PullAlmEn == 1 && (I_BeltSupply_Off[0] == 0 || I_BeltSupply_Off[1] == 0))
				{
					my.count++;
					if(my.count >= 3)
					{
						my.count = 0;
						I_BeltSupply_Off[0] = 0;
						I_BeltSupply_Off[1] = 0;
						AlarmSetBit(2,0,6);//�����߶���ʧ��
					}
					else
					{
						my.done = 1;
					}
				}
				else
				{
					my.count = 0;
					I_BeltSupply_Off[0] = 0;
					I_BeltSupply_Off[1] = 0;
					my.done = 1;
				}
			}
			break;
	}
}

//����
void BeltSupply(LogicParaDef *p, s32 i)
{
	static u32 Tm[2];
	//���������ź�û�и�Ӧʱ
	if(I_Get(I_BeltSupply[i]) == OFF)
	{
		my.execute = 1;
	}
	TASK_PROCESS
	{		
		case 1://ÿ�����ߺ��ִ��һ�ι��ߣ�������ߺ����ź�û�и�Ӧ
			if(AxSta(AxBelt[i]) == AXSTA_READY)
			{
				if(I_Get(I_BeltSupply[i]) == ON)
				{
					my.execute = 0;
					my.step = 0;
				}
				else
				{
					TimerRst(&Tm[i]);
					Q_Set(Q_BeltSupply[i],ON);//���ߵ����
					AxRelUnit(AxBelt[i],100,999999);
					my.count = 0;
					my.step = 2;
				}
			}
			break;
			
		case 2://�����¸�Ӧ���ź�
			if(I_Get(I_BeltSupply[i]) == ON)
			{
				AxStop(AxBelt[i]);
				Q_Set(Q_BeltSupply[i],OFF);//�رչ��ߵ��
				my.execute = 0;
				my.step = 0;
			}
//			else if(TimerCnt(&Tm[i],1) >= 5000 && GCFG.PullAlmEn == 1)//������ߵ�����г�������ʱ����
//			{
//				AxStop(AxBelt[i]);
//				Q_Set(Q_BeltSupply[i],OFF);//�رչ��ߵ��
//				AlarmSetBit(2,0,2+i);//���������ʹ����̫��
//				my.execute = 0;
//				my.step = 0;
//			}
			break;
	}
}

//���ϴ���
void UnloadHit(LogicParaDef *p)
{
	static u32 Tm;
	static TrigDef Tr;
	if(TrigOne(&Tr,I_Get(I_ExitCheck) == OFF && (FSM_STA == RUN || FSM_STA == D_RESET) && (GUR.HitReady == 1 || GCFG.HitMode != 0), 20))//������ͨ�����ϸ�Ӧ
	{
		my.execute = 1;//ִ��һ�δ���
	}
	TASK_PROCESS
	{
		case 1:
			TimerRst(&Tm);
			my.step = 2;
			break;
		
		case 2://�����£���ʱ
			if(TimerCnt(&Tm,1) >= GDLY.UnloadInp)
			{
				if(GCFG.HitMode == 0)
					AxRelUnit(AxHit,100,GPOS.UnloadDown);
				Q_Set(Q_UnloadCyd,ON);
				TimerRst(&Tm);
				my.step = 3;
			}
			break;
		
		case 3:////��ʱ����������
			if(TimerCnt(&Tm,1) >= GDLY.UnloadOnDelay ) //&& AxSta(AxHit) == AXSTA_READY)
			{
				Q_Set(Q_UnloadCyd,OFF);
				TimerRst(&Tm);
				my.step = 4;
			}
			break;
			
		case 4://��ʱ���������������ִ��һ��Ƥ��
			if(TimerCnt(&Tm,1) >= GDLY.UnloadOffDelay && (AxSta(AxHit) == AXSTA_READY || GCFG.HitMode != 0))
			{
				my.execute = 0;
				my.step = 0;
				my.count ++;
				if(my.count >= GCFG.UnloadPackNum)//�������ڴ������
				{
					my.count = 0;
					if(GCFG.UnloadPackNum > 0)
					{
						LT.UnloadBeltTask.execute = 1;//ִ��һ��Ƥ����
					}
				}
			}
			break;
	}
}

//����Ƥ��
void UnloadBelt(LogicParaDef *p)
{
	static u32 tm;
	TASK_PROCESS
	{
		case 1://��ʱ����
			TimerRst(&tm);
			my.step = 2;
			break;
		
		case 2://����Ƥ��
			if(TimerCnt(&tm,1) > GDLY.UnloadBeltDelay)
			{
				Q_Set(Q_CollectMotor,ON);
				TimerRst(&tm);
				my.step = 3;
			}
			break;
		
		case 3://��ʱ��ر�
			if(TimerCnt(&tm,1) > GDLY.UnloadBeltTime)
			{
				Q_Set(Q_CollectMotor,OFF);
				my.execute = 0;
				my.step = 0;
			}
			break;
	}
}

//�����+Ԥ��λ
void AxGohomeReady(LogicParaDef *p,u32 i)
{
	TASK_PROCESS
	{
		case 1:
			AxGohome(i);
			my.step = 2;
			break;
		
		case 2:
			if(AxSta(i) == AXSTA_READY)
			{
				switch(i)
				{
					case AxFlow:
						AxAbsUnit(i,10,GPOS.FlowReady);
						break;
					
					case AxPickZ:
						AxAbsUnit(i,10,GPOS.PickZReady);
						break;
					
					case AxPull:
						AxAbsUnit(i,10,GPOS.PullBack);
						break;
					
					case AxWeld:
						AxAbsUnit(i,10,GPOS.WeldReady);
						break;
					
					case AxPickLF:
						AxAbsUnit(i,10,GPOS.PickTakeLF);
						break;
					
					case AxPickRF:
						AxAbsUnit(i,10,GPOS.PickTakeRF);
						break;
					
					case AxPickLB:
						AxAbsUnit(i,10,GPOS.PickTakeLB);
						break;
					
					case AxPickRB:
						AxAbsUnit(i,10,GPOS.PickTakeRB);
						break;
					
					case AxHit:
						AxAbsUnit(i,10,GPOS.UnloadUp);
						break;
				}
				my.step = 3;
			}
			break;
			
		case 3:
			if(AxSta(i) == AXSTA_READY)
			{
				my.execute = 0;
				my.step = 0;
			}
			break;
	}
}

//��Ӧ�õ�������
void AxisDataConfig()
{
	GSS.axis[0].Axhomecfg.orglev = ON;
	GSS.axis[1].Axhomecfg.orglev = ON;
	GSS.axis[2].Axhomecfg.orglev = ON;
	GSS.axis[3].Axhomecfg.orglev = ON;
	GSS.axis[4].Axhomecfg.orglev = ON;
	GSS.axis[5].Axhomecfg.orglev = ON;
	GSS.axis[6].Axhomecfg.orglev = ON;
	GSS.axis[7].Axhomecfg.orglev = ON;
	GSS.axis[8].Axhomecfg.orglev = ON;
	GSS.axis[9].Axhomecfg.orglev = ON;
	GSS.axis[10].Axhomecfg.orglev = ON;
	GSS.axis[11].Axhomecfg.orglev = ON;
	
	GUS.AxisCfg.Data.AccTime[AxBelt_2] = GUS.AxisCfg.Data.AccTime[AxBelt_1];
	GUS.AxisCfg.Data.Direction[AxBelt_2] = GUS.AxisCfg.Data.Direction[AxBelt_1];
	GUS.AxisCfg.Data.HomeSpd[AxBelt_2] = GUS.AxisCfg.Data.HomeSpd[AxBelt_1];
	GUS.AxisCfg.Data.RunSpd[AxBelt_2] = GUS.AxisCfg.Data.RunSpd[AxBelt_1];
	GUS.AxisCfg.Data.StartSpd[AxBelt_2] = GUS.AxisCfg.Data.StartSpd[AxBelt_1];
	for(int i=0;i<PULS_NUM;i++)
	{
		GSS.axis[i].Axconver.MPR = 1.0f;
		GSS.axis[i].Axconver.PPR = 1;
		GSS.axis[i].Axhomecfg.homemode = 0;
		GSS.axis[i].Axhomecfg.homespeedoffset = 0;
		GSS.axis[i].Axhomecfg.homespeedslow = 1000;
		GSS.axis[i].Axhomecfg.orgnum = i;
        GSS.axis[i].Axlimitcfg.alarmmode = 2;
        GSS.axis[i].Axlimitcfg.limitMode = 0;
		GSS.axis[i].AxSpd.acctime = GUS.AxisCfg.Data.AccTime[i];
        GSS.axis[i].AxSpd.dectime = GUS.AxisCfg.Data.AccTime[i];
		GSS.axis[i].Axhomecfg.homespeedfast = GUS.AxisCfg.Data.HomeSpd[i];
		GSS.axis[i].AxSpd.startspeed = GUS.AxisCfg.Data.StartSpd[i];
        GSS.axis[i].AxSpd.endspeed = GUS.AxisCfg.Data.StartSpd[i];
		
		if(GSW.JogForward[0] == 0 && GSW.JogBackward[0] == 0)
		{
			GSS.axis[i].AxSpd.runspeed = GUS.AxisCfg.Data.RunSpd[i];
		}
		else
		{
			GSS.axis[AxFlow].AxSpd.runspeed = 500;
			GSS.axis[AxPickZ].AxSpd.runspeed = 500;
			GSS.axis[AxPull].AxSpd.runspeed = 500;
			GSS.axis[AxPickLB].AxSpd.runspeed = 200;
			GSS.axis[AxPickLF].AxSpd.runspeed = 200;
			GSS.axis[AxPickRB].AxSpd.runspeed = 200;
			GSS.axis[AxPickRF].AxSpd.runspeed = 200;
			GSS.axis[AxWeld].AxSpd.runspeed = 500;
			GSS.axis[AxHit].AxSpd.runspeed = 500;
			GSS.axis[AxBelt_1].AxSpd.runspeed = 500;
			GSS.axis[AxBelt_2].AxSpd.runspeed = 500;
		}
		if(i == AxFlow)
		{
			GSS.axis[i].Axhomecfg.homespeedslow = GSS.axis[i].Axhomecfg.homespeedfast;
		}
		if(i == AxHit)
		{
			GSS.axis[i].Axhomecfg.orgnum = 20;
		}
		AxDirCfg(i,GUS.AxisCfg.Data.Direction[i]);
		AxisHomeCfg(i,GSS.axis[i].Axhomecfg);
	}
}

//���豸�İ�ť����
LogicParaDef DeviceBtnTask;
void DeviceBtn()
{
	static TrigDef Tr_Start,Tr_Stop,Tr_Scram,Tr_Reset;
	static u32 Tm_Scram;
	LogicParaDef *p = &DeviceBtnTask;    //����ôһ��Ȧ����ͼ��ʲô
	//��λǰ����ťִ�и�λ����λ�󣬰�ť����
	if(TrigOne(&Tr_Start,I_Get(I_Start) == ON,50))
	{
		GUW.HmiStart = 1;  //Hmi �˻�����
	}
	//��λǰ����ťִ�и�λ����λ�󣬰�ťֹͣ
	if(TrigOne(&Tr_Stop,I_Get(I_Stop) == ON,50))
	{
		FSM_CMD = STOP;
	}
	//��λ��ť
	if(TrigOne(&Tr_Reset,I_Get(I_Reset) == ON,50))
	{
		FSM_CMD = D_RESET;
	}
	//��ͣ����
	if(TrigOne(&Tr_Scram,I_Get(I_Scram) == ON,50))
	{
		FSM_CMD = STOP;
	}
	//��ͣ����
	if(TimerCnt(&Tm_Scram,I_Get(I_Scram) == OFF) > 50)
	{
		GUR.HaveToReset = 1;
		for(int i=0; i<PULS_NUM; i++)
		{
			AxStop(i);
		}
		FSM_CMD = SCRAM;
	}
	//������������ť����Ҫʱ�ȸ�λ������
	if(GUW.HmiStart)
	{
		GUW.HmiStart = 0;
		my.execute = 1;
	}
	TASK_PROCESS
	{
		case 1:
			if(GUR.HaveToReset)
			{
				FSM_CMD = D_RESET;
			}
			my.step = 2;
			break;
			
		case 2:
			if(FSM_STA == STOP)
			{
				my.execute = 0;
				my.step = 0;
				if(GSR.ErrorLevel == 0)
				{
					FSM_CMD = RUN;
				}
			}
			break;
	}
}
//���豸�㶯����
void DeviceJogSet()
{
	GSW.jogSpd = 100;
	if(GUW.JogFeed == 0)
	{
		GSW.JogMode = 2;
	}
	else
	{
		GSW.JogMode = 1;
		switch(GUW.JogFeed)
		{
			case 1:
				GSW.JogPosition = 1;
				break;
			
			case 2:
				GSW.JogPosition = 5;
				break;
			
			case 3:
				GSW.JogPosition = 10;
				break;
			
			case 4:
				GSW.JogPosition = 50;
				break;
		}
	}
}
//��ͣ��ִ��һ��
void scram_handle()          //�ڲ���Ҫ��ʼ����һЩ���� + ������ֹͣ
{
	GUR.HaveToReset = 1;    //��ͣ���ٴ����������ȸ�λ
	Q_Set(Q_MaskSupply,OFF);
	Q_Set(Q_ExitMotor,OFF);
	PARAINIT(LT);           //���³�ʼ�� LogicTask����Task�� ��ֵд0   ������ͬTask LogicTask = { 0 }; //ͳһ��������  
	for(int i=0; i<PULS_NUM; i++)
	{
		AxStop(i);
	}
}

//������ִ��һ��
void start_handle()
{
	PARAINIT(DeveceUph);   //��ʼ����һЩ  ��¼�Բ���
}
//ֹͣ��ִ��һ��
void stop_handle()
{
	Q_Set(Q_MaskSupply,OFF);
	Q_Set(Q_ExitMotor,OFF);
}

//2020.7.20
//�豸�߼�ѭ��
void Logic()
{
	AxisDataConfig();				//����Ŀ�õ������÷�ʽ
	DeviceJogSet();					//����Ŀ�㶯����
	DeviceBtn();					//���豸�İ�ť����
	
    /******���������߼�*******/
	DeviceMain(&LT.DeviceMainTask);			//�豸������
	Pick(&LT.PickTask);						//ȡ�ߣ�ȡ���õ��ߣ�����λ�Ϸ��ȴ�
	Weld(&LT.WeldTask);						//����
	Pull(&LT.PullTask);						//����
	BeltSupply(&LT.BeltSupplyTask[0],0);	//������
	BeltSupply(&LT.BeltSupplyTask[1],1);	//������
	UnloadHit(&LT.UnloadHitTask);			//���ϴ���
	UnloadBelt(&LT.UnloadBeltTask);			//����Ƥ��
	for(int i=0; i<PULS_NUM; i++)
	{
		AxGohomeReady(&LT.AxGohomeReady[i],i);
	}
}

