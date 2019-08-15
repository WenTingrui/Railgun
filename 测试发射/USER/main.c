#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "math.h"

#include "key.h"
#include "dianji_tim3_pwm.h"
#include "duoji_tim2_pwm.h"
#include "timer.h"
#include "getxy_uart.h"
#include "fire_command.h"
#include "charge_tim4.h"
#include "gpu_uart3.h"	 

#define et_len 20
#define X_GO_MAX 200
#define X_GO_MIN 180
#define X_RE_MAX 140
#define X_RE_MIN 120
#define MAX_TIME_COUNT 40000  //限制最大瞄准时长 约20s 

extern float umotor_x;
extern int etx;

char data_send[50];//待发送至串口屏的指令（char型变量）
int target_x=999;//小球中心的横坐标
int target_y=1;//小球中心的纵坐标
int sum_et_x=0;//横坐标误差的20次和（平均滤波用）
int sum_et_y=0;
int eet_x[et_len]={0};//横坐标误差20次数组
int eet_y[et_len]={0};
u32 time;//充电时间
u32 locktime=10000;//目标锁定时间，归零时锁定完成，舵机停止，开炮
u32 max_time_count;

void GpuSend(char * buf)
{
	u8 i=0;
	while (1)
	{
		if (buf[i]!=0)
		{
			USART_SendData(USART3, buf[i]); //发送一个byte到串口
			while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) != SET); //等待发送结束
			i++;
		}
		else return;
	}
}
u32 input_data_theta(int mode)
{
	u8 i=0, temp;
	u32 number=0;
	for(i=0;i<4;i++)//输入一个4位数d(mm)/θ(")
	{
		while(GetMatrixKeyValue()==0);//等待按下
		temp=GetMatrixKeyValue();
		if(temp==5||temp==6||temp==7)temp--;
		else if(temp==9||temp==10||temp==11)temp=temp-2;
		else if(temp==14)temp=0;
		number=number*10+temp;
//		printf("%d",temp);
		if(mode==0) //输入data
			if(i<3)
				sprintf((char*)data_send,"DS32(%d,218,'%d',1);\r\n",80+12*i,temp);//data转换为UsartGPU指令
			else
				sprintf((char*)data_send,"DS32(%d,218,'%d',1);\r\n",92+12*i,temp);//data转换为UsartGPU指令
		else //输入theta
			if(i<2)
				sprintf((char*)data_send,"DS32(%d,252,'%d',1);\r\n",92+12*i,temp);//theta转换为UsartGPU指令
			else
				sprintf((char*)data_send,"DS32(%d,252,'%d',1);\r\n",104+12*i,temp);//theta转换为UsartGPU指令
		GpuSend(data_send);
		delay_ms(100);
		while(GetMatrixKeyValue()!=0);//等待松开
	}
	return number;
}

int main(void)
 {	
	u8 mode=0;
	u8 temp,dir=1;
	 u16 i;
	u32 data=0,theta=0;
	char data_int[10];//待发送至串口屏的输入距离整数部分（char型变量）
	char data_fra[10];//待发送至串口屏的输入距离小数部分（char型变量）
	char theta_int[10];//待发送至串口屏的输入角度整数部分（char型变量）
	char theta_fra[10];//待发送至串口屏的输入角度小数部分（char型变量）
	 
	delay_init();	    	 //延时函数初始化	  
 	TIM3_PWM_Init(20000-1,72-1);//纵向舵机	 
	TIM2_PWM_Init(20000-1,72-1);//横向舵机	
	 uart_csb_init();
	 usart3_init();
	 fire_unit();
	MatrixKeyConfiguration();
	 
		Get_umotor_x (3000);//正中间
		Get_umotor_y (0);//仰角0
   	while(1)
	{
//		
		GPIO_SetBits(GPIOA,GPIO_Pin_5);	//开始充电
				time=1000;                                     //充电时间函数关系转换，time即毫秒数
				TIM4_Int_Init(1000-1,72-1);
				while(time!=0);//等待充电完成
				TIM_Cmd(TIM4, DISABLE);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);	//充电完毕	
		
				time=1000;                                     //充电时间函数关系转换，time即毫秒数
				TIM4_Int_Init(1000-1,72-1);
				while(time!=0);//等待充电完成
				TIM_Cmd(TIM4, DISABLE);
		
		GPIO_SetBits(GPIOA,GPIO_Pin_6);	//发射
				time=20;                                     //充电时间函数关系转换，time即毫秒数
				TIM4_Int_Init(1000-1,72-1);
				while(time!=0);//等待充电完成
				TIM_Cmd(TIM4, DISABLE);
				GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//停止发射
		
				time=5000;                                     //充电时间函数关系转换，time即毫秒数
				TIM4_Int_Init(1000-1,72-1);
				while(time!=0);//等待充电完成
				TIM_Cmd(TIM4, DISABLE);
		
	}	 
 }
