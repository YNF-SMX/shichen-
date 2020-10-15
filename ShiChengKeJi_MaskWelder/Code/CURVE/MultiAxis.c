#include "MultiAxis.h"
#include "stdarg.h"
#include "bsplib.h"
#include "interlayer_config.h"

//#include "DataDefine.h"
//#include "axismove.h"
/*
 2轴直线
 输入参数： #初始位置#   #终点位置#   #合速度#

 3轴直线
 4轴直线

 2轴圆弧
 3轴圆弧

*/
CircleDef Circle[4] = {0};
MultiAxParaDef MultiAx[4] = {0};
MultiAxParaDef MultiAxLine[4] = {0};
MultiAxParaDef MultiAxCirc[4] = {0};

u32 mresultant(s32 a, s32 b)
{
    u32 r;
    a = (a >= 0 ? a : -a);
    b = (b >= 0 ? b : -b);
    r = sqrtf(1.0f * a * a + 1.0f * b * b);
    return r;
}

u32 mresultant_xyz(s32 a, s32 b, s32 c)
{
    u32 r;
    a = (a >= 0 ? a : -a);
    b = (b >= 0 ? b : -b);
    c = (c >= 0 ? c : -c);
    r = sqrtf(1.0f * a * a + 1.0f * b * b +  1.0f * c * c);
    return r;
}

// 3点计算圆信息
s32 ThreePointToCirc(u8 Num, CircPointDef p1, CircPointDef p2, CircPointDef p3)
{
    float a, b, c, d, delta, m, n;
    float te, tf, tg;

    a = p2.x - p1.x;
    b = p2.y - p1.y;
    c = p3.x - p1.x;
    d = p3.y - p1.y;
    delta = a * d - b * c;
    m = (1.0f * p1.x * p1.x + 1.0f * p1.y * p1.y) - (1.0f * p2.x * p2.x + 1.0f * p2.y * p2.y);
    n = (1.0f * p1.x * p1.x + 1.0f * p1.y * p1.y) - (1.0f * p3.x * p3.x + 1.0f * p3.y * p3.y);
    if(fabs(delta) > 0.001)
    {
        te = (m / (float)delta) * d - (n / (float)delta) * b;
        tf = (n / (float)delta) * a - (m / (float)delta) * c;
        tg = -(1.0f * p1.x * te + 1.0f * p1.y * tf + 1.0f * p1.x * p1.x + 1.0f * p1.y * p1.y);

        Circle[Num].rxy.x = -te / 2;
        Circle[Num].rxy.y = -tf / 2;
        Circle[Num].r = sqrtf(te * te + tf * tf - 4 * tg) / 2;
        return 0;
    }
    else
    {
        Circle[Num].rxy.x = 0;
        Circle[Num].rxy.y = 0;
        Circle[Num].r = 0;
        return -1;
    }
}

// 弧度计算
float RadianCal(CircPointDef p, CircleDef circle)
{
    float radian;
    //radian = (p.x-circle.rxy.x)/circle.r;
    //if(radian>1)
    //{
    //	radian = 1;
    //}
    //else if(radian<-1)
    //{
    //	radian = -1;
    //}
    //radian = (p.y>=circle.rxy.y ? acosf(radian): 2*M_PI -acosf(radian));
    radian = atan2(p.y - circle.rxy.y, p.x - circle.rxy.x);
    return radian;
}

// 方向计算
s8 RotCal(float rad1, float rad2, float rad3)
{
    s8 rot;
    if(rad2 < rad1) rad2 = rad2 + 2 * M_PI;
    if(rad3 < rad1) rad3 = rad3 + 2 * M_PI;
    rot = ((rad3 - rad1) > (rad2 - rad1) ? CCW : CW);
    return rot;
}

// 弦长计算弧度
float ChordlenToRad(u32 Chdlen, float r)
{
    return 2 * asinf(Chdlen / (2.0f * r));
}

s32 ArcCalc(u8 Group)
{
    u8 i;
    ThreePointToCirc(Group, Circle[Group].p[0], Circle[Group].p[1], Circle[Group].p[2]);
    if(Circle[Group].r != 0)
    {
        for(i = 0; i < 3; i++)
        {
            Circle[Group].p[i].rad = RadianCal(Circle[Group].p[i], Circle[Group]);
        }
        if(Circle[Group].arc == 0) //弧
        {
            Circle[Group].rot = RotCal(Circle[Group].p[0].rad, Circle[Group].p[1].rad, Circle[Group].p[2].rad);
            if(Circle[Group].rot == CCW)	//rad2>rad1>rad0
            {
                Circle[Group].p[1].rad = (Circle[Group].p[1].rad > Circle[Group].p[0].rad ? Circle[Group].p[1].rad : Circle[Group].p[1].rad + 2 * M_PI);
                Circle[Group].p[2].rad = (Circle[Group].p[2].rad > Circle[Group].p[0].rad ? Circle[Group].p[2].rad : Circle[Group].p[2].rad + 2 * M_PI);
                Circle[Group].arc = Circle[Group].p[2].rad - Circle[Group].p[0].rad;
            }
            else	//rad2<rad1<rad0
            {
                Circle[Group].p[1].rad = (Circle[Group].p[1].rad < Circle[Group].p[0].rad ? Circle[Group].p[1].rad : Circle[Group].p[1].rad - 2 * M_PI);
                Circle[Group].p[2].rad = (Circle[Group].p[2].rad < Circle[Group].p[0].rad ? Circle[Group].p[2].rad : Circle[Group].p[2].rad - 2 * M_PI);
                Circle[Group].arc = Circle[Group].p[0].rad - Circle[Group].p[2].rad;
            }
        }
        else
        {
            Circle[Group].p[2].x = Circle[Group].p[0].x;
            Circle[Group].p[2].y = Circle[Group].p[0].y;
            Circle[Group].p[2].rad = (Circle[Group].rot == CCW ? Circle[Group].p[0].rad + 2 * M_PI : Circle[Group].p[0].rad - 2 * M_PI);
        }
    }
    return 0;
}

//加速圆弧弧长计算
RadPulsSpdDef Disc_AcceRadCalc(u32 Vinit, u32 Vtar, u32 Acce, float r)
{
    RadPulsSpdDef RadPulsSpd;
    RadPulsSpd.Spd = Vinit;
    RadPulsSpd.Puls = mround(RadPulsSpd.Spd * INPST);
    RadPulsSpd.Rad = ChordlenToRad(RadPulsSpd.Puls, r);
    if(Acce > 0 && Acce * INPST > 1.0f && Vinit < Vtar)
    {
        while(RadPulsSpd.Spd + mround(1.5f * Acce * INPST) < Vtar)
        {
            RadPulsSpd.Spd += mround(Acce * INPST);
            RadPulsSpd.Puls = mround(RadPulsSpd.Spd * INPST);
            RadPulsSpd.Rad += ChordlenToRad(RadPulsSpd.Puls, r);
        }
    }
    else//匀速
    {
        RadPulsSpd.Spd = Vinit;
        RadPulsSpd.Rad = 0;
    }
    return RadPulsSpd;
}

//减速圆弧弧长计算
RadPulsSpdDef Disc_DeceRadCalc(u32 Vtar, u32 Vend, u32 Dece, float r)
{
    RadPulsSpdDef RadPulsSpd;
    RadPulsSpd.Spd = mround(Vtar - Dece * INPST);
    RadPulsSpd.Puls = mround(RadPulsSpd.Spd * INPST);
    RadPulsSpd.Rad = ChordlenToRad(RadPulsSpd.Puls, r);
    if(Dece > 0 && Dece * INPST > 1.0f && Vend < Vtar)
    {
        while(RadPulsSpd.Spd > (Vend + mround(Dece * INPST / 2.0f)))
        {
            RadPulsSpd.Spd = mround(RadPulsSpd.Spd - Dece * INPST);
            if(RadPulsSpd.Spd < Vend || RadPulsSpd.Spd > 0x70000000) //防止数据越界
                RadPulsSpd.Spd = Vend;
            RadPulsSpd.Puls = mround(RadPulsSpd.Spd * INPST);
            RadPulsSpd.Rad += ChordlenToRad(RadPulsSpd.Puls, r);
        }
    }
    else
    {
        RadPulsSpd.Spd = Vend;
        RadPulsSpd.Rad = 0;
    }
    return RadPulsSpd;
}

// 重新分配运行速度
RadPulsSpdDef RadToSpd(u32 Vinit, u32 Acce, float rad, float r)
{
    RadPulsSpdDef RadPulsSpd;
    RadPulsSpd.Spd = Vinit;
    RadPulsSpd.Puls = mround(RadPulsSpd.Spd * INPST);
    RadPulsSpd.Rad = ChordlenToRad(RadPulsSpd.Puls, r);

    while(RadPulsSpd.Rad < rad)
    {
        RadPulsSpd.Spd += mround (Acce * INPST);
        RadPulsSpd.Puls = mround (RadPulsSpd.Spd * INPST);
        RadPulsSpd.Rad += ChordlenToRad(RadPulsSpd.Puls, r);
    }
    RadPulsSpd.Rad = RadPulsSpd.Rad - ChordlenToRad(RadPulsSpd.Puls, r);
    return RadPulsSpd;
}

//计算加速、匀速、减速行程(弧度)
s32 MultiAxArcCalc(u8 Group)
{
    RadPulsSpdDef tRadPulsSpd;
    tRadPulsSpd = Disc_AcceRadCalc(MultiAxCirc[Group].Vinit, MultiAxCirc[Group].Vtar, MultiAxCirc[Group].Acce, Circle[Group].r);
    Circle[Group].arc_acc = tRadPulsSpd.Rad;
    tRadPulsSpd = Disc_DeceRadCalc(MultiAxCirc[Group].Vtar, MultiAxCirc[Group].Vinit, MultiAxCirc[Group].Acce, Circle[Group].r);
    Circle[Group].arc_dec = tRadPulsSpd.Rad;
    if(Circle[Group].arc_acc + Circle[Group].arc_dec < Circle[Group].arc)
    {
        Circle[Group].arc_uni = Circle[Group].arc - (Circle[Group].arc_acc + Circle[Group].arc_dec);
    }
    else
    {
        tRadPulsSpd = RadToSpd(MultiAxCirc[Group].Vinit, MultiAxCirc[Group].Acce, Circle[Group].arc / 2, Circle[Group].r);
        MultiAxCirc[Group].Vtar = tRadPulsSpd.Spd;
        Circle[Group].arc_acc = tRadPulsSpd.Rad;
        tRadPulsSpd = Disc_DeceRadCalc(MultiAxCirc[Group].Vtar, MultiAxCirc[Group].Vinit, MultiAxCirc[Group].Acce, Circle[Group].r);
        Circle[Group].arc_dec = tRadPulsSpd.Rad;
        if(Circle[Group].arc_acc + Circle[Group].arc_dec < Circle[Group].arc)
        {
            Circle[Group].arc_uni = Circle[Group].arc - (Circle[Group].arc_acc + Circle[Group].arc_dec);
        }
        else
        {
            Circle[Group].arc_uni = 0;
        }
    }
    return 0;
}

//获取轴状态
//轴IDLE时为 1
//轴BUSY时为 0
s32 MultiAxStatus(u8 Group, u8 AxNum)
{
    u8 i;
    //u8 result;
    for(i = 0; i < AxNum; i++)
    {
        if(HZ_AxGetStatus(MultiAx[Group].Axis[i]) != AXSTA_SYNC)
            return -1;

        if(Multi_AxGetStop(MultiAx[Group].Axis[i]) > 0) //||Multi_AxGetStop(MultiAx[Group].Axis[i])==(-1))
            return 0;
    }
    return 1;
}

//匹配圆弧单步对应的步长和速度
s32 MultiAxArcStep(u8 Group, u8 AxNum)
{
    u8 i;
    MultiAxCirc[Group].PosResu = MultiAxCirc[Group].Vresu * INPST;									//单步弦长
    Circle[Group].arc_step = ChordlenToRad(MultiAxCirc[Group].PosResu, Circle[Group].r);
    if(Circle[Group].rot == CCW)
    {
        Circle[Group].arc_nxt += Circle[Group].arc_step;											//计算下个位置对应的弧度
        Circle[Group].arc_cur = Circle[Group].arc_nxt - Circle[Group].p[0].rad;					    //计算下个位置对应的弧长
    }
    else
    {
        Circle[Group].arc_nxt -= Circle[Group].arc_step;				 							//计算下个位置对应的弧度
        Circle[Group].arc_cur = Circle[Group].p[0].rad - Circle[Group].arc_nxt;					 	//计算下个位置对应的弧长
    }
    for(i = 0; i < AxNum; i++)
    {
        if(i == 0)
        {
            MultiAxCirc[Group].RunPos[i] = Circle[Group].rxy.x + mround(Circle[Group].r * cosf(Circle[Group].arc_nxt)) - HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);	 //相对坐标x
        }
        else if(i == 1)
        {
            MultiAxCirc[Group].RunPos[i] = Circle[Group].rxy.y + mround(Circle[Group].r * sinf(Circle[Group].arc_nxt)) - HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);	 //相对坐标y
        }
        else if(i == 2)
        {
            MultiAxCirc[Group].RunPos[i] = mround(1.0f * (Circle[Group].p[2].z - Circle[Group].p[0].z) * Circle[Group].arc_cur / Circle[Group].arc);
            MultiAxCirc[Group].RunPos[i] += Circle[Group].p[0].z;
            MultiAxCirc[Group].RunPos[i] -= HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);
        }
        else
        {
            MultiAxCirc[Group].RunPos[i] = mround(1.0f * (Circle[Group].p[2].w - Circle[Group].p[0].w) * Circle[Group].arc_cur / Circle[Group].arc);
            MultiAxCirc[Group].RunPos[i] += Circle[Group].p[0].w;
            MultiAxCirc[Group].RunPos[i] -= HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);
        }
    }
    MultiAxCirc[Group].PosResu = mround(mresultant(MultiAxCirc[Group].RunPos[0], MultiAxCirc[Group].RunPos[1]));
    for(i = 0; i < AxNum; i++)
    {
        MultiAxCirc[Group].RunSpd[i] = MultiAxCirc[Group].Vresu * (MultiAxCirc[Group].RunPos[i] > 0 ? MultiAxCirc[Group].RunPos[i] : -MultiAxCirc[Group].RunPos[i]) / MultiAxCirc[Group].PosResu;
        HZ_AxSetPara(i, MultiAxCirc[Group].RunSpd[i], 10, MultiAxCirc[Group].RunSpd[i], 10, MultiAxCirc[Group].RunSpd[i], 1000, 700, 0, 0, 0);
        //HZ_AxMoveRel(MultiAxCirc[Group].Axis[i],MultiAxCirc[Group].RunPos[i]);
        Multi_AxMoveRel(MultiAxCirc[Group].Axis[i], MultiAxCirc[Group].RunPos[i]);
    }
    return 0;
}

//计算直线加速距离
PulsSpdDef Disc_AcceCalc(u32 Vinit, u32 Vtar, u32 Acce)
{
    PulsSpdDef PulsSpd;
    PulsSpd.Spd = Vinit;
    PulsSpd.Puls = mround(PulsSpd.Spd * INPST);
    //20190424
    if(Acce == 0)
    {
        PulsSpd.Puls = 0;
    }
    else
    {
        while(PulsSpd.Spd + mround(1.5f * Acce * INPST) < Vtar)
        {
            PulsSpd.Spd += mround(Acce * INPST);
            PulsSpd.Puls += mround(PulsSpd.Spd * INPST);
        }
    }
    return PulsSpd;
}

//计算直线减速距离
PulsSpdDef Disc_DeceCalc(u32 Vtar, u32 Vend, u32 Dece)
{
    PulsSpdDef PulsSpd;
    PulsSpd.Spd = mround(Vtar - Dece * INPST);
    PulsSpd.Puls = mround(PulsSpd.Spd * INPST);
    //20190424
    if(Dece == 0)
    {
        PulsSpd.Spd = Vend;
        PulsSpd.Puls = 0;
    }
    else
    {
        while(PulsSpd.Spd > (Vend + mround(Dece * INPST / 2.0f)))
        {
            PulsSpd.Spd = mround(PulsSpd.Spd - Dece * INPST);
            if(PulsSpd.Spd < Vend || PulsSpd.Spd > 0x70000000) //防止数据越界
                PulsSpd.Spd = Vend;
            PulsSpd.Puls += mround(PulsSpd.Spd * INPST);
        }
    }
    return PulsSpd;
}

PulsSpdDef DiscToSpd(u32 Vinit, u32 Acce, u32 Puls)
{
    PulsSpdDef PulsSpd;
    PulsSpd.Spd = Vinit;
    PulsSpd.Puls = mround(PulsSpd.Spd * INPST);
    while(PulsSpd.Puls < Puls)
    {
        PulsSpd.Spd += mround(Acce * INPST);
        PulsSpd.Puls += mround(PulsSpd.Spd * INPST);
    }
    PulsSpd.Puls = mround(PulsSpd.Puls - PulsSpd.Spd * INPST);
    return PulsSpd;
}

//计算直线加速、匀速、减速行程
s32 MultiAxLineCalc(u8 Group)
{
    PulsSpdDef PulsSpd;
    PulsSpd = Disc_AcceCalc(MultiAxLine[Group].Vinit, MultiAxLine[Group].Vtar, MultiAxLine[Group].Acce);
    MultiAxLine[Group].PosAcc = PulsSpd.Puls;
    PulsSpd = Disc_DeceCalc(MultiAxLine[Group].Vtar, MultiAxLine[Group].Vinit, MultiAxLine[Group].Acce);
    MultiAxLine[Group].PosDec = PulsSpd.Puls;
    if(MultiAxLine[Group].PosAcc + MultiAxLine[Group].PosDec < MultiAxLine[Group].Pos_XY)
    {
        MultiAxLine[Group].PosUnif = MultiAxLine[Group].Pos_XY - (MultiAxLine[Group].PosAcc + MultiAxLine[Group].PosDec);
    }
    else
    {
        PulsSpd = DiscToSpd(MultiAxLine[Group].Vinit, MultiAxLine[Group].Acce, MultiAxLine[Group].Pos_XY / 2); //加速度足够大
        MultiAxLine[Group].Vtar = PulsSpd.Spd;
        MultiAxLine[Group].PosAcc = PulsSpd.Puls;
        PulsSpd = Disc_DeceCalc(MultiAxLine[Group].Vtar, MultiAxLine[Group].Vinit, MultiAxLine[Group].Acce);
        MultiAxLine[Group].PosDec = PulsSpd.Puls;
        if(MultiAxLine[Group].PosAcc + MultiAxLine[Group].PosDec < MultiAxLine[Group].Pos_XY)
        {
            MultiAxLine[Group].PosUnif = MultiAxLine[Group].Pos_XY - (MultiAxLine[Group].PosAcc + MultiAxLine[Group].PosDec);
        }
        else
        {
            MultiAxLine[Group].PosUnif = 0;
        }
    }
    return 0;
}

s32 MultiAxLineStep(u8 Group, u8 AxNum)
{
    MultiAxLine[Group].PosStep = mround(MultiAxLine[Group].Vresu * INPST);
    MultiAxLine[Group].PosResu += MultiAxLine[Group].PosStep;

    MultiAxLine[Group].RunPos[0] = mround(1.0f * MultiAxLine[Group].PosResu * MultiAxLine[Group].Pos_X / MultiAxLine[Group].Pos_XY);
    MultiAxLine[Group].RunPos[0] += (MultiAxLine[Group].InitPos[0] - HZ_AxGetCurPos(MultiAxLine[Group].Axis[0]));

    MultiAxLine[Group].RunPos[1] = mround(1.0f * MultiAxLine[Group].PosResu * MultiAxLine[Group].Pos_Y / MultiAxLine[Group].Pos_XY);
    MultiAxLine[Group].RunPos[1] += (MultiAxLine[Group].InitPos[1] - HZ_AxGetCurPos(MultiAxLine[Group].Axis[1]));

    MultiAxLine[Group].RunPos[2] = mround(1.0f * MultiAxLine[Group].PosResu * MultiAxLine[Group].Pos_Z / MultiAxLine[Group].Pos_XY);
    MultiAxLine[Group].RunPos[2] += (MultiAxLine[Group].InitPos[2] - HZ_AxGetCurPos(MultiAxLine[Group].Axis[2]));

    MultiAxLine[Group].RunPos[3] = mround(1.0f * MultiAxLine[Group].PosResu * MultiAxLine[Group].Pos_W / MultiAxLine[Group].Pos_XY);
    MultiAxLine[Group].RunPos[3] += (MultiAxLine[Group].InitPos[3] - HZ_AxGetCurPos(MultiAxLine[Group].Axis[3]));

    for(u8 i = 0; i < AxNum; i++)
    {
        MultiAxLine[Group].RunSpd[i] = MultiAxLine[Group].Vresu * mabs(MultiAxLine[Group].RunPos[i]) / MultiAxLine[Group].PosStep;
        HZ_AxSetPara(i, MultiAxLine[Group].RunSpd[i], 10, MultiAxLine[Group].RunSpd[i], 10, MultiAxLine[Group].RunSpd[i], 1000, 700, 0, 0, 0);
        //HZ_AxMoveRel(MultiAxLine[Group].Axis[i],MultiAxLine[Group].RunPos[i]);
        Multi_AxMoveRel(MultiAxLine[Group].Axis[i], MultiAxLine[Group].RunPos[i]);
    }
    return 0;
}

s32 MultiAxMove(u8 Group, u8 AxNum)
{
    u8 i;
    if(MultiAxLine[Group].execute == 1 && MultiAxLine[Group].step == MultiAxIdle)
    {
        for(i = 0; i < AxNum; i++)
        {
            Multi_AxSetMulti(MultiAxLine[Group].Axis[i]);
        }
        MultiAxLineCalc(Group);
        MultiAxLine[Group].Vresu = MultiAxLine[Group].Vinit;
        MultiAxLine[Group].PosStep = mround(MultiAxLine[Group].Vresu * INPST);
        MultiAxLine[Group].PosResu = MultiAxLine[Group].PosStep;

        MultiAxLine[Group].RunPos[0] = mround(1.0f * MultiAxLine[Group].PosStep * MultiAxLine[Group].Pos_X / MultiAxLine[Group].Pos_XY);
        MultiAxLine[Group].RunPos[1] = mround(1.0f * MultiAxLine[Group].PosStep * MultiAxLine[Group].Pos_Y / MultiAxLine[Group].Pos_XY);
        MultiAxLine[Group].RunPos[2] = mround(1.0f * MultiAxLine[Group].PosStep * MultiAxLine[Group].Pos_Z / MultiAxLine[Group].Pos_XY);
        MultiAxLine[Group].RunPos[3] = mround(1.0f * MultiAxLine[Group].PosStep * MultiAxLine[Group].Pos_W / MultiAxLine[Group].Pos_XY);
        for(i = 0; i < AxNum; i++)
        {
            MultiAxLine[Group].RunSpd[i] = MultiAxLine[Group].Vresu * mabs(MultiAxLine[Group].RunPos[i]) / MultiAxLine[Group].PosStep;
            HZ_AxSetPara(i, MultiAxLine[Group].RunSpd[i], 10, MultiAxLine[Group].RunSpd[i], 10, MultiAxLine[Group].RunSpd[i], 1000, 700, 0, 0, 0);
            //HZ_AxMoveRel(MultiAxLine[Group].Axis[i],MultiAxLine[Group].RunPos[i]);
            Multi_AxMoveRel(MultiAxLine[Group].Axis[i], MultiAxLine[Group].RunPos[i]);
        }
        if(MultiAxLine[Group].PosAcc == 0 && MultiAxLine[Group].PosDec == 0)
        {
            MultiAxLine[Group].step = MultiAxDece;
        }
        else
        {
            MultiAxLine[Group].step = MultiAxAcce;
        }

    }
    switch(MultiAxLine[Group].step)
    {
    case MultiAxAcce:
        if(MultiAxStatus(Group, AxNum) == 1)
        {
            if(MultiAxLine[Group].PosResu + MultiAxLine[Group].PosUnif + MultiAxLine[Group].PosDec < MultiAxLine[Group].Pos_XY)
            {
                //加速
                MultiAxLine[Group].Vresu += mround(MultiAxLine[Group].Acce * INPST);
                MultiAxLineStep(Group, AxNum);
            }
            else
            {
                //状态切换
                if(MultiAxLine[Group].PosUnif > 0)
                {
                    MultiAxLine[Group].Vresu = MultiAxLine[Group].Vtar;
                    MultiAxLineStep(Group, AxNum);
                    MultiAxLine[Group].step = MultiAxUnif;
                }
                else
                {
                    MultiAxLine[Group].Vresu -= mround(MultiAxLine[Group].Acce * INPST);
                    if(MultiAxLine[Group].Vresu > 0x70000000)
                        MultiAxLine[Group].Vresu = MultiAxLine[Group].Vinit;
                    MultiAxLine[Group].Vresu = (MultiAxLine[Group].Vresu > MultiAxLine[Group].Vinit ? MultiAxLine[Group].Vresu : MultiAxLine[Group].Vinit);
                    MultiAxLineStep(Group, AxNum);
                    MultiAxLine[Group].step = MultiAxDece;
                }
            }
        }
        else if(MultiAxStatus(Group, AxNum) == -1)
        {
            for(i = 0; i < AxNum; i++)
            {
                HZ_AxStop(MultiAxLine[Group].Axis[i]);
            }
            MultiAxLine[Group].done = 0;
            MultiAxLine[Group].execute = 0;
            MultiAxLine[Group].step = 0;
        }
        break;

    case MultiAxUnif:
        if(MultiAxStatus(Group, AxNum) == 1)
        {
            if(MultiAxLine[Group].PosResu + MultiAxLine[Group].PosDec < MultiAxLine[Group].Pos_XY)
            {
                MultiAxLine[Group].Vresu = MultiAxLine[Group].Vtar;
                MultiAxLineStep(Group, AxNum);
            }
            else
            {
                MultiAxLine[Group].Vresu -= mround(MultiAxLine[Group].Acce * INPST);
                if(MultiAxLine[Group].Vresu > 0x70000000)
                    MultiAxLine[Group].Vresu = MultiAxLine[Group].Vinit;
                MultiAxLine[Group].Vresu = (MultiAxLine[Group].Vresu > MultiAxLine[Group].Vinit ? MultiAxLine[Group].Vresu : MultiAxLine[Group].Vinit);
                MultiAxLineStep(Group, AxNum);
                MultiAxLine[Group].step = MultiAxDece;
            }
        }
        else if(MultiAxStatus(Group, AxNum) == -1)
        {
            for(i = 0; i < AxNum; i++)
            {
                HZ_AxStop(MultiAxLine[Group].Axis[i]);
            }
            MultiAxLine[Group].done = 0;
            MultiAxLine[Group].execute = 0;
            MultiAxLine[Group].step = 0;
        }
        break;

    case MultiAxDece:
        if(MultiAxStatus(Group, AxNum) == 1)
        {
            if(MultiAxLine[Group].done == 1)
            {
                MultiAxLine[Group].done = 0;
                MultiAxLine[Group].execute = 0;
                MultiAxLine[Group].step = 0;
                //by yang
                for(i = 0; i < AxNum; i++)
                {

                    A_STA(MultiAxLine[Group].Axis[i]) = AXSTA_READY;
                }
            }
            else
            {
                MultiAxLine[Group].Vresu -= mround(MultiAxLine[Group].Acce * INPST);
                if(MultiAxLine[Group].Vresu > 0x70000000)
                    MultiAxLine[Group].Vresu = MultiAxLine[Group].Vinit;
                MultiAxLine[Group].Vresu = (MultiAxLine[Group].Vresu > MultiAxLine[Group].Vinit ? MultiAxLine[Group].Vresu : MultiAxLine[Group].Vinit);
                MultiAxLine[Group].PosStep = mround(MultiAxLine[Group].Vresu * INPST);
                MultiAxLine[Group].PosResu += MultiAxLine[Group].PosStep;

                if(MultiAxLine[Group].PosResu < MultiAxLine[Group].Pos_XY)
                {
                    MultiAxLine[Group].RunPos[0] = mround(1.0f * MultiAxLine[Group].PosResu * MultiAxLine[Group].Pos_X / MultiAxLine[Group].Pos_XY);
                    MultiAxLine[Group].RunPos[0] += (MultiAxLine[Group].InitPos[0] - HZ_AxGetCurPos(MultiAxLine[Group].Axis[0]));

                    MultiAxLine[Group].RunPos[1] = mround(1.0f * MultiAxLine[Group].PosResu * MultiAxLine[Group].Pos_Y / MultiAxLine[Group].Pos_XY);
                    MultiAxLine[Group].RunPos[1] += (MultiAxLine[Group].InitPos[1] - HZ_AxGetCurPos(MultiAxLine[Group].Axis[1]));

                    MultiAxLine[Group].RunPos[2] = mround(1.0f * MultiAxLine[Group].PosResu * MultiAxLine[Group].Pos_Z / MultiAxLine[Group].Pos_XY);
                    MultiAxLine[Group].RunPos[2] += (MultiAxLine[Group].InitPos[2] - HZ_AxGetCurPos(MultiAxLine[Group].Axis[2]));

                    MultiAxLine[Group].RunPos[3] = mround(1.0f * MultiAxLine[Group].PosResu * MultiAxLine[Group].Pos_W / MultiAxLine[Group].Pos_XY);
                    MultiAxLine[Group].RunPos[3] += (MultiAxLine[Group].InitPos[3] - HZ_AxGetCurPos(MultiAxLine[Group].Axis[3]));
                }
                else
                {
                    for(i = 0; i < AxNum; i++)
                    {
                        MultiAxLine[Group].RunPos[i] = MultiAxLine[Group].TarPos[i] - HZ_AxGetCurPos(MultiAxLine[Group].Axis[i]);	 //相对坐标x
                    }
                    MultiAxLine[Group].done = 1;
                }
                for(i = 0; i < AxNum; i++)
                {
                    MultiAxLine[Group].RunSpd[i] = MultiAxLine[Group].Vresu * mabs(MultiAxLine[Group].RunPos[i]) / MultiAxLine[Group].PosStep;
                    HZ_AxSetPara(i, MultiAxLine[Group].RunSpd[i], 10, MultiAxLine[Group].RunSpd[i], 10, MultiAxLine[Group].RunSpd[i], 1000, 700, 0, 0, 0);
                    //HZ_AxMoveRel(MultiAxLine[Group].Axis[i],MultiAxLine[Group].RunPos[i]);
                    Multi_AxMoveRel(MultiAxLine[Group].Axis[i], MultiAxLine[Group].RunPos[i]);
                }
            }
        }
        else if(MultiAxStatus(Group, AxNum) == -1)
        {
            for(i = 0; i < AxNum; i++)
            {
                HZ_AxStop(MultiAxLine[Group].Axis[i]);
            }
            MultiAxLine[Group].done = 0;
            MultiAxLine[Group].execute = 0;
            MultiAxLine[Group].step = 0;
        }
        break;
    }
    return 0;
}

//放入主循环
s32 MultiAxMoveArc(u8 Group, u8 AxNum)
{
    u8 i;
    if(MultiAxCirc[Group].execute == 1 && MultiAxCirc[Group].step == MultiAxIdle)
    {
        //by yang
        for(i = 0; i < AxNum; i++)
        {
            Multi_AxSetMulti(MultiAxCirc[Group].Axis[i]);
        }

        MultiAxArcCalc(Group);
        MultiAxCirc[Group].Vresu = MultiAxCirc[Group].Vinit;
        MultiAxCirc[Group].PosResu = mround(MultiAxCirc[Group].Vresu * INPST);															     //单步弦长
        Circle[Group].arc_step = ChordlenToRad(MultiAxCirc[Group].PosResu, Circle[Group].r);
        if(Circle[Group].rot == CCW)
        {
            Circle[Group].arc_nxt = Circle[Group].p[0].rad + Circle[Group].arc_step;				 //计算下个位置对应的弧度
            Circle[Group].arc_cur = Circle[Group].arc_step;
        }
        else
        {
            Circle[Group].arc_nxt = Circle[Group].p[0].rad - Circle[Group].arc_step;				 //计算下个位置对应的弧度
            Circle[Group].arc_cur = Circle[Group].arc_step;
        }

        for(i = 0; i < AxNum; i++)
        {
            if(i == 0)
            {
                MultiAxCirc[Group].RunPos[i] = Circle[Group].rxy.x + mround(Circle[Group].r * cosf(Circle[Group].arc_nxt)) - Circle[Group].p[0].x;	 //相对坐标x
            }
            else if(i == 1)
            {
                MultiAxCirc[Group].RunPos[i] = Circle[Group].rxy.y + mround(Circle[Group].r * sinf(Circle[Group].arc_nxt)) - Circle[Group].p[0].y;	 //相对坐标y
            }
            else if(i == 2)
            {
                MultiAxCirc[Group].RunPos[i] = mround(1.0f * (Circle[Group].z) * Circle[Group].arc_cur / Circle[Group].arc);
                MultiAxCirc[Group].RunPos[i] += Circle[Group].p[0].z;
                MultiAxCirc[Group].RunPos[i] -= HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);
            }
            else
            {
                MultiAxCirc[Group].RunPos[i] = mround(1.0f * (Circle[Group].w) * Circle[Group].arc_cur / Circle[Group].arc);
                MultiAxCirc[Group].RunPos[i] += Circle[Group].p[0].w;
                MultiAxCirc[Group].RunPos[i] -= HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);
            }
        }
        MultiAxCirc[Group].PosResu = mround(mresultant(MultiAxCirc[Group].RunPos[0], MultiAxCirc[Group].RunPos[1]));
        //相对长度 轴动
        for(i = 0; i < AxNum; i++)
        {
            MultiAxCirc[Group].RunSpd[i] = MultiAxCirc[Group].Vresu * (MultiAxCirc[Group].RunPos[i] > 0 ? MultiAxCirc[Group].RunPos[i] : -MultiAxCirc[Group].RunPos[i]) / MultiAxCirc[Group].PosResu;
            HZ_AxSetPara(i, MultiAxCirc[Group].RunSpd[i], 10, MultiAxCirc[Group].RunSpd[i], 10, MultiAxCirc[Group].RunSpd[i], 1000, 700, 0, 0, 0);
            //HZ_AxMoveRel(MultiAxCirc[Group].Axis[i],MultiAxCirc[Group].RunPos[i]);
            Multi_AxMoveRel(MultiAxCirc[Group].Axis[i], MultiAxCirc[Group].RunPos[i]);
        }

        if(Circle[Group].arc_acc == 0 && Circle[Group].arc_dec == 0)
        {
            MultiAxCirc[Group].step = MultiAxDece;
        }
        else
        {
            MultiAxCirc[Group].step = MultiAxAcce;
        }
    }
    switch(MultiAxCirc[Group].step)
    {
    case MultiAxAcce:
        if(MultiAxStatus(Group, AxNum) == 1)
        {
            if(Circle[Group].arc_cur + Circle[Group].arc_uni + Circle[Group].arc_dec <= Circle[Group].arc)
            {
                //加速
                MultiAxCirc[Group].Vresu += mround(MultiAxCirc[Group].Acce * INPST);							//单步合速度
                MultiAxArcStep(Group, AxNum);
            }
            else
            {
                //状态切换
                if(Circle[Group].arc_uni > 0)
                {
                    MultiAxCirc[Group].Vresu = MultiAxCirc[Group].Vtar;									//单步合速度
                    MultiAxArcStep(Group, AxNum);
                    MultiAxCirc[Group].step = MultiAxUnif;
                }
                else
                {
                    MultiAxCirc[Group].Vresu -= mround(MultiAxCirc[Group].Acce * INPST);						//单步合速度
                    MultiAxCirc[Group].Vresu = (MultiAxCirc[Group].Vresu > MultiAxCirc[Group].Vinit ? MultiAxCirc[Group].Vresu : MultiAxCirc[Group].Vinit);
                    MultiAxArcStep(Group, AxNum);
                    MultiAxCirc[Group].step = MultiAxDece;
                }
            }
        }
        else if(MultiAxStatus(Group, AxNum) == -1)
        {
            for(i = 0; i < AxNum; i++)
            {
                HZ_AxStop(MultiAxCirc[Group].Axis[i]);
            }
            MultiAxCirc[Group].done = 0;
            MultiAxCirc[Group].execute = 0;
            MultiAxCirc[Group].step = 0;
        }
        break;

    case MultiAxUnif:
        if(MultiAxStatus(Group, AxNum) == 1)
        {
            if(Circle[Group].arc_cur + Circle[Group].arc_dec <= Circle[Group].arc)
            {
                MultiAxCirc[Group].Vresu = MultiAxCirc[Group].Vtar;									//单步合速度
                MultiAxArcStep(Group, AxNum);
            }
            else
            {
                MultiAxCirc[Group].Vresu -= mround(MultiAxCirc[Group].Acce * INPST);						//单步合速度
                MultiAxCirc[Group].Vresu = (MultiAxCirc[Group].Vresu > MultiAxCirc[Group].Vinit ? MultiAxCirc[Group].Vresu : MultiAxCirc[Group].Vinit);
                MultiAxArcStep(Group, AxNum);
                MultiAxCirc[Group].step = MultiAxDece;
            }
        }
        else if(MultiAxStatus(Group, AxNum) == -1)
        {
            for(i = 0; i < AxNum; i++)
            {
                HZ_AxStop(MultiAxCirc[Group].Axis[i]);
            }
            MultiAxCirc[Group].done = 0;
            MultiAxCirc[Group].execute = 0;
            MultiAxCirc[Group].step = 0;
        }
        break;

    case MultiAxDece:
        if(MultiAxStatus(Group, AxNum) == 1)
        {
            if(MultiAxCirc[Group].done == 1)
            {
                MultiAxCirc[Group].done = 0;
                MultiAxCirc[Group].execute = 0;
                MultiAxCirc[Group].step = 0;
                //by yang
                for(i = 0; i < AxNum; i++)
                {

                    A_STA(MultiAxCirc[Group].Axis[i]) = AXSTA_READY;
                }
            }
            else if(Circle[Group].arc_cur <= Circle[Group].arc)
            {
                MultiAxCirc[Group].Vresu -= mround(MultiAxCirc[Group].Acce * INPST);																								//单步合速度
                MultiAxCirc[Group].Vresu = (MultiAxCirc[Group].Vresu > MultiAxCirc[Group].Vinit ? MultiAxCirc[Group].Vresu : MultiAxCirc[Group].Vinit);
                MultiAxCirc[Group].PosResu = mround(MultiAxCirc[Group].Vresu * INPST);
                Circle[Group].arc_step = ChordlenToRad(MultiAxCirc[Group].PosResu, Circle[Group].r);					//单步弦长
                if(Circle[Group].rot == 0)
                {
                    Circle[Group].arc_nxt += Circle[Group].arc_step;												 //计算下个位置对应的弧度
                    Circle[Group].arc_cur = Circle[Group].arc_nxt - Circle[Group].p[0].rad;					 		 //计算下个位置对应的弧长
                }
                else
                {
                    Circle[Group].arc_nxt -= Circle[Group].arc_step;				 								 //计算下个位置对应的弧度
                    Circle[Group].arc_cur = Circle[Group].p[0].rad - Circle[Group].arc_nxt;							 //计算下个位置对应的弧长
                }

                if(Circle[Group].arc_cur < Circle[Group].arc)
                {
                    for(i = 0; i < AxNum; i++)
                    {
                        if(i == 0)
                        {
                            MultiAxCirc[Group].RunPos[i] = Circle[Group].rxy.x + mround(Circle[Group].r * cosf(Circle[Group].arc_nxt));
                            MultiAxCirc[Group].RunPos[i] -= HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);	 //相对坐标x
                        }
                        else if(i == 1)
                        {
                            MultiAxCirc[Group].RunPos[i] = Circle[Group].rxy.y + mround(Circle[Group].r * sinf(Circle[Group].arc_nxt));
                            MultiAxCirc[Group].RunPos[i] -= HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);	 //相对坐标y
                        }
                        else if(i == 2)
                        {
                            MultiAxCirc[Group].RunPos[i] = mround(1.0f * (Circle[Group].p[2].z - Circle[Group].p[0].z) * Circle[Group].arc_cur / Circle[Group].arc);
                            MultiAxCirc[Group].RunPos[i] += Circle[Group].p[0].z;
                            MultiAxCirc[Group].RunPos[i] -= HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);
                        }
                        else
                        {
                            MultiAxCirc[Group].RunPos[i] = mround(1.0f * (Circle[Group].p[2].w - Circle[Group].p[0].w) * Circle[Group].arc_cur / Circle[Group].arc);
                            MultiAxCirc[Group].RunPos[i] += Circle[Group].p[0].w;
                            MultiAxCirc[Group].RunPos[i] -= HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);
                        }
                    }
                }
                else
                {
                    for(i = 0; i < AxNum; i++)
                    {
                        if(i == 0)
                        {
                            MultiAxCirc[Group].RunPos[i] = Circle[Group].p[2].x - HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);	 //相对坐标x
                        }
                        else if(i == 1)
                        {
                            MultiAxCirc[Group].RunPos[i] = Circle[Group].p[2].y - HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);	 //相对坐标y
                        }
                        else if(i == 2)
                        {
                            MultiAxCirc[Group].RunPos[i] = Circle[Group].p[2].z - HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);	 //相对坐标z
                        }
                        else if(i == 3)
                        {
                            MultiAxCirc[Group].RunPos[i] = Circle[Group].p[2].w - HZ_AxGetCurPos(MultiAxCirc[Group].Axis[i]);	 //相对坐标w
                        }
                        MultiAxCirc[Group].done = 1;
                    }
                }
                MultiAxCirc[Group].PosResu = mround(mresultant(MultiAxCirc[Group].RunPos[0], MultiAxCirc[Group].RunPos[1]));
                for(i = 0; i < AxNum; i++)
                {
                    MultiAxCirc[Group].RunSpd[i] = MultiAxCirc[Group].Vresu * (MultiAxCirc[Group].RunPos[i] > 0 ? MultiAxCirc[Group].RunPos[i] : -MultiAxCirc[Group].RunPos[i]) / MultiAxCirc[Group].PosResu;
                    HZ_AxSetPara(i, MultiAxCirc[Group].RunSpd[i], 10, MultiAxCirc[Group].RunSpd[i], 10, MultiAxCirc[Group].RunSpd[i], 1000, 700, 0, 0, 0);
                    //HZ_AxMoveRel(MultiAxCirc[Group].Axis[i],MultiAxCirc[Group].RunPos[i]);
                    Multi_AxMoveRel(MultiAxCirc[Group].Axis[i], MultiAxCirc[Group].RunPos[i]);
                }
            }
        }
        else if(MultiAxStatus(Group, AxNum) == -1)
        {
            for(i = 0; i < AxNum; i++)
            {
                HZ_AxStop(MultiAxCirc[Group].Axis[i]);
            }
            MultiAxCirc[Group].done = 0;
            MultiAxCirc[Group].execute = 0;
            MultiAxCirc[Group].step = 0;
        }
        break;
    }
    return 0;
}

s32 HZ_MultiAxMotion(void)
{
    for(u8 i = 0; i < 4; i++)
    {
        MultiAxMoveArc(i, MultiAxCirc[i].AxNum);
        MultiAxMove(i, MultiAxLine[i].AxNum);
    }
    return 0;
}


/**********************************************
应用层调用
**********************************************/

//设置轴

s32 HZ_MultiAxStatus(u8 Group)
{
    if(MultiAxCirc[Group].execute == 0 && MultiAxLine[Group].execute == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

//s32 HZ_MultiAxSet(u8 Group,u8 AxNum,u8 Ax0,u8 Ax1,u8 Ax2,u8 Ax3)
//{
//    if(AxNum<2||AxNum>=5)
//    {
//        return -1;
//    }
//    else
//    {
//        MultiAx[Group].Axis[0] = Ax0;
//        MultiAx[Group].Axis[1] = Ax1;
//        MultiAx[Group].Axis[2] = Ax2;
//        MultiAx[Group].Axis[3] = Ax3;
//        return 0;
//    }
//}

//by yang
s32 HZ_MultiAxSet(int Group, ...)
{
    va_list ap;
    //__va_start(ap, Group);
	va_start(ap, Group);
    MultiAx[Group].AxNum = va_arg(ap, int);//__va_arg(ap, int);
    if(MultiAx[Group].AxNum < 2 || MultiAx[Group].AxNum > 4)
    {
        return -1;
    }
    for(int i = 0; i < MultiAx[Group].AxNum; i++)
    {
        MultiAx[Group].Axis[i] = va_arg(ap, int);//__va_arg(ap, int);
    }
    //__va_end(ap);
	va_end(ap);
    return 0;
}

//设置速度
s32 HZ_MultiAxSetVal(u8 Group, VelocityDef Vel)
{
    if(Vel.Vinit < 260 || Vel.Vinit > 200000)
        Vel.Vinit = 260;
    if(Vel.Tacc > 1000)
        Vel.Tacc = 1000;
    if(Vel.Vtar < Vel.Vinit)
        Vel.Vtar = Vel.Vinit;

    MultiAx[Group].Vinit = Vel.Vinit;
    MultiAx[Group].Tacc = Vel.Tacc;
    MultiAx[Group].Vtar = Vel.Vtar;

    if(Vel.Vtar > Vel.Vinit && Vel.Tacc > 0)
        MultiAx[Group].Acce = (Vel.Vtar - Vel.Vinit) * 1000.f / (Vel.Tacc * 1.0f);
    else
        MultiAx[Group].Acce = 0; //初速度=运行速度

    if(MultiAx[Group].Acce < 1000) //加速度<1000 当匀速处理
        MultiAx[Group].Acce = 0;
    return 0;
}


//计算圆弧信息 不触发运动
s32 HZ_MultiAxCalcArc(u8 Group, u8 AxNum, PointDef Pinit, PointDef Pmid, PointDef Ptar)
{
    if(Group > 4 || AxNum < 2 || AxNum > 5)
    {
        return -1;
    }
    for(u8 i = 0; i < 4; i++)
        MultiAxCirc[Group].Axis[i] = MultiAx[Group].Axis[i];
    //如果当前轴正在同步运行 by yang
    for(int i = 0 ; i < AxNum; i++)
    {
        if(A_STA(MultiAx[Group].Axis[i]) != 0)
        {
            return -1;
        }
    }

    Circle[Group].p[0].x = Pinit.x;
    Circle[Group].p[0].y = Pinit.y;
    Circle[Group].p[0].z = Pinit.z;
    Circle[Group].p[0].w = Pinit.w;

    Circle[Group].p[1].x = Pmid.x;
    Circle[Group].p[1].y = Pmid.y;
    Circle[Group].p[1].z = Pmid.z;
    Circle[Group].p[1].w = Pmid.w;

    Circle[Group].p[2].x = Ptar.x;
    Circle[Group].p[2].y = Ptar.y;
    Circle[Group].p[2].z = Ptar.z;
    Circle[Group].p[2].w = Ptar.w;

    Circle[Group].arc = 0;
    Circle[Group].z = Circle[Group].p[2].z - Circle[Group].p[0].z;
    Circle[Group].w = Circle[Group].p[2].w - Circle[Group].p[0].w;
    Circle[Group].allcircle = 0;
    ArcCalc(Group);
    if((Circle[Group].p[2].x == Circle[Group].p[0].x && Circle[Group].p[2].y == Circle[Group].p[0].y)
            || (Circle[Group].p[1].x == Circle[Group].p[0].x && Circle[Group].p[1].y == Circle[Group].p[0].y)
            || Circle[Group].r == 0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


//设置圆弧信息及触发
s32 HZ_MultiAxMoveArc(u8 Group, u8 AxNum, PointDef Pmid, PointDef Ptar)
{
    if(Group > 4 || AxNum < 2 || AxNum > 5)
    {
        return -1;
    }
    for(u8 i = 0; i < 4; i++)
        MultiAxCirc[Group].Axis[i] = MultiAx[Group].Axis[i];
    //如果当前轴正在同步运行 by yang
    for(int i = 0 ; i < AxNum; i++)
    {
        if(A_STA(MultiAx[Group].Axis[i]) != 0)
        {
            return -1;
        }
    }

    Circle[Group].p[0].x = HZ_AxGetCurPos(MultiAxCirc[Group].Axis[0]);
    Circle[Group].p[0].y = HZ_AxGetCurPos(MultiAxCirc[Group].Axis[1]);
    Circle[Group].p[0].z = HZ_AxGetCurPos(MultiAxCirc[Group].Axis[2]);
    Circle[Group].p[0].w = HZ_AxGetCurPos(MultiAxCirc[Group].Axis[3]);

    Circle[Group].p[1].x = Pmid.x;
    Circle[Group].p[1].y = Pmid.y;
    Circle[Group].p[1].z = Pmid.z;
    Circle[Group].p[1].w = Pmid.w;

    Circle[Group].p[2].x = Ptar.x;
    Circle[Group].p[2].y = Ptar.y;
    Circle[Group].p[2].z = Ptar.z;
    Circle[Group].p[2].w = Ptar.w;

    Circle[Group].arc = 0;
    Circle[Group].z = Circle[Group].p[2].z - Circle[Group].p[0].z;
    Circle[Group].w = Circle[Group].p[2].w - Circle[Group].p[0].w;
    Circle[Group].allcircle = 0;
    ArcCalc(Group);
    if((Circle[Group].p[2].x == Circle[Group].p[0].x && Circle[Group].p[2].y == Circle[Group].p[0].y)
            || (Circle[Group].p[1].x == Circle[Group].p[0].x && Circle[Group].p[1].y == Circle[Group].p[0].y)
            || Circle[Group].r == 0)
    {
        return -1;
    }
    else
    {
        MultiAxCirc[Group].Vinit = MultiAx[Group].Vinit;
        MultiAxCirc[Group].Tacc = MultiAx[Group].Tacc;
        MultiAxCirc[Group].Vtar = MultiAx[Group].Vtar;
        MultiAxCirc[Group].Acce = MultiAx[Group].Acce;

        MultiAx[Group].AxNum = AxNum;
        MultiAxCirc[Group].AxNum = MultiAx[Group].AxNum;
        MultiAxCirc[Group].execute = 1;
        return 0;
    }
}

//计算整圆信息不触发
s32 HZ_MultiAxCalcCirc(u8 Group, u8 AxNum, PointDef Pinit, PointDef Pmid, PointDef Ptar, u8 rot)
{
    if(Group > 4 || AxNum < 2 || AxNum > 5)
    {
        return -1;
    }
    //by li 20190618
    for(u8 i = 0; i < 4; i++)
        MultiAxCirc[Group].Axis[i] = MultiAx[Group].Axis[i];
    //by yang
    for(int i = 0 ; i < AxNum; i++)
    {
        if(A_STA(MultiAx[Group].Axis[i]) != 0)
        {
            return -1;
        }
    }
    Circle[Group].p[0].x = Pinit.x;
    Circle[Group].p[0].y = Pinit.y;
    Circle[Group].p[0].z = Pinit.z;
    Circle[Group].p[0].w = Pinit.w;

    Circle[Group].p[1].x = Pmid.x;
    Circle[Group].p[1].y = Pmid.y;
    Circle[Group].p[1].z = Pmid.z;
    Circle[Group].p[1].w = Pmid.w;

    Circle[Group].p[2].x = Ptar.x;
    Circle[Group].p[2].y = Ptar.y;
    Circle[Group].p[2].z = Ptar.z;
    Circle[Group].p[2].w = Ptar.w;

    Circle[Group].arc = 0;
    Circle[Group].z = Circle[Group].p[2].z - Circle[Group].p[0].z;
    Circle[Group].w = Circle[Group].p[2].w - Circle[Group].p[0].w;
    Circle[Group].rot = rot;
    Circle[Group].arc = 2 * M_PI;
    Circle[Group].allcircle = 1;
    ArcCalc(Group);
    if((Circle[Group].p[2].x == Circle[Group].p[0].x && Circle[Group].p[2].y == Circle[Group].p[0].y && Circle[Group].allcircle == 0)
            || (Circle[Group].p[1].x == Circle[Group].p[0].x && Circle[Group].p[1].y == Circle[Group].p[0].y)
            || Circle[Group].r == 0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


//设置整圆信息及触发
s32 HZ_MultiAxMoveCirc(u8 Group, u8 AxNum, PointDef Pmid, PointDef Ptar, u8 rot)
{
    if(Group > 4 || AxNum < 2 || AxNum > 5)
    {
        return -1;
    }
    //by li 20190618
    for(u8 i = 0; i < 4; i++)
        MultiAxCirc[Group].Axis[i] = MultiAx[Group].Axis[i];
    //by yang
    for(int i = 0 ; i < AxNum; i++)
    {
        if(A_STA(MultiAx[Group].Axis[i]) != 0)
        {
            return -1;
        }
    }
    Circle[Group].p[0].x = HZ_AxGetCurPos(MultiAx[Group].Axis[0]);
    Circle[Group].p[0].y = HZ_AxGetCurPos(MultiAx[Group].Axis[1]);
    Circle[Group].p[0].z = HZ_AxGetCurPos(MultiAx[Group].Axis[2]);
    Circle[Group].p[0].w = HZ_AxGetCurPos(MultiAx[Group].Axis[3]);

    Circle[Group].p[1].x = Pmid.x;
    Circle[Group].p[1].y = Pmid.y;
    Circle[Group].p[1].z = Pmid.z;
    Circle[Group].p[1].w = Pmid.w;

    Circle[Group].p[2].x = Ptar.x;
    Circle[Group].p[2].y = Ptar.y;
    Circle[Group].p[2].z = Ptar.z;
    Circle[Group].p[2].w = Ptar.w;

    Circle[Group].arc = 0;
    Circle[Group].z = Circle[Group].p[2].z - Circle[Group].p[0].z;
    Circle[Group].w = Circle[Group].p[2].w - Circle[Group].p[0].w;
    Circle[Group].rot = rot;
    Circle[Group].arc = 2 * M_PI;
    Circle[Group].allcircle = 1;
    ArcCalc(Group);
    if((Circle[Group].p[2].x == Circle[Group].p[0].x && Circle[Group].p[2].y == Circle[Group].p[0].y && Circle[Group].allcircle == 0)
            || (Circle[Group].p[1].x == Circle[Group].p[0].x && Circle[Group].p[1].y == Circle[Group].p[0].y)
            || Circle[Group].r == 0)
    {
        return -1;
    }
    else
    {
        MultiAxCirc[Group].Vinit = MultiAx[Group].Vinit;
        MultiAxCirc[Group].Tacc = MultiAx[Group].Tacc;
        MultiAxCirc[Group].Vtar = MultiAx[Group].Vtar;
        MultiAxCirc[Group].Acce = MultiAx[Group].Acce;
        MultiAx[Group].AxNum = AxNum;
        MultiAxCirc[Group].AxNum = AxNum;
        MultiAxCirc[Group].execute = 1;
        return 0;
    }
}



float HZ_GetCircR(u8 Group)
{
    return Circle[Group].r;
}

float HZ_GetCircRx(u8 Group)
{
    return Circle[Group].rxy.x;
}

float HZ_GetCircRy(u8 Group)
{
    return Circle[Group].rxy.y;
}

float HZ_GetCircArc(u8 Group)
{
    return Circle[Group].arc;
}

float HZ_GetCircRot(u8 Group)
{
    return Circle[Group].rot;
}

//当前点
mP2Def HZ_GetCircRatioPoint(u8 Group, float ratio)
{
    mP2Def Pxy;
    float trad;
    if(Circle[Group].rot == CCW)
    {
        trad = Circle[Group].p[2].rad - ratio * Circle[Group].arc;
    }
    else
    {
        trad = Circle[Group].p[2].rad + ratio * Circle[Group].arc;
    }

    Pxy.x =  Circle[Group].r * cos(trad) + Circle[Group].rxy.x;
    Pxy.y =  Circle[Group].r * sin(trad) + Circle[Group].rxy.y;
    return Pxy;
}

//当前点
mP2Def HZ_GetCircMiddlePoint(u8 Group, PointDef Pinit, PointDef Ptar, u8 rot)
{
    mP2Def Pxy;
    float trad[2];
    trad[0] = atan2(Pinit.y - Circle[Group].rxy.y, Pinit.x - Circle[Group].rxy.x);
    trad[1] = atan2(Ptar.y - Circle[Group].rxy.y, Ptar.x - Circle[Group].rxy.x);

    if(rot == 0)
    {
        if(Circle[Group].rot == CCW)
        {
            if(trad[1] < trad[0])
                trad[1] += 2 * M_PI;
        }
        else
        {
            if(trad[1] > trad[0])
                trad[1] -= 2 * M_PI;
        }
    }
    else
    {
        if(Circle[Group].rot == CCW)
        {
            if(trad[1] > trad[0])
                trad[1] -= 2 * M_PI;
        }
        else
        {
            if(trad[1] < trad[0])
                trad[1] += 2 * M_PI;
        }
    }
    trad[0] = trad[0] / 2;
    trad[1] = trad[1] / 2;
    trad[0] += trad[1];

    Pxy.x =  Circle[Group].r * cos(trad[0]) + Circle[Group].rxy.x;
    Pxy.y =  Circle[Group].r * sin(trad[0]) + Circle[Group].rxy.y;
    return Pxy;
}



//设置直线信息及触发
//参数：组 ，轴数 ，对应的目标位置
s32 HZ_MultiAxMove(u8 Group, ...)
{
    va_list ap;
    va_start(ap, Group);
    if(Group > 4)
    {
        return -1;
    }
    int axnum = va_arg(ap, int);
    //by yang
    for(u8 i = 0; i < axnum; i++)
    {
        if(A_STA(MultiAxLine[Group].Axis[i]) != 0)
            return -1;
    }
    for(u8 i = 0; i < axnum; i++)
    {
        MultiAxLine[Group].InitPos[i] = HZ_AxGetCurPos(MultiAx[Group].Axis[i]);
        MultiAxLine[Group].TarPos[i] = va_arg(ap, int);
    }
    //MultiAxLine[Group].TarPos[0] = va_arg(ap,int);
    //MultiAxLine[Group].TarPos[1] = va_arg(ap,int);
    //MultiAxLine[Group].TarPos[2] = va_arg(ap,int);
    //MultiAxLine[Group].TarPos[3] = va_arg(ap,int);
    va_end(ap);

    MultiAxLine[Group].Pos_X = MultiAxLine[Group].TarPos[0] - MultiAxLine[Group].InitPos[0];
    MultiAxLine[Group].Pos_Y = MultiAxLine[Group].TarPos[1] - MultiAxLine[Group].InitPos[1];
    MultiAxLine[Group].Pos_Z = MultiAxLine[Group].TarPos[2] - MultiAxLine[Group].InitPos[2];
    MultiAxLine[Group].Pos_W = MultiAxLine[Group].TarPos[3] - MultiAxLine[Group].InitPos[3];

    //MultiAxLine[Group].Pos_XY = mresultant(MultiAxLine[Group].Pos_X, MultiAxLine[Group].Pos_Y);
    if(axnum <= 2)
        MultiAxLine[Group].Pos_XY = mresultant(MultiAxLine[Group].Pos_X, MultiAxLine[Group].Pos_Y);
    else
        MultiAxLine[Group].Pos_XY = mresultant_xyz(MultiAxLine[Group].Pos_X, MultiAxLine[Group].Pos_Y, MultiAxLine[Group].Pos_Z);


    //20190424
    //插补不允许前两个轴的位置同时为零
    if(MultiAxLine[Group].Pos_XY > 0)
    {
        MultiAxLine[Group].Vinit = MultiAx[Group].Vinit;
        MultiAxLine[Group].Tacc = MultiAx[Group].Tacc;
        MultiAxLine[Group].Vtar = MultiAx[Group].Vtar;
        MultiAxLine[Group].Acce = MultiAx[Group].Acce;
        for(u8 i = 0; i < 4; i++)
            MultiAxLine[Group].Axis[i] = MultiAx[Group].Axis[i];
        MultiAx[Group].AxNum = axnum;
        MultiAxLine[Group].AxNum = axnum;
        MultiAxLine[Group].execute = 1;
        for(u8 i = 0; i < axnum; i++)
        {
            A_STA(MultiAxLine[Group].Axis[i]) = AXSTA_SYNC;
        }
        return 0;
    }
    else
    {
        return -1;
    }

}

///**************************************************插补重写*************************************************************/////
typedef enum	//轴动模式选择
{
    ABSMODE,//0
    RELMODE,//1
    SPDMODE,//2
    GOHOMEMODE,//3
    STOPMODE,	//4
} MoveMode;
/**
* @author 18/12/1  NYZ
* @Description: 把脉冲数转换程用户单位，例如，脉冲转换程毫米
* @param -- 轴号 脉冲数
* @param --
* @return -- 距离或角度 等用户单位
*/
static float PulseToUserUnit(AxisGroupDataDef p, int num, s32 Dat)
{
    return (((float)Dat * (float)p.MPR[num]) / (float)p.PPR[num]);
}

/**
* @author 18/12/1  NYZ
* @Description: 把用户单位转换成脉冲数，例如，毫米->脉冲数
* @param -- 轴号 单位距离
* @param --  u32 PPR[3];		//每转脉冲数 3个轴
			float MPR[3];	//每转毫米__导程
* @return -- 脉冲数
*/
static s32	UserUnitToPulse(AxisGroupDataDef p, int num, float Dat)
{
    return (s32)(Dat * (float)p.PPR[num] / p.MPR[num]);
}


/**
* @author 18/12/1  NYZ
* @Description: 以用户单位移动轴
* @param -- 轴号 模式 速度比例 位置
* @param --
* @return --
*/
static void MC_MoveUU(u8 i, u32 mode, u32 StartSpeed, u32 Acctime, u32 TargtSpeed, float TargetPos, AxisGroupDataDef p)
{
    if(StartSpeed < 50)
    {
        StartSpeed = 50;
    }
    if(StartSpeed > TargtSpeed)
    {
        TargtSpeed = StartSpeed + 1;
    }

    HZ_AxSetPara(i,  StartSpeed, Acctime, \
                 TargtSpeed, \
                 Acctime, StartSpeed, \
                 StartSpeed, TargtSpeed, \
                 UserUnitToPulse(p, i, TargetPos), 0, 0);

    switch(mode)
    {
    case ABSMODE:
        HZ_AxMoveAbs(i, UserUnitToPulse(p, i, TargetPos));
        break;
    case RELMODE:
        HZ_AxMoveRel(i, UserUnitToPulse(p, i, TargetPos));
//				Multi_AxMoveRel(i, UserUnitToPulse(&GSS.Conversion[i], TargetPos));
        break;

    }
}
/**
* @author 19/9/8  NYZ
* @Description: XY轴相对移动函数，用户单位
* @param --
* @param --
* @return --
*/
static s8 MoveXYuuRel(u8 xAx, u8 yAx, u8 zAx,  u32 xSpeed, u32 ySpeed, u32 zSpeed, int XTargetPos, int YTargetPos, int ZTargetPos)
{
//    float xSpeedf, ySpeedf;
//		int xTrgPos, yTrgPos;
//    xSpeedf = speed * fabs(XTargetPos) / sqrt(XTargetPos * XTargetPos + YTargetPos * YTargetPos);
//    ySpeedf = speed * fabs(YTargetPos) / sqrt(XTargetPos * XTargetPos + YTargetPos * YTargetPos);

//		if(XTargetPos != 0)
//		{
//			MC_MoveUU(xAx, ABSMODE, xSpeed, 20, xSpeed,GSR.AxisPosition[xAx] + XTargetPos);
//		}
//		if(YTargetPos != 0)
//		{
//			MC_MoveUU(yAx, ABSMODE, ySpeed, 20, ySpeed,GSR.AxisPosition[yAx] + YTargetPos);
//		}
    if(XTargetPos != 0)
    {
        Puls_SetSpd(xAx, xSpeed );
        if(XTargetPos < 0)
        {
            Dir_SetSta(xAx, 0);
            A_DIR(xAx) = BACK_WARD;
        }
        else
        {
            Dir_SetSta(xAx, 1);
            A_DIR(xAx) = FOR_WARD;
        }
        A_POS_CUR(xAx) += XTargetPos;
        Puls_Start(xAx, abs(XTargetPos));
    }

    if(YTargetPos != 0)
    {
        Puls_SetSpd(yAx, ySpeed);
        if(YTargetPos < 0)
        {
            Dir_SetSta(yAx, 0);
            A_DIR(yAx) = BACK_WARD;
        }
        else
        {
            Dir_SetSta(yAx, 1);
            A_DIR(yAx) = FOR_WARD;
        }
        Puls_Start(yAx, abs(YTargetPos));
        A_POS_CUR(yAx) += YTargetPos;
    }

    if(ZTargetPos != 0)
    {
        Puls_SetSpd(zAx, zSpeed);
        if(ZTargetPos < 0)
        {
            Dir_SetSta(zAx, 0);
            A_DIR(zAx) = BACK_WARD;
        }
        else
        {
            Dir_SetSta(zAx, 1);
            A_DIR(zAx) = FOR_WARD;
        }
        Puls_Start(zAx, abs(ZTargetPos));
        A_POS_CUR(zAx) += ZTargetPos;
    }
    return  1;
}

/**
* @author 19/9/8  NYZ
* @Description: XY轴绝对移动函数，用户单位
* @param --
* @param --
* @return --
*/
s8 MoveXYuuABS(u8 xAx, u8 yAx, u32 xSpeed, u32 ySpeed, float XTargetPos, float YTargetPos, AxisGroupDataDef p)
{
//    float xSpeedf, ySpeedf;
//		int xTrgPos, yTrgPos;
//    xSpeedf = speed * fabs(XTargetPos) / sqrt(XTargetPos * XTargetPos + YTargetPos * YTargetPos);
//    ySpeedf = speed * fabs(YTargetPos) / sqrt(XTargetPos * XTargetPos + YTargetPos * YTargetPos);

    MC_MoveUU(xAx, ABSMODE, xSpeed, 20, xSpeed, XTargetPos, p);
    MC_MoveUU(yAx, ABSMODE, ySpeed, 20, ySpeed, YTargetPos, p);
    return  1;
}

/**
* @author 19/9/8  NYZ
* @Description: 插补直线加速度
* @param --
* @param --
* @return --
*/
float XYSpeedAcc(SpdDataDef* spdPara, float length, float currLen)
{
    float accLen, decLen;
    float accT, decT;
    float currSpd;
    float t;
    float a, b, c;
    accT = (spdPara->speed - spdPara->startSpeed) / spdPara->acc;
    decT = (spdPara->speed - spdPara->endSpeed) / spdPara->dec;
    accLen = spdPara->startSpeed * accT + 0.5f * spdPara->speed * accT;
    decLen = spdPara->endSpeed * decT + 0.5f * spdPara->speed * decT;

    while(length < accLen + decLen && spdPara->speed > spdPara->startSpeed)
    {
        spdPara->speed -= 1.0f;
        accT = (spdPara->speed - spdPara->startSpeed) / spdPara->acc;
        decT = (spdPara->speed - spdPara->endSpeed) / spdPara->dec;
        accLen = spdPara->startSpeed * accT + 0.5f * spdPara->speed * accT;
        decLen = spdPara->endSpeed * decT + 0.5f * spdPara->speed * decT;
    }



    if(spdPara->speed < spdPara->startSpeed)
    {
        currSpd = spdPara->startSpeed;
    }
    else
    {
        if(currLen <= accLen)
        {
            //currLen = startSpeed*t + 0.5f*acc*t*t;
            a = 0.5f * spdPara->acc;
            b = spdPara->startSpeed;
            c = -1.0f * currLen;
            t =  (-1.0f * b + sqrt(b * b - 4 * a * c)) / (2.0f * a);
            currSpd = spdPara->startSpeed + spdPara->acc * t;

        }
        else if(currLen >= accLen && currLen <= (length - decLen))
        {
            currSpd = spdPara->speed;
        }
        else if(currLen > (length - decLen))
        {
            //currLen = speed*t - 0.5f*dec*t*t; currLen = startSpeed*t + 0.5f*acc*t*t;
            a = 0.5f * spdPara->dec;
            b = spdPara->endSpeed;
            c = -1.0f * (length - currLen);
            t =  (-1.0f * b + sqrt(b * b - 4 * a * c)) / (2.0f * a);
            currSpd = spdPara->endSpeed + spdPara->dec * t;
            if(currSpd > spdPara->speed)
            {
                currSpd = spdPara->speed;
            }
        }
        else
        {
            currSpd = spdPara->endSpeed ;
        }

        if(currSpd < spdPara->endSpeed)
        {
            currSpd = spdPara->endSpeed;
        }
    }

    if(currSpd > spdPara->speed)
    {
        currSpd = spdPara->speed;
    }

    if(currSpd < 5.0f)
    {
        currSpd = 5.0f;
    }
    return currSpd;
}

/**
* @author 19/9/8  NYZ
* @Description: XY轴直线插补，用户单位
* @param --     X轴号，Y轴号
* @param --     速度，MM/S； 加速度，MM/s2
* @return --
*/
void MoveLineXYRun(AxisGroupDataDef* LIn)
{
    static float l, dl;
    float t;
    if(LIn->step == 0 && LIn->execute == 1)
    {
        LIn->step = 1;
        LIn->done = 0;
        LIn->busy = 1;
        LIn->stopFlag = 0;
		LIn->currLen = 0;
        LIn->index = 0;
    }


    switch(LIn->step)
    {
    case 1:
        LIn->length = sqrt((LIn->p2.x - LIn->p1.x) * (LIn->p2.x - LIn->p1.x) + (LIn->p2.y - LIn->p1.y) * (LIn->p2.y - LIn->p1.y) + (LIn->p2.z - LIn->p1.z) * (LIn->p2.z - LIn->p1.z));
//        angle = atan(fabs(LIn->p2.y - LIn->p1.y) / fabs(LIn->p2.x - LIn->p1.x));

        if(LIn->length > 10.0f)
        {
            LIn->InterDiv = LINEDIV;

        }
        else
        {
            LIn->InterDiv = LINEDIV / 10.0f;
            LIn->SpeedSet.startSpeed = LIn->SpeedSet.startSpeed / 2.0f;
            LIn->SpeedSet.endSpeed = LIn->SpeedSet.startSpeed;
        }

        LIn->xDiv = LIn->InterDiv * (LIn->p2.x - LIn->p1.x) / LIn->length;
        LIn->yDiv = LIn->InterDiv * (LIn->p2.y - LIn->p1.y) / LIn->length;
        LIn->zDiv = LIn->InterDiv * (LIn->p2.z - LIn->p1.z) / LIn->length;
        LIn->currP = LIn->p1;
        LIn->preP = LIn->p1;
        LIn->step = 2;
        break;

    case 2:
        //计算已走曲线长度
        LIn->currLen = sqrt((LIn->p1.x - LIn->currP.x) * (LIn->p1.x - LIn->currP.x) + (LIn->p1.y - LIn->currP.y) * (LIn->p1.y - LIn->currP.y)  + (LIn->p1.z - LIn->currP.z) * (LIn->p1.z - LIn->currP.z));
        //计算运行速度
        LIn->currSpeed =  XYSpeedAcc(&LIn->SpeedSet, LIn->length, LIn->currLen);
        //计算两点之间长度
        l = sqrt((LIn->p2.x - LIn->currP.x) * (LIn->p2.x - LIn->currP.x) + (LIn->p2.y - LIn->currP.y) * (LIn->p2.y - LIn->currP.y) + (LIn->p2.z - LIn->currP.z) * (LIn->p2.z - LIn->currP.z));

        if(l >= LIn->InterDiv)
        {

            LIn->currP.x += LIn->xDiv;
            LIn->currP.y += LIn->yDiv;
            LIn->currP.z += LIn->zDiv;
            LIn->endFlag = 0;

        }
        else
        {
            LIn->currP = LIn->p2;
            LIn->endFlag = 1;
        }

        LIn->xTrgPos =	LIn->currP.x - PulseToUserUnit(*LIn, 0, HZ_AxGetCurPos(LIn->xAx)); //GSR.AxisUnitPosition[LIn->xAx];
        LIn->yTrgPos =  LIn->currP.y - PulseToUserUnit(*LIn, 1, HZ_AxGetCurPos(LIn->yAx)); //GSR.AxisUnitPosition[LIn->yAx];
        LIn->zTrgPos =  LIn->currP.z - PulseToUserUnit(*LIn, 2, HZ_AxGetCurPos(LIn->zAx)); //GSR.AxisUnitPosition[LIn->zAx];

        dl = sqrt(LIn->xTrgPos * LIn->xTrgPos + LIn->yTrgPos * LIn->yTrgPos + LIn->zTrgPos * LIn->zTrgPos);
        t = dl / LIn->currSpeed;
        LIn->xTrgPosPP =  UserUnitToPulse(*LIn, 0, LIn->xTrgPos);
        LIn->yTrgPosPP =  UserUnitToPulse(*LIn, 1, LIn->yTrgPos);
        LIn->zTrgPosPP =  UserUnitToPulse(*LIn, 2, LIn->zTrgPos);

//        if(abs(LIn->xTrgPosPP + LIn->xTrgPosBuf) < 5)
//        {
//            LIn->xTrgPosBuf += LIn->xTrgPosPP;
//            LIn->xTrgPosPP = 0;
//        }
//        else
//        {
//            LIn->xTrgPosPP  += LIn->xTrgPosBuf;
//            LIn->xTrgPosBuf = 0;
//        }


//        if(abs(LIn->yTrgPosPP + LIn->yTrgPosBuf) < 5)
//        {
//            LIn->yTrgPosBuf += LIn->yTrgPosPP;
//            LIn->yTrgPosPP = 0;
//        }
//        else
//        {
//            LIn->yTrgPosPP += LIn->yTrgPosBuf;
//            LIn->yTrgPosBuf = 0;
//        }
//
//        if(abs(LIn->zTrgPosPP + LIn->zTrgPosBuf) < 5)
//        {
//            LIn->zTrgPosBuf += LIn->zTrgPosPP;
//            LIn->zTrgPosPP = 0;
//        }
//        else
//        {
//            LIn->zTrgPosPP += LIn->zTrgPosBuf;
//            LIn->zTrgPosBuf = 0;
//        }

        LIn->xTgSpd = (u32)(fabs((float)LIn->xTrgPosPP) / t);
        if(LIn->xTgSpd < 200)
        {
            LIn->xTgSpd = 200;
        }
        LIn->yTgSpd = (u32)(fabs((float)LIn->yTrgPosPP) / t);
        if(LIn->yTgSpd < 200)
        {
            LIn->yTgSpd = 200;
        }
        LIn->zTgSpd = (u32)(fabs((float)LIn->zTrgPosPP) / t);
        if(LIn->zTgSpd < 200)
        {
            LIn->zTgSpd = 200;
        }

        LIn->step = 3;
        break;

    case 3:
        if(0 == Puls_GetSta(LIn->xAx) && 0 == Puls_GetSta(LIn->yAx) && 0 == Puls_GetSta(LIn->zAx))
        {
            MoveXYuuRel(LIn->xAx, LIn->yAx, LIn->zAx, LIn->xTgSpd, LIn->yTgSpd, LIn->zTgSpd, LIn->xTrgPosPP, LIn->yTrgPosPP, LIn->zTrgPosPP);
            if(LIn->endFlag == 0)
            {
                if(LIn->stopFlag == 1)
                {
                    LIn->step = 0xA0;
                }
                else
                {
                    LIn->step = 2;
                }
            }
            else
            {
                LIn->step = 4;
            }
        }
        break;


    case 0xA0:
        if(LIn->stopFlag == 0)
        {
            LIn->length = LIn->length - LIn->currLen;
            LIn->currLen = 0;
            LIn->p1 = LIn->currP;
            LIn->step = 2;
        }
        break;

    case 4:
//        if(HZ_AxGetStatus(LIn->xAx) == 0 && HZ_AxGetStatus(LIn->yAx) == 0)
//			A_POS_CUR(LIn->xAx) = A_POS_LST(LIn->xAx) +  Puls_GetRan(LIn->xAx) * A_DIR(LIn->xAx);
//			A_POS_CUR(LIn->yAx) = A_POS_LST(LIn->yAx) +  Puls_GetRan(LIn->yAx) * A_DIR(LIn->yAx);;
        if(0 == Puls_GetSta(LIn->xAx) && 0 == Puls_GetSta(LIn->yAx) && 0 == Puls_GetSta(LIn->zAx) && HZ_AxGetStatus(LIn->xAx) == 0 && HZ_AxGetStatus(LIn->yAx) == 0 && HZ_AxGetStatus(LIn->zAx) == 0)
        {
            LIn->step = 0;
            LIn->done = 0;
            LIn->busy = 0;
            LIn->execute = 0;
            A_STA(LIn->xAx) = AXSTA_READY;
            A_STA(LIn->yAx) = AXSTA_READY;
			A_STA(LIn->zAx) = AXSTA_READY;
        }
        break;
    }
}


/**
* @author 19/9/8  NYZ
* @Description: 执行XY直线插补
* @param --     X轴号，Y轴号
* @param --     速度，MM/S； 加速度，MM/s2
* @return --
*/
s8 AxGroupMoveXYLine(u8 xAixs, u8 yAxis, u8 zAxis, PointfDef p1, PointfDef p2, float startSpeed, float acc, float speed, float dec, float endSpeed, AxisGroupDataDef* LIn)
{
    if(LIn->execute == 0)
    {
        LIn->p1 = p1;
        LIn->p2 = p2;
        LIn->SpeedSet.startSpeed = startSpeed;
        LIn->SpeedSet.acc = acc;
        LIn->SpeedSet.speed = speed;
        LIn->SpeedSet.dec = dec;
        LIn->SpeedSet.endSpeed = endSpeed;
        LIn->xAx = xAixs;
        LIn->yAx = yAxis;
        LIn->zAx = zAxis;
        LIn->stopFlag = 0;
        LIn->execute = 1;
//        A_STA(0) = AXSTA_SYNC;
//        A_STA(1) = AXSTA_SYNC;
        LIn->mode = LINEMODE;
        return 1;
    }
    else
    {
        return -1;
    }

}


/**
* @author 19/9/8  NYZ
* @Description: 获取轴组状态
* @param --
* @param --
* @return --   0:插补结束  1：插补运行中
*/
s8 AxGroupGetSta(AxisGroupDataDef LIn)
{
    return LIn.execute;
}



u32 mresultant_xyz1(s32 a, s32 b, s32 c)
{
    u32 r;
    a = (a >= 0 ? a : -a);
    b = (b >= 0 ? b : -b);
    c = (c >= 0 ? c : -c);
    r = sqrtf(1.0f * a * a + 1.0f * b * b +  1.0f * c * c);
    return r;
}

// 弧度计算
float RadianCal1(PointfDef p, PointfDef center)
{
    float radian;
    radian = atan2(p.y - center.y, p.x - center.x);
    return radian;
}
// 方向计算
s8 CirDirCal(float rad1, float rad2, float rad3)
{
    s8 rot;
    if(rad2 < rad1) rad2 = rad2 + 2 * M_PI;
    if(rad3 < rad1) rad3 = rad3 + 2 * M_PI;
    rot = ((rad3 - rad1) > (rad2 - rad1) ? CCW : CW);
    return rot;
}

// 方向计算
s8 CirDirCal1(float rad1, float rad2, float rad3)
{
    s8 rot;
    if(rad2 < rad1) rad2 = rad2 + 2 * M_PI;
    if(rad3 < rad1) rad3 = rad3 + 2 * M_PI;
    rot = ((rad3 - rad1) > (rad2 - rad1) ? CCW : CW);
    return rot;
}

// 方向计算
s8 CirDir(PointfDef p1, PointfDef p2, PointfDef p3)
{
//    PointfDef p12,p23;
    float v;
//	  p12.x = p2.x - p1.x;
//		p12.y = p2.y - p1.y;
//
//		p23.x = p3.x - p2.x;
//		p23.y = p3.y - p2.y;

    v = (p2.x - p1.x) * (p3.y - p2.y) - (p2.y - p1.y) * (p3.x - p2.x);

    if(v > 0.0001f)
    {
        return CW;
    }
    else
    {
        return CCW;
    }
}

float AngleCal(PointfDef p1, PointfDef p2)
{
    float angle;
    float v = atan2(p2.y - p1.y, p2.x - p1.x);

    if(v > 0.0001f)
    {
        angle = v * 360.0f / (2.0f * M_PI);
    }
    else
    {
        angle = 360.0f + v * 360.0f / (2.0f * M_PI);
    }
    return angle;
}

// 3点计算圆信息
CircleDefNyz ThreePointToCirc1(PointfDef p1, PointfDef p2, PointfDef p3)
{
    float a, b, c, d, delta, m, n;
    float te, tf, tg;
    float fMax, fmin;
    CircleDefNyz circle;
    a = p2.x - p1.x;
    b = p2.y - p1.y;
    c = p3.x - p1.x;
    d = p3.y - p1.y;
    delta = a * d - b * c;
    m = (1.0f * p1.x * p1.x + 1.0f * p1.y * p1.y) - (1.0f * p2.x * p2.x + 1.0f * p2.y * p2.y);
    n = (1.0f * p1.x * p1.x + 1.0f * p1.y * p1.y) - (1.0f * p3.x * p3.x + 1.0f * p3.y * p3.y);
    if(fabs(delta) > 0.001)
    {
        te = (m / (float)delta) * d - (n / (float)delta) * b;
        tf = (n / (float)delta) * a - (m / (float)delta) * c;
        tg = -(1.0f * p1.x * te + 1.0f * p1.y * tf + 1.0f * p1.x * p1.x + 1.0f * p1.y * p1.y);

        circle.center.x = -te / 2;
        circle.center.y = -tf / 2;
        circle.r = sqrtf(te * te + tf * tf - 4 * tg) / 2;
        circle.p1Angle = AngleCal(circle.center, p1);
        circle.p2Angle = AngleCal(circle.center, p2);
        circle.p3Angle = AngleCal(circle.center, p3);

//				circle.p1Rad = atan2(p1.y - circle.center.y, p1.x - circle.center.x);
//				circle.p2Rad = atan2(p2.y - circle.center.y, p2.x - circle.center.x);
//				circle.p3Rad = atan2(p3.y - circle.center.y, p3.x - circle.center.x);

//			  circle.dir =  CirDirCal(circle.startRad, circle.midRad, circle.endRad);
        circle.dir =  CirDir( p1, p2, p3);

        if(circle.p3Angle > circle.p1Angle)
        {
            fMax = circle.p3Angle;
            fmin = circle.p1Angle;
        }
        else
        {
            fMax = circle.p1Angle;
            fmin = circle.p3Angle;
        }

        if(circle.p2Angle < fMax  && circle.p2Angle > fmin)
        {
            circle.angle = fMax - fmin;
        }
        else
        {
            circle.angle = 360 - (fMax - fmin);
        }
    }
    else
    {
        PARAINIT(circle);
    }
    return circle;
}




// 圆弧坐标点象限判断
s8 CirQuadrantCal(PointfDef p, PointfDef center)
{
    s8 q = 0;
    float x, y;
    x = p.x - center.x;
    y = p.y - center.y;
    if(x > 0.001f && y > 0.001f )
    {
        q = 0;
    }
    else if(x < 0.001f && y > 0.001f )
    {
        q = 1;
    }
    else if(x < 0.001f && y < 0.001f )
    {
        q = 2;
    }
    else if(x > 0.001f && y < 0.001f )
    {
        q = 3;
    }

    return q;
}

/**
* @author 19/9/8  NYZ
* @Description: XY轴圆弧插补，用户单位
* @param --     X轴号，Y轴号
* @param --     速度，MM/S； 加速度，MM/s2
* @return --
*/
void MoveCirXYRun(AxisGroupDataDef* LIn)
{
    static float dl;
    float t;
    if(LIn->step == 0 && LIn->execute == 1)
    {
        LIn->step = 1;
        LIn->done = 0;
        LIn->busy = 1;
        LIn->endFlag = 0;
        LIn->stopFlag = 0;
        LIn->index = 0;
        LIn->xTrgPosBuf = 0;
        LIn->xTrgPosBuf = 0;
		LIn->currLen = 0;
    }


    switch(LIn->step)
    {
    case 1:
        //计算圆参数，圆心、半径
        LIn->Circle =	ThreePointToCirc1(LIn->p1, LIn->p2, LIn->p3);
        if(LIn->mode == CIRCLEMODE)
        {
            LIn->Circle.angle = 360.0f;
        }

        LIn->Circle.curAngle = LIn->Circle.p1Angle;
        LIn->Circle.relAngle = 0;
        //计算圆弧长度
        LIn->length = (LIn->Circle.angle - LIn->Circle.relAngle) * (2.0f * M_PI) / 360.0f * LIn->Circle.r;
        LIn->currP = LIn->p1;
        LIn->step = 2;
        break;

    case 2:
        LIn->InterDiv = CIRCLEDIV / LIn->Circle.r * 360.0f / (2.0f * M_PI);
        LIn->preP.x = PulseToUserUnit(*LIn, 0, HZ_AxGetCurPos(LIn->xAx)); //GSR.AxisUnitPosition[LIn->xAx];
        LIn->preP.y = PulseToUserUnit(*LIn, 1, HZ_AxGetCurPos(LIn->yAx)); //GSR.AxisUnitPosition[LIn->yAx];
//    LIn->preP = LIn->currP;
        if(fabs(LIn->Circle.relAngle - LIn->Circle.angle) > LIn->InterDiv)
        {
            LIn->Circle.relAngle += LIn->InterDiv;

            if(LIn->Circle.dir == CW)
            {
                LIn->Circle.curAngle += LIn->InterDiv;
            }
            else
            {
                LIn->Circle.curAngle -= LIn->InterDiv;
            }
            if(LIn->Circle.curAngle < 0.0001f)
            {
                LIn->Circle.curAngle = 360.0f + LIn->Circle.curAngle;
            }
            else if(LIn->Circle.curAngle > 360.0f)
            {
                LIn->Circle.curAngle = LIn->Circle.curAngle - 360.0f;
            }
            //计算插补当前点
            LIn->currP.x = LIn->Circle.r * cos(LIn->Circle.curAngle * (2.0f * M_PI) / 360.0f) + LIn->Circle.center.x;
            LIn->currP.y = LIn->Circle.r * sin(LIn->Circle.curAngle * (2.0f * M_PI) / 360.0f) + LIn->Circle.center.y;

        }
        else
        {

            if(LIn->mode == CIRCLEMODE)
            {
                LIn->currP = LIn->p1;
            }
            else
            {
                LIn->currP = LIn->p3;
            }
            LIn->endFlag = 1;
        }


        //计算已走曲线长度
        LIn->currLen = LIn->Circle.relAngle * (2.0f * M_PI) / 360.0f * LIn->Circle.r;
        //计算运行速度
        LIn->currSpeed =  XYSpeedAcc(&LIn->SpeedSet, LIn->length, LIn->currLen);
        LIn->xTrgPos = LIn->currP.x - LIn->preP.x;
        LIn->yTrgPos = LIn->currP.y - LIn->preP.y;
//    LIn->xTrgPos = LIn->currP.x - GSR.AxisUnitPosition[LIn->xAx];
//    LIn->yTrgPos = LIn->currP.y - GSR.AxisUnitPosition[LIn->yAx];
        dl = sqrt(LIn->xTrgPos * LIn->xTrgPos + LIn->yTrgPos * LIn->yTrgPos);
        t = dl / LIn->currSpeed;
        LIn->xTrgPosPP =  UserUnitToPulse(*LIn, 0, LIn->xTrgPos);
        LIn->yTrgPosPP =  UserUnitToPulse(*LIn, 1, LIn->yTrgPos);
//				LIn->xTgSpd = UserUnitToPulse(&GSS.Conversion[LIn->xAx],LIn->currSpeed*fabs(cos(angle)));
//				LIn->yTgSpd = UserUnitToPulse(&GSS.Conversion[LIn->yAx],LIn->currSpeed*fabs(sin(angle)));
        if(abs(LIn->xTrgPosPP + LIn->xTrgPosBuf) < 5)
        {
            LIn->xTrgPosBuf += LIn->xTrgPosPP;
            LIn->xTrgPosPP = 0;
        }
        else
        {
            LIn->xTrgPosPP  += LIn->xTrgPosBuf;
            LIn->xTrgPosBuf = 0;
        }

        if(abs(LIn->yTrgPosPP + LIn->yTrgPosBuf) < 5)
        {
            LIn->yTrgPosBuf += LIn->yTrgPosPP;
            LIn->yTrgPosPP = 0;
        }
        else
        {
            LIn->yTrgPosPP += LIn->yTrgPosBuf;
            LIn->yTrgPosBuf = 0;
        }
		


        LIn->xTgSpd = (u32)(fabs((float)LIn->xTrgPosPP) / t);
        LIn->yTgSpd = (u32)(fabs((float)LIn->yTrgPosPP) / t);
        if(LIn->xTgSpd > LIn->yTgSpd)
        {
            LIn->yTgSpd = abs(LIn->yTrgPosPP) * LIn->xTgSpd / abs(LIn->xTrgPosPP);
        }
        else
        {
            LIn->xTgSpd = abs(LIn->xTrgPosPP) * LIn->yTgSpd / abs(LIn->yTrgPosPP);
        }
        LIn->step = 3;
        break;

    case 3:
//        if(HZ_AxGetStatus(LIn->xAx) == 0 && HZ_AxGetStatus(LIn->yAx) == 0)

        if(0 == Puls_GetSta(LIn->xAx) && 0 == Puls_GetSta(LIn->yAx))
        {
            if(LIn->endFlag == 0)
            {
                MoveXYuuRel(LIn->xAx, LIn->yAx, LIn->zAx, LIn->xTgSpd, LIn->yTgSpd, LIn->zTgSpd, LIn->xTrgPosPP, LIn->yTrgPosPP, 0);
                if(LIn->stopFlag == 1)
                {
                    LIn->step = 0xA0;
                }
                else
                {
                    LIn->step = 2;
                }
            }
            else
            {
                MoveXYuuABS(LIn->xAx, LIn->yAx, LIn->xTgSpd, LIn->yTgSpd, LIn->currP.x, LIn->currP.y, *LIn);
                LIn->step = 4;
            }
        }
        break;


    case 0xA0:
        if(LIn->stopFlag == 0)
        {
            LIn->length = LIn->length - LIn->currLen;
            LIn->currLen = 0;
            LIn->step = 2;
        }
        break;

    case 4:
//        if(HZ_AxGetStatus(LIn->xAx) == 0 && HZ_AxGetStatus(LIn->yAx) == 0)
//			A_POS_CUR(LIn->xAx) = A_POS_LST(LIn->xAx) +  Puls_GetRan(LIn->xAx) * A_DIR(LIn->xAx);
//			A_POS_CUR(LIn->yAx) = A_POS_LST(LIn->yAx) +  Puls_GetRan(LIn->yAx) * A_DIR(LIn->yAx);;
        if(0 == Puls_GetSta(LIn->xAx) && 0 == Puls_GetSta(LIn->yAx) && HZ_AxGetStatus(LIn->xAx) == 0 && HZ_AxGetStatus(LIn->yAx) == 0)
        {
            LIn->step = 0;
            LIn->done = 0;
            LIn->busy = 0;
            LIn->endFlag = 0;
            LIn->execute = 0;
            A_STA(LIn->xAx) = AXSTA_READY;
            A_STA(LIn->yAx) = AXSTA_READY;
        }
        break;
    }
}

/**
* @author 19/9/8  NYZ
* @Description: 执行XY圆弧插补
* @param --     X轴号，Y轴号
* @param --     速度，MM/S； 加速度，MM/s2
* @return --
*/
s8 AxGroupMoveXYCir(u8 xAixs, u8 yAxis, PointfDef p1, PointfDef p2, PointfDef p3, float startSpeed, float acc, float speed, float dec, float endSpeed, AxisGroupDataDef* LIn)
{
    if(LIn->execute == 0)
    {
        LIn->p1 = p1;
        LIn->p2 = p2;
        LIn->p3 = p3;
        LIn->SpeedSet.startSpeed = startSpeed;
        LIn->SpeedSet.acc = acc;

        LIn->SpeedSet.speed = speed;
        LIn->SpeedSet.dec = dec;
        LIn->SpeedSet.endSpeed = endSpeed;
        LIn->xAx = xAixs;
        LIn->yAx = yAxis;
        LIn->mode =  CIRMODE;
        LIn->stopFlag = 0;
        LIn->execute = 1;
//        A_STA(0) = AXSTA_SYNC;
//        A_STA(1) = AXSTA_SYNC;
        return 1;
    }
    else
    {
        return -1;
    }
}



/**
* @author 19/9/8  NYZ
* @Description: 执行XY整圆插补
* @param --     X轴号，Y轴号
* @param --     速度，MM/S； 加速度，MM/s2
* @return --
*/
s8 AxGroupMoveXYCircle(u8 xAixs, u8 yAxis, PointfDef p1, PointfDef p2, PointfDef p3, float startSpeed, float acc, float speed, float dec, float endSpeed, AxisGroupDataDef* LIn)
{
    if(LIn->execute == 0)
    {
        LIn->p1 = p1;
        LIn->p2 = p2;
        LIn->p3 = p3;
        LIn->SpeedSet.startSpeed = startSpeed;
        LIn->SpeedSet.acc = acc;
        LIn->SpeedSet.speed = speed;
        LIn->SpeedSet.dec = dec;
        LIn->SpeedSet.endSpeed = endSpeed;
        LIn->xAx = xAixs;
        LIn->yAx = yAxis;
        LIn->mode =  CIRCLEMODE;
//        A_STA(0) = AXSTA_SYNC;
//        A_STA(1) = AXSTA_SYNC;
        LIn->stopFlag = 0;
        LIn->execute = 1;
        return 1;
    }
    else
    {
        return -1;
    }
}

/**
* @author 19/9/12  插补暂停
* @Description:
* @param --
* @param --
* @return --
*/
s8 AxGroupMoveXYStop(AxisGroupDataDef* LIn)
{
    if(LIn->execute == 1)
    {
        LIn->stopFlag = 1;
        return 1;
    }
    else
    {
        return -1;
    }
}


/**
* @author 19/9/12  插补继续
* @Description:
* @param --
* @param --
* @return --
*/
s8 AxGroupMoveXYContinue(AxisGroupDataDef* LIn)
{
    if(LIn->execute == 1)
    {
        LIn->stopFlag = 0;
        return 1;
    }
    else
    {
        return -1;
    }
}

/**
* @author 19/9/11  NYZ
* @Description: 插补运行
* @param --
* @param --
* @return --
*/
void MoveInterXyRUN(AxisGroupDataDef* LIn)
{
    switch(LIn->mode )
    {
    case LINEMODE:
        MoveLineXYRun(LIn);
        break;

    case CIRMODE:
    case CIRCLEMODE:
        MoveCirXYRun(LIn);
        break;
    }
}
