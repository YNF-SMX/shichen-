#ifndef __MULTIAXIS_H
#define __MULTIAXIS_H

//#include "MultiAxis.h"
#include "BSPlib.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
//#include "interlayer_config.h"
#include "axis.h"

#define M_PI 3.14159f
#define mabs(a) (a>=0?a:-a)									//取正
#define mround(a) (int)(a+0.5f) 							//四舍五入
//#define mresultant(a,b) sqrtf(a*a*1.0f+b*b*1.0f)			//合向量
//#define mresultant_xyz(a,b,c) sqrtf(a*a*1.0f+b*b*1.0f+c*c*1.0f)

#define INPST  2.0f/1000									//插补周期 
#define CCW 0												//逆时针
#define CW 1												//顺时针


#define LINEDIV 1.0f    //直线插补，步距
#define CIRCLEDIV 0.5f  //圆弧插补，步距

#define PARAINIT(a)  memset(&a,0,sizeof(a));

typedef enum
{
    LINEMODE = 1,  		//直线模式
    CIRMODE = 2,		//圆弧模式
    CIRCLEMODE = 3,		//整圆模式
} InterpolationMode;


//插补部分//
//直线插补
typedef struct
{
    float x;
    float y;
    float z;
} PointfDef;

typedef struct
{
    PointfDef center;  //圆心
    float r;      //半径
    float angle;    //圆弧角度
    float p1Angle;  //P1点向量角
    float p2Angle;  //P2点向量角
    float p3Angle;  //P3点向量角
    float dir;  //方向
    float curAngle;  //当前弧度
    float relAngle;  //走过弧度值
} CircleDefNyz;


typedef struct
{
    float startSpeed; //开始速度 MM/S
    float acc;        //加速度, MM/S2
    float speed;
    float dec;
    float endSpeed;
} SpdDataDef;




typedef struct
{
    u8 execute;
    u8 step;
    u8 busy;
    u8 done;
    u8 count;
    u32 index;
    u8 endFlag;  					//插补结束标志
    u8 stopFlag;  				//插补暂停标志
    u8 mode;   						//模式：1：直线  2：整圆   3：圆弧
    u8 currMode;   				//当前模式：1：直线  2：整圆   3：圆弧
    SpdDataDef SpeedSet; 	//速度参数

    float currSpeed; 			//当前速度, MM/s
    u8 xAx;								//X轴号
    u8 yAx;								//Y轴号
    u8 zAx;								//z轴号

    PointfDef p1;   			//第一点坐标,直线点1，圆弧点1
    PointfDef p2;   			//第二点坐标,直线点2，圆弧点2
    PointfDef p3;   			//第二点坐标,圆弧点3

    CircleDefNyz Circle;  		//圆弧、整圆

    PointfDef preP;  			//上一坐标点
    PointfDef currP;  		//当前坐标
    float xDiv;   				//X轴每一补移动量
    float yDiv;   				//Y轴每一补移动量
    float zDiv;   				//z轴每一补移动量

    float xTrgPos; //X轴移动量
    float yTrgPos; //Y轴移动量
    float zTrgPos; //z轴移动量

    float InterDiv; //圆弧插补步距，度

    s32 xTrgPosPP; //X轴移动量, 脉冲量
    s32 yTrgPosPP; //Y轴移动量, 脉冲量
    s32 zTrgPosPP; //z轴移动量, 脉冲量

    s32 xTrgPosBuf; //X轴移动量, 脉冲量
    s32 yTrgPosBuf; //Y轴移动量, 脉冲量
    s32 zTrgPosBuf; //z轴移动量, 脉冲量

    u32 xTgSpd; //X轴速度
    u32 yTgSpd; //Y轴速度
    u32 zTgSpd; //Y轴速度

    float length;    //曲线长度
//		float xDis;      //直线x轴距离
//		float yDis;				//直线y轴距离
//		float zDis;				//直线z轴距离

    float currLen;  //已走线长度
    u32 pointNum;
    u8 quadrant;	//坐标象限
	
	u32 PPR[3];		//每转脉冲数 3个轴
    float MPR[3];	//每转毫米__导程
	
} AxisGroupDataDef;


typedef struct {
    s32 x;
    s32 y;
} mP2Def;

typedef struct
{
    s32 x;
    s32 y;
    s32 z;
    s32 w;
} PointDef;




typedef struct
{
    u32 Vinit;
    u32 Tacc;
    u32 Vtar;
} VelocityDef;

typedef struct
{
    u8 ax0;
    u8 ax1;
    u8 ax2;
    u8 ax3;
} MultiAxDef;


typedef struct
{
    float x;
    float y;
    float z;
    float rad;	  //弧度,圆弧插补时才用
} Point2Def;

typedef struct
{
    float x;
    float y;
    float z;
    float w;
    float rad;
} CircPointDef;

typedef struct
{
    CircPointDef p[3];   //圆弧3个点
    Point2Def rxy;		 //圆心
    float r;			 //半径
    float arc;			 //弧度
    float z;
    float w;
    float arc_acc;
    float arc_uni;
    float arc_dec;
    float arc_cur;
    float arc_nxt;
    float arc_step;
    u8 rot;				 //方向
    u8 allcircle;
} CircleDef;

typedef struct {
    float Rad;
    u32 Spd;
    u32 Puls;
} RadPulsSpdDef;

typedef struct {
    u32 Spd;
    u32 Puls;
} PulsSpdDef;


typedef struct
{
    u8 Group;
    u8 AxNum;
    u8 execute;
    u8 step;
    u8 done;

    u32 Vinit;
    u32 Vtar;
    u32 Tacc;
    u32 Acce;

    u32 Vresu;
    u32 PosResu;
    s32 PosStep;
    u32 PosAcc;		//加速行程
    u32 PosUnif;	//匀速行程
    u32 PosDec;		//减速行程
    s32 Pos_XY;		//两轴合行程
    s32 Pos_X;
    s32 Pos_Y;
    s32 Pos_Z;
    s32 Pos_W;

    PointDef P[3];

    u8  Axis[4];
    s32 InitPos[4];
    s32 MidPos[4];
    s32 TarPos[4];
    s32 RelPos[4];

    u32 RunSpd[4];
    s32 RunPos[4];

    //u32

} MultiAxParaDef;

enum MultiAxisStatus
{
    MultiAxIdle = 0,
    MultiAxAcce = 1,
    MultiAxUnif = 2,
    MultiAxDece = 3
};


extern u32 mresultant(s32 a, s32 b);
extern u32 mresultant_xyz(s32 a, s32 b, s32 c);
extern u32 GetSpd(u8 axNum);
extern u32 GetDecSteps(u8 axNum);
extern s32 Multi_AxGetStop(u8 AxNum);
extern s32 Multi_AxSetMulti(u8 AxNum);
extern s32 Multi_AxMoveRel(u8 AxNum, s32 pos);
extern AxParaDef AxPara[AXIS_NUM];

extern void MoveLineXYRun(AxisGroupDataDef* LIn);
extern s8 AxGroupGetSta(AxisGroupDataDef LIn);
extern s8 AxGroupMoveXYLine(u8 xAixs, u8 yAxis, u8 zAxis, PointfDef p1, PointfDef p2, float startSpeed, float acc, float speed, float dec, float endSpeed, AxisGroupDataDef* LIn);
extern void MoveInterXyRUN(AxisGroupDataDef* LIn);
extern s8 AxGroupMoveXYCircle(u8 xAixs, u8 yAxis, PointfDef p1, PointfDef p2, PointfDef p3, float startSpeed, float acc, float speed, float dec, float endSpeed, AxisGroupDataDef* LIn);
extern s8 AxGroupMoveXYCir(u8 xAixs, u8 yAxis, PointfDef p1, PointfDef p2, PointfDef p3, float startSpeed, float acc, float speed, float dec, float endSpeed, AxisGroupDataDef* LIn);
extern s8 AxGroupMoveXYStop(AxisGroupDataDef* LIn);
extern s8 AxGroupMoveXYContinue(AxisGroupDataDef* LIn);
#endif
