#include "pid.h"
/*
    Ϊ���ٸ�������ʹ�ã��ذ�ϵ��ȫ������100
*/
/*
    KpΪ����ϵ����KiΪ����ϵ����KdΪ΢��ϵ��,����ʽpid�㷨
*/
/*
��ѧ��ʽ�������

*/
    float Kp=0.5;
    float Ki=1;
    float Kd=0.01;//Ki>>Kd
    float lasterror=0;
    float preerror=0;
    float result=0;
    float setpofloat=0;
   /*
  pid����ͨ���������ͽ��������� 
  */
 /*ʹ������ʽpid*/
  void start_pid(float processValue)
{
    float thisError;
    float increment;
    float pError,dError,iError;
    thisError=setpofloat-processValue;
    pError=thisError-lasterror;
    dError=thisError-2*lasterror+preerror;
    iError=thisError;
    increment=Kp*pError+Ki*iError+Kd*dError;
    preerror=lasterror;
    lasterror=thisError;
    result+=increment;
    printf("resudlt:%f %f\n\r",result,setpofloat);
    TIM_SetCompare2(TIM3,-result);
}
/*pid��ʼ��*/
void pid_DeInit(float a,float b)
{
    lasterror=0;
    preerror=0;
    result=0;
    setpofloat=a;
    result=b;
}

    float lasterror1=0;
    float preerror1=0;
    float result1=0;
    float setpofloat1=0;
   /*
  pid����ͨ���������ͽ��������� 
  */
 /*ʹ������ʽpid*/
  void start_pid_1(float processValue)
{
    float thisError;
    float increment;
    float pError,dError,iError;
    thisError=setpofloat1-processValue;
    pError=thisError-lasterror1;
    dError=thisError-2*lasterror1+preerror1;
    iError=thisError;
    increment=Kp*pError+Ki*iError+Kd*dError;
    preerror1=lasterror1;
    lasterror1=thisError;
    result1+=increment;
    printf("resudlt:%f %f\n\r",result,setpofloat);
    TIM_SetCompare2(TIM5,-result);
}
void pid_DeInit_1(float a,float b)
{
    setpofloat1=a;
    result1=b;
}