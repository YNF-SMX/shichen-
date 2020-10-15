#include "curve.h"

CurveParaDef CurvePara[PULS_NUM] = {0};

/*********************************************************************************************************************/
//S型离散型 加速度递增 脉冲计算()
PulsSpdParaDef Disc_AcceIncrPulsCalc(u32 Vinit,u32 Acce,u32 Jerk)
{
	u32 Acct;
	PulsSpdParaDef PulsSpdPara;	
	PulsSpdPara.Spd = Vinit;
	PulsSpdPara.Puls = (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
	Acct = (int)(Jerk*ST+0.5f);
	while(Acct+(int)(Jerk*ST*0.5f+0.5f)<Acce)
	{
		PulsSpdPara.Spd += (int)(Acct*ST/1000.0f+0.5f);
		PulsSpdPara.Puls += (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
		Acct += (int)(Jerk*ST*1.0f+0.5f);		
	}
	return PulsSpdPara;
}

//S型离散型匀加速脉冲计算()
PulsSpdParaDef Disc_AcceUnifPulsCalc(u32 Vinit,u32 Vtar,u32 Acce)
{
	PulsSpdParaDef PulsSpdPara;	 
	
    PulsSpdPara.Spd = Vinit + (int)(Acce*ST/1000.0f+0.5f);
    PulsSpdPara.Puls = (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
    while(PulsSpdPara.Spd + (int)(Acce*ST/2000.0f+0.5f) < Vtar)
	{
        PulsSpdPara.Spd += (int)(Acce*ST/1000.0f+0.5f);
        PulsSpdPara.Puls += (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
    }
    return PulsSpdPara;
}

//S型离散型 加速度递减 脉冲计算()
PulsSpdParaDef Disc_AcceDecrPulsCalc(u32 Vinit,u32 Vtar,u32 Acce,u32 Jerk)
{
	s32 Acct;
	PulsSpdParaDef PulsSpdPara;	
	Acct = (int)(Acce-Jerk*ST+0.5f);
	PulsSpdPara.Spd = Vinit + (int)(Acct*ST/1000.0f+0.5f);
	PulsSpdPara.Puls = (int)(PulsSpdPara.Spd*ST/1000.0f + 0.5f);
	Acct = (int)(Acct-Jerk*ST+0.5f);
	//while( PulsSpdPara.Spd + (int)(1.5f*Jerk*ST*ST/1000.0f+0.5f) < Vtar)
	while(Acct>(int)(1.5f*Jerk*ST+0.5f))
	{
		PulsSpdPara.Spd += (int)(Acct*ST/1000.0f+0.5f);
		PulsSpdPara.Puls += (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
		Acct = (int)(Acct-Jerk*ST+0.5f);		
	}
	return PulsSpdPara;
}

//S型离散型减速递增脉冲计算()
PulsSpdParaDef Disc_DeceIncrPulsCalc(u32 Vinit,u32 Dece,u32 Djerk)
{
	u32 Dect;
	PulsSpdParaDef PulsSpdPara;
	Dect = (int)(Djerk*ST+0.5f);
	PulsSpdPara.Spd = (int)(Vinit-Dect*ST/1000.0f+0.5f);  
	PulsSpdPara.Puls = (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
	Dect += (int)(Djerk*ST+0.5f); 
	while(Dect + (int)(Djerk*ST*0.5f+0.5f) < Dece)
	{
		PulsSpdPara.Spd = (int)(PulsSpdPara.Spd-Dect*ST/1000.0f+0.5f);
		PulsSpdPara.Puls += (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
		Dect += (int)(Djerk*ST+0.5f);
	}
	return PulsSpdPara;
}

//S型离散型匀减速脉冲计算()
PulsSpdParaDef Disc_DeceUnifPulsCalc(u32 Vinit,u32 Vend,u32 Dece)
{	
	PulsSpdParaDef PulsSpdPara;
	PulsSpdPara.Spd = (int)(Vinit-Dece*ST/1000.0f+0.5f);
	PulsSpdPara.Puls = (int)(PulsSpdPara.Spd*ST/1000.0f + 0.5f);
	//while((int)(PulsSpdPara.Spd - Dece*ST/2000.0f +0.5f) > Vend)
	while(PulsSpdPara.Spd>(int)(Vend + Dece*ST/2000.0f +0.5f))
	{
		PulsSpdPara.Spd = (int)(PulsSpdPara.Spd-Dece*ST/1000.0f+0.5f);
		PulsSpdPara.Puls += (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
	}
	return PulsSpdPara;
}

//离散型减速递减脉冲计算()
PulsSpdParaDef Disc_DeceDecrPulsCalc(u32 Vinit,u32 Vend,u32 Dece,u32 Djerk)
{
	u32 Dect;
	PulsSpdParaDef PulsSpdPara;
	Dect = (int)(Dece - Djerk*ST +0.5f);
	PulsSpdPara.Spd = (int)(Vinit - Dect*ST/1000.0f +0.5f);
	PulsSpdPara.Puls = (int)(PulsSpdPara.Spd*ST/1000.0f + 0.5f);
	Dect = (int)(Dect - Djerk*ST +0.5f);
	//while((int)(PulsSpdPara.Spd - Djerk*ST*ST/2000.0f +0.5f)>Vend)
	while(Dect>(int)(Djerk*ST/2.0f+0.5f))
	{
		PulsSpdPara.Spd = (int)(PulsSpdPara.Spd - Dect*ST/1000.0f +0.5f);
		if(PulsSpdPara.Spd<Vend||PulsSpdPara.Spd>0x7000000)
			PulsSpdPara.Spd = Vend;
		PulsSpdPara.Puls += (int)(PulsSpdPara.Spd*ST/1000.0f + 0.5f);
		Dect = (int)(Dect - Djerk*ST +0.5f);
	}
	return PulsSpdPara;
}

//T型离散型匀加速脉冲计算
PulsSpdParaDef Disc_AccePulsCalc(u32 Vinit,u32 Vtar,u32 Acce)
{
	PulsSpdParaDef PulsSpdPara;
    PulsSpdPara.Spd = Vinit;
    PulsSpdPara.Puls = (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
	if(Acce>1000.0f/ST)
	{
		while(PulsSpdPara.Spd+(int)(1.5f*Acce*ST/1000.0f+0.5f)<Vtar)
		{
			PulsSpdPara.Spd +=(int)(Acce*ST/1000.0f+0.5f);
			PulsSpdPara.Puls += (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
		}
	}
	else//匀速
	{
		PulsSpdPara.Puls = 0;		
	}
	return PulsSpdPara;
}

//T型离散型匀减速脉冲计算
PulsSpdParaDef Disc_DecePulsCalc(u32 Vtar,u32 Vend,u32 Dece)
{
    PulsSpdParaDef PulsSpdPara;
	PulsSpdPara.Spd = (int)(Vtar-Dece*ST/1000.0f+0.5f);
	PulsSpdPara.Puls = (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
	if(Dece>1000.0f/ST)
	{
		while(PulsSpdPara.Spd>(Vend+(int)(Dece*ST/2000.0f+0.5f)))
		{
			PulsSpdPara.Spd = (int)(PulsSpdPara.Spd - Dece*ST/1000.0f+0.5f);
			if(PulsSpdPara.Spd<Vend||PulsSpdPara.Spd>0x70000000)//防止数据越界
				PulsSpdPara.Spd = Vend; 
			PulsSpdPara.Puls += (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
		}
	}
	else
	{
		PulsSpdPara.Spd = Vend;
		PulsSpdPara.Puls = 0;
	}
	return PulsSpdPara;
}

//T型加减速重新分配运行速度
PulsSpdParaDef PulseToSpd(u32 Vinit,u32 Acce,u32 Puls)
{
    PulsSpdParaDef PulsSpdPara;
    PulsSpdPara.Spd = Vinit;
    PulsSpdPara.Puls = (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
    while(PulsSpdPara.Puls<Puls)
    {
        PulsSpdPara.Spd += (int)(Acce*ST/1000.0f+0.5f);   
        PulsSpdPara.Puls += (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
    }
	PulsSpdPara.Puls = (int)(PulsSpdPara.Puls - PulsSpdPara.Spd*ST/1000.0f+0.5f);
    return PulsSpdPara;
}

//S型加速度递增重新分配运行速度
ASPParaDef p2as(u32 Puls,u32 Vinit,u32 Jerk)
{
	ASPParaDef ASPPara;
	ASPPara.Spd = Vinit;
	ASPPara.Puls = (int)(ASPPara.Spd*ST/1000.0f+0.5f);
	ASPPara.Acc = (int)(Jerk*ST+0.5f);
	while(ASPPara.Puls<Puls)
	{
		ASPPara.Spd += (int)(ASPPara.Acc*ST/1000.0f+0.5f);
		ASPPara.Puls += (int)(ASPPara.Spd*ST/1000.0f+0.5f);
		ASPPara.Acc += (int)(Jerk*ST+0.5f);		
	}
	return ASPPara;
}

//计算不同速度状态的脉冲个数
void CalcPulse(u8 AxNum)
{
    u8 i; 
    PulsSpdParaDef PulsSpdPara_1;   
    for(i=0;i<SPDSECTION;i++)
    {
        CurvePara[AxNum].PulsPara.Calc.Para[i] = 0;
    }  
    PULS_TAR(AxNum)= Puls_GetRun(AxNum);   
    if(CURVE_MODE(AxNum)==TCURVE)
    {
        if(SPD_TACC(AxNum) && SPD_TDEC(AxNum))
        {
            SPD_ACCE(AxNum) = 1000.0f*(SPD_TAR(AxNum)-SPD_INIT(AxNum))/SPD_TACC(AxNum);
            PulsSpdPara_1 = Disc_AccePulsCalc(SPD_INIT(AxNum),SPD_TAR(AxNum),SPD_ACCE(AxNum));
			PULS_ACCE_UNIF(AxNum) = PulsSpdPara_1.Puls;//计算出加速段脉冲数
			
			SPD_DECE(AxNum) = 1000.0f*(SPD_TAR(AxNum)-SPD_END(AxNum))/SPD_TDEC(AxNum);
			PulsSpdPara_1 = Disc_DecePulsCalc(SPD_TAR(AxNum),SPD_END(AxNum),SPD_DECE(AxNum));  
            PULS_DECE_UNIF(AxNum) = PulsSpdPara_1.Puls;//计算出减速段脉冲数    
        }
       
        if(PULS_ACCE_UNIF(AxNum)+PULS_DECE_UNIF(AxNum)<=CurvePara[AxNum].PulsPara.Tar)//3段  
        {            
            PULS_ACCE_ZREO(AxNum) = PULS_TAR(AxNum) - PULS_ACCE_UNIF(AxNum) - PULS_DECE_UNIF(AxNum); //匀速段脉冲数
        }            
        else//新3段
        {//速度无法加到最大速度,重新计算加速段,减速段
			PulsSpdPara_1 = PulseToSpd(SPD_INIT(AxNum),SPD_ACCE(AxNum),(1.0f-UNIFRATIO)*CurvePara[AxNum].PulsPara.Tar*SPD_TACC(AxNum)/(SPD_TACC(AxNum)+SPD_TDEC(AxNum)));
			SPD_TAR(AxNum) = PulsSpdPara_1.Spd;
			PULS_ACCE_UNIF(AxNum) = PulsSpdPara_1.Puls;
			PulsSpdPara_1 = Disc_DecePulsCalc(SPD_TAR(AxNum),SPD_END(AxNum),SPD_DECE(AxNum));
			PULS_DECE_UNIF(AxNum) = PulsSpdPara_1.Puls;				
			if(PULS_ACCE_UNIF(AxNum)+PULS_DECE_UNIF(AxNum)<=CurvePara[AxNum].PulsPara.Tar)           
				PULS_ACCE_ZREO(AxNum) = PULS_TAR(AxNum) - PULS_ACCE_UNIF(AxNum) - PULS_DECE_UNIF(AxNum); 
			else           
				PULS_ACCE_ZREO(AxNum) = 0;                      
        }              
    }
    else if(CURVE_MODE(AxNum)==SCURVE)
    {
        if(SPD_TACC(AxNum)&&SPD_TDEC(AxNum))
 		{
			SPD_TAA(AxNum) = SRATIO*SPD_TACC(AxNum)/1000.0f;
			SPD_ACCE(AxNum) = (int)(1000.0f*(SPD_TAR(AxNum)-SPD_INIT(AxNum))/((1-SRATIO)*SPD_TACC(AxNum)));
			SPD_JERK(AxNum) = (int)(SPD_ACCE(AxNum)/(1000.0f*SPD_TAA(AxNum)) + 0.5f);
			
			SPD_TDD(AxNum) = SRATIO*SPD_TDEC(AxNum)/1000.0f;
			SPD_DECE(AxNum) = (int)(1000.0f*(SPD_TAR(AxNum)-SPD_END(AxNum))/((1-SRATIO)*SPD_TDEC(AxNum)));
			SPD_DJERK(AxNum) = (int)(SPD_DECE(AxNum)/(1000.0f*SPD_TDD(AxNum)) + 0.5f);
			//加速度递增
			PulsSpdPara_1 = Disc_AcceIncrPulsCalc(SPD_INIT(AxNum),SPD_ACCE(AxNum),SPD_JERK(AxNum));
			SPD_VACCE1(AxNum) = PulsSpdPara_1.Spd;
			PULS_ACCE_INCR(AxNum) = PulsSpdPara_1.Puls;
			//匀加速
			SPD_VACCE2(AxNum) = SPD_TAR(AxNum) + SPD_INIT(AxNum) - SPD_VACCE1(AxNum);
			PulsSpdPara_1 = Disc_AcceUnifPulsCalc(SPD_VACCE1(AxNum),SPD_VACCE2(AxNum),SPD_ACCE(AxNum));
			SPD_VACCE2(AxNum) = PulsSpdPara_1.Spd;
			PULS_ACCE_UNIF(AxNum) = PulsSpdPara_1.Puls;
			//加速度递减
			PulsSpdPara_1 = Disc_AcceDecrPulsCalc(SPD_VACCE2(AxNum),SPD_TAR(AxNum),SPD_ACCE(AxNum),SPD_JERK(AxNum));
			PULS_ACCE_DECR(AxNum) = PulsSpdPara_1.Puls;
			//减速度递增
			PulsSpdPara_1 = Disc_DeceIncrPulsCalc(SPD_TAR(AxNum),SPD_DECE(AxNum),SPD_DJERK(AxNum));
			SPD_VDECE1(AxNum) = PulsSpdPara_1.Spd;
			PULS_DECE_INCR(AxNum) = PulsSpdPara_1.Puls;
			//匀减速
			SPD_VDECE2(AxNum) = SPD_TAR(AxNum) + SPD_END(AxNum) - SPD_VDECE1(AxNum);
			PulsSpdPara_1 = Disc_DeceUnifPulsCalc(SPD_VDECE1(AxNum),SPD_VDECE2(AxNum),SPD_DECE(AxNum));
			SPD_VDECE2(AxNum) = PulsSpdPara_1.Spd;
			PULS_DECE_UNIF(AxNum) = PulsSpdPara_1.Puls;
			//减速度递减
			PulsSpdPara_1 = Disc_DeceDecrPulsCalc(SPD_VDECE2(AxNum),SPD_END(AxNum),SPD_DECE(AxNum),SPD_DJERK(AxNum));
			PULS_DECE_DECR(AxNum) = PulsSpdPara_1.Puls;
			
			if((PULS_ACCE_INCR(AxNum)+PULS_ACCE_UNIF(AxNum)+PULS_ACCE_DECR(AxNum)
			   +PULS_DECE_INCR(AxNum)+PULS_DECE_UNIF(AxNum) +PULS_DECE_DECR(AxNum))<PULS_TAR(AxNum))//7段
			{
				PULS_ACCE_ZREO(AxNum) = PULS_TAR(AxNum) - (PULS_ACCE_INCR(AxNum)+PULS_ACCE_UNIF(AxNum)+PULS_ACCE_DECR(AxNum)
															+PULS_DECE_INCR(AxNum)+PULS_DECE_UNIF(AxNum)+PULS_DECE_DECR(AxNum));
			}
			else
			{
				PULS_ACCE_UNIF(AxNum) = 0;
				PULS_DECE_UNIF(AxNum) = 0;
				PulsSpdPara_1.Puls = PULS_TAR(AxNum)*SPD_TACC(AxNum)/(SPD_TACC(AxNum)+SPD_TDEC(AxNum));
				if(SPD_INIT(AxNum)>(int)(1000.0f*PulsSpdPara_1.Puls/(float)SPD_TACC(AxNum)+0.5f))
					SPD_TAR(AxNum) = SPD_INIT(AxNum);
				else
					SPD_TAR(AxNum) = (int)(2000.0f*PulsSpdPara_1.Puls/(float)SPD_TACC(AxNum)-SPD_INIT(AxNum)+0.5f);
				if(SPD_TAR(AxNum)==SPD_INIT(AxNum))
				{//匀速完成
					for(i=0;i<SPDSECTION;i++)
					{
						CurvePara[AxNum].PulsPara.Calc.Para[i] = 0;
					}
					PULS_ACCE_ZREO(AxNum) = 0;				
				}
				else
				{
					SPD_JERK(AxNum) = 400.0f*(SPD_TAR(AxNum)-SPD_INIT(AxNum))/(float)SPD_TACC(AxNum);
					SPD_ACCE(AxNum) = SPD_JERK(AxNum)*SPD_TACC(AxNum)/2;
					
					SPD_DJERK(AxNum) = SPD_JERK(AxNum)*(float)(SPD_TACC(AxNum)/(float)SPD_TDEC(AxNum))*(float)(SPD_TACC(AxNum)/(float)SPD_TDEC(AxNum));
					SPD_DECE(AxNum) = SPD_DJERK(AxNum)*SPD_TDEC(AxNum)/2;
					
					PULS_ACCE_UNIF(AxNum) = 0;
					PULS_DECE_UNIF(AxNum) = 0;
					PulsSpdPara_1 = Disc_AcceIncrPulsCalc(SPD_INIT(AxNum),SPD_ACCE(AxNum),SPD_JERK(AxNum));
					SPD_VACCE1(AxNum) = PulsSpdPara_1.Spd;
					PULS_ACCE_INCR(AxNum) = PulsSpdPara_1.Puls;		

					PulsSpdPara_1 = Disc_AcceDecrPulsCalc(SPD_VACCE1(AxNum),SPD_TAR(AxNum),SPD_ACCE(AxNum),SPD_JERK(AxNum));
					//SPD_TAR(AxNum) = PulsSpdPara_1.Spd;
					PULS_ACCE_DECR(AxNum) = PulsSpdPara_1.Puls;	 
					
					PulsSpdPara_1 = Disc_DeceIncrPulsCalc(SPD_TAR(AxNum),SPD_DECE(AxNum),SPD_DJERK(AxNum));
					SPD_VDECE1(AxNum) = PulsSpdPara_1.Spd;
					PULS_DECE_INCR(AxNum) = PulsSpdPara_1.Puls;
					
					PulsSpdPara_1 = Disc_DeceDecrPulsCalc(SPD_VDECE1(AxNum),SPD_END(AxNum),SPD_DECE(AxNum),SPD_DJERK(AxNum));
					PULS_DECE_DECR(AxNum) = PulsSpdPara_1.Puls;
					
					if(PULS_TAR(AxNum)>(PULS_ACCE_INCR(AxNum)+PULS_ACCE_DECR(AxNum)+PULS_DECE_INCR(AxNum)+PULS_DECE_DECR(AxNum)))
					{
						PULS_ACCE_ZREO(AxNum) = PULS_TAR(AxNum)-(PULS_ACCE_INCR(AxNum)+PULS_ACCE_DECR(AxNum)+PULS_DECE_INCR(AxNum)+PULS_DECE_DECR(AxNum));
					}
				}					
			}
		}			
    }   
}

//输出速度初始化
void SpdInit(u8 AxNum)
{ 
    if(CURVE_MODE(AxNum)==TCURVE)   //T型函数初始化
    {
        SPD_CUR(AxNum) = SPD_INIT(AxNum);
        PULS_NEXT(AxNum) = (int)(SPD_CUR(AxNum)*ST/1000.0f+0.5f);
        CURVE_STATE(AxNum) = ACCE_UNIF; 
    }
    else    //S型函数初始化
    {
        SPD_CUR(AxNum) = SPD_INIT(AxNum);
        PULS_NEXT(AxNum) = (int)(SPD_CUR(AxNum)*ST/1000.0f+0.5f);
        SPD_ACCEVAR(AxNum) = (int)(SPD_JERK(AxNum)*ST+0.5f);
		CURVE_STATE(AxNum) = ACCE_INCR;
    }
}

u32 CurvesGetSpd(u8 AxNum)
{  
    PULS_CUR(AxNum) = Puls_GetRan(AxNum);  
    if(PULS_CUR(AxNum)==PULS_TAR(AxNum))
    {
        CURVE_STATE(AxNum) = ACCE_IDLE; 
    }   
    if(CURVE_MODE(AxNum)==TCURVE)
    {
        switch(CURVE_STATE(AxNum))
        {                     
            case ACCE_UNIF://匀加速              
                if((PULS_CUR(AxNum)+PULS_ACCE_ZREO(AxNum)+PULS_DECE_UNIF(AxNum))<=PULS_TAR(AxNum))
                {
                    if(PULS_CUR(AxNum)>=PULS_NEXT(AxNum))
                    {
                        SPD_CUR(AxNum) += (int)(SPD_ACCE(AxNum)*ST/1000.0f+0.5f);
                        SPD_CUR(AxNum) = SPD_CUR(AxNum)<SPD_TAR(AxNum)?SPD_CUR(AxNum):SPD_TAR(AxNum);
                        PULS_NEXT(AxNum) = PULS_NEXT(AxNum) + (int)(SPD_CUR(AxNum)*ST/1000.0f+0.5f);
                    }
                }
                else
                {
                    if(PULS_ACCE_ZREO(AxNum)) 
					{						
                        SPD_CUR(AxNum) = SPD_TAR(AxNum);
						CURVE_STATE(AxNum) = ACCE_ZERO; 
					}						                  
                }               
                break;

            case ACCE_ZERO:              
                if((PULS_CUR(AxNum)+PULS_DECE_UNIF(AxNum))>=PULS_TAR(AxNum))
                {                 
                    PULS_NEXT(AxNum) = PULS_ACCE_UNIF(AxNum) + PULS_ACCE_ZREO(AxNum);
					SPD_CUR(AxNum) = (int)(SPD_CUR(AxNum)-(SPD_DECE(AxNum)*ST/1000.0f+0.5f));
                    PULS_NEXT(AxNum) = PULS_NEXT(AxNum) + (int)(SPD_CUR(AxNum)*ST/1000.0f+0.5f);                  
                    CURVE_STATE(AxNum) = DECE_UNIF;                  
                }               
                break;

            case DECE_UNIF:
                if(PULS_CUR(AxNum)>=PULS_NEXT(AxNum))
                {
                    SPD_CUR(AxNum) = (int)(SPD_CUR(AxNum)-(SPD_DECE(AxNum)*ST/1000.0f+0.5f));
                    SPD_CUR(AxNum) = SPD_CUR(AxNum)>SPD_END(AxNum)?SPD_CUR(AxNum):SPD_END(AxNum);
                    PULS_NEXT(AxNum) = PULS_NEXT(AxNum) + (int)(SPD_CUR(AxNum)*ST/1000.0f+0.5f);                         
                }
                break;
        } 
    }
    else
    {           
		switch(CURVE_STATE(AxNum))
        { 
			case ACCE_INCR://加加速
				if(PULS_CUR(AxNum)<PULS_ACCE_INCR(AxNum))
				{
					if(PULS_CUR(AxNum)>=PULS_NEXT(AxNum))
                    {
						SPD_CUR(AxNum) += (int)(SPD_ACCEVAR(AxNum)*ST/1000.0f+0.5f);
						PULS_NEXT(AxNum) += (int)(SPD_CUR(AxNum)*ST/1000.0f+0.5f);
						SPD_ACCEVAR(AxNum) += (int)(SPD_JERK(AxNum)*ST*1.0f+0.5f);							
                    }
				}
				else
				{
					CURVE_STATE(AxNum) = ACCE_UNIF;					
				}
                break;
            
            case ACCE_UNIF://匀加速
                if(PULS_CUR(AxNum)<PULS_ACCE_INCR(AxNum)+PULS_ACCE_UNIF(AxNum))
				{
					if(PULS_CUR(AxNum)>=PULS_NEXT(AxNum))
					{
						SPD_CUR(AxNum) += (int)(SPD_ACCE(AxNum)*ST/1000.0f+0.5f);					
						SPD_CUR(AxNum) = SPD_CUR(AxNum)<SPD_TAR(AxNum)?SPD_CUR(AxNum):SPD_TAR(AxNum);
						PULS_NEXT(AxNum) += (int)(SPD_CUR(AxNum)*ST/1000.0f+0.5f);					
					}
				}
				else
				{
					SPD_ACCEVAR(AxNum) = (int)(SPD_ACCE(AxNum)-SPD_JERK(AxNum)*ST*1.0f+0.5f);
					if(PULS_ACCE_UNIF(AxNum))
						SPD_ACCEVAR(AxNum) = (int)(SPD_ACCE(AxNum)-SPD_JERK(AxNum)*ST*1.0f+0.5f);
					else
						SPD_ACCEVAR(AxNum) = SPD_ACCE(AxNum);
					CURVE_STATE(AxNum) = ACCE_DECR;
				}
                break;
            
            case ACCE_DECR://减加速
                if(PULS_CUR(AxNum)<PULS_ACCE_INCR(AxNum)+PULS_ACCE_UNIF(AxNum)+PULS_ACCE_DECR(AxNum))
				{
					if(PULS_CUR(AxNum)>=PULS_NEXT(AxNum))
					{
						SPD_CUR(AxNum) += (int)(SPD_ACCEVAR(AxNum)*ST/1000.0f+0.5f);					
						SPD_CUR(AxNum) = SPD_CUR(AxNum)<SPD_TAR(AxNum)?SPD_CUR(AxNum):SPD_TAR(AxNum);
						PULS_NEXT(AxNum) += (int)(SPD_CUR(AxNum)*ST/1000.0f+0.5f);	
						SPD_ACCEVAR(AxNum) = (int)(SPD_ACCEVAR(AxNum)-SPD_JERK(AxNum)*ST*1.0f+0.5f);
					}
				}
				else
				{
					SPD_CUR(AxNum) = SPD_TAR(AxNum);
					CURVE_STATE(AxNum) = ACCE_ZERO;
				}
                break;
            
            case ACCE_ZERO://匀速
				if(PULS_CUR(AxNum)>=PULS_ACCE_INCR(AxNum)+PULS_ACCE_UNIF(AxNum)+PULS_ACCE_DECR(AxNum)+PULS_ACCE_ZREO(AxNum))
				{
					PULS_NEXT(AxNum) = PULS_ACCE_INCR(AxNum)+PULS_ACCE_UNIF(AxNum)+PULS_ACCE_DECR(AxNum)+PULS_ACCE_ZREO(AxNum);
					SPD_DECEVAR(AxNum) = (int)(SPD_DJERK(AxNum)*ST*1.0f+0.5f);
					SPD_CUR(AxNum) = (int)(SPD_CUR(AxNum)-SPD_DECEVAR(AxNum)*ST/1000.0f+0.5f);
					PULS_NEXT(AxNum) += (int)(SPD_CUR(AxNum)*ST/1000.0f+0.5f); 
					SPD_DECEVAR(AxNum) += (int)(SPD_DJERK(AxNum)*ST*1.0f+0.5f);
					CURVE_STATE(AxNum) = DECE_INCR;
				}
                break;
            
            case DECE_INCR://加减速
				if(PULS_CUR(AxNum)<PULS_ACCE_INCR(AxNum)+PULS_ACCE_UNIF(AxNum)+PULS_ACCE_DECR(AxNum)+PULS_ACCE_ZREO(AxNum)+PULS_DECE_INCR(AxNum))
				{
					if(PULS_CUR(AxNum)>=PULS_NEXT(AxNum))
					{						
						SPD_CUR(AxNum) = (int)(SPD_CUR(AxNum)-SPD_DECEVAR(AxNum)*ST/1000.0f+0.5f);
						PULS_NEXT(AxNum) += (int)(SPD_CUR(AxNum)*ST/1000.0f+0.5f);
						SPD_DECEVAR(AxNum) += (int)(SPD_DJERK(AxNum)*ST*1.0f+0.5f);						
					}
				}
				else
				{
					CURVE_STATE(AxNum) = DECE_UNIF;
				}
                break;
            
            case DECE_UNIF://匀减速
                if(PULS_CUR(AxNum)<PULS_ACCE_INCR(AxNum)+PULS_ACCE_UNIF(AxNum)+PULS_ACCE_DECR(AxNum)
					+PULS_ACCE_ZREO(AxNum)+PULS_DECE_INCR(AxNum)+PULS_DECE_UNIF(AxNum))
				{
					if(PULS_CUR(AxNum)>=PULS_NEXT(AxNum))
					{
						SPD_CUR(AxNum) = (int)(SPD_CUR(AxNum) - SPD_DECE(AxNum)*ST/1000.0f+0.5f);					
						PULS_NEXT(AxNum) += (int)(SPD_CUR(AxNum)*ST/1000.0f+0.5f);	
					}
				}
				else
				{
					if(PULS_DECE_UNIF(AxNum))
						SPD_DECEVAR(AxNum) = (int)(SPD_DECE(AxNum) - SPD_DJERK(AxNum)*ST+0.5f);
					else
						SPD_DECEVAR(AxNum) = SPD_DECE(AxNum);
					CURVE_STATE(AxNum) = DECE_DECR;
				}
                break;
            
            case DECE_DECR://减减速
                if(PULS_CUR(AxNum)<PULS_ACCE_INCR(AxNum)+PULS_ACCE_UNIF(AxNum)+PULS_ACCE_DECR(AxNum)
				+PULS_ACCE_ZREO(AxNum)+PULS_DECE_INCR(AxNum)+PULS_DECE_UNIF(AxNum)+PULS_DECE_DECR(AxNum))
				{
					if(PULS_CUR(AxNum)>=PULS_NEXT(AxNum))
					{
						SPD_CUR(AxNum) = (int)(SPD_CUR(AxNum) - SPD_DECEVAR(AxNum)*ST/1000.0f+0.5f);
						SPD_CUR(AxNum) = SPD_CUR(AxNum)<0x70000000?SPD_CUR(AxNum):SPD_END(AxNum);
						SPD_CUR(AxNum) = SPD_CUR(AxNum)>SPD_END(AxNum)?SPD_CUR(AxNum):SPD_END(AxNum);						
						PULS_NEXT(AxNum) += (int)(SPD_CUR(AxNum)*ST/1000.0f+0.5f);
						if(SPD_DECEVAR(AxNum)>(int)(SPD_DJERK(AxNum)*ST*1.0f+0.5f))
						{
							SPD_DECEVAR(AxNum) = (int)(SPD_DECEVAR(AxNum) - SPD_DJERK(AxNum)*ST*1.0f+0.5f);
						}
						else
						{
							SPD_DECEVAR(AxNum) = 0;
						}
					}
				}
                break;
            
            default:
                break;
		}
    }  
    return SPD_CUR(AxNum);
}


u32 CurvesGetDecSteps(u8 AxNum)
{
    PulsSpdParaDef PulsSpdPara;
	if(SPD_DECE(AxNum)<2000)
		SPD_DECE(AxNum)=2000;
	PulsSpdPara.Spd = (int)(SPD_TAR(AxNum)-SPD_DECE(AxNum)*ST/1000.0f+0.5f);
	PulsSpdPara.Puls = (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
	while(PulsSpdPara.Spd>SPD_END(AxNum)+(int)(SPD_DECE(AxNum)*ST/2000.0f+0.5f))
	{
		PulsSpdPara.Spd = (int)(PulsSpdPara.Spd - SPD_DECE(AxNum)*ST/1000.0f+0.5f);
		if(PulsSpdPara.Spd<SPD_END(AxNum)||PulsSpdPara.Spd>0x70000000)
			PulsSpdPara.Spd = SPD_END(AxNum); 
		PulsSpdPara.Puls += (int)(PulsSpdPara.Spd*ST/1000.0f+0.5f);
	}
	return PulsSpdPara.Puls;
}

u32 CurvesReset(u8 AxNum,u8 TorS,u32 Vinit,u32 Vtar,u32 Vend,u32 Pos,u32 Tacc,u32 Tdec)
{
    CURVE_MODE(AxNum) = TorS;
    SPD_INIT(AxNum) = Vinit;
	SPD_CUR(AxNum) = Vinit;
    SPD_TAR(AxNum) = Vtar;
    SPD_END(AxNum) = Vend;
    PULS_TAR(AxNum) = Pos;
    SPD_TACC(AxNum) = Tacc;
    SPD_TDEC(AxNum) = Tdec;
    
    CalcPulse(AxNum);
    SpdInit(AxNum);   
    return 0;
}
