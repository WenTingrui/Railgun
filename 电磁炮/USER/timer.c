#include "timer.h"
#include "dianji_tim3_pwm.h"
#include "duoji_tim2_pwm.h"

#define TARGET_X 80
#define TARGET_Y 0

float KP=100;
float KI=15;
float KD=0;

extern u32 locktime;
extern u32 max_time_count;
extern int target_x;//追踪到的小球中心横坐标
extern int target_y;
extern int sum_et_x;
extern int sum_et_y;
extern int eet_x[];
extern int eet_y[];
int et_len=20;
u8 ii;
int etx=0,etx_1=0,etx_2=0;//横坐标误差
int ety=0,ety_1=0,ety_2=0;
float umotor_x;//舵机输出量大小取值0~6000
float umotor_y=0;//舵机输出量大小取值0~6000

//定时器1中断服务程序

void TIM1_UP_IRQHandler(void)   //TIM1中断
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //清除TIMx更新中断标志 
			if(max_time_count>0)
			{
				max_time_count --;
				if(target_x !=999)
					{
						//上一时刻与上上时刻的偏差et值
						etx_2=etx_1;  //ety_2 =ety_1;
						etx_1=etx;   //ety_1 =ety;
						
						sum_et_x=sum_et_x-eet_x[0]+target_x-TARGET_X;
						etx=sum_et_x/et_len;
				//		sum_et_y=sum_et_y-eet_y[0]+target_y-TARGET_Y;
				//		ety=sum_et_y/et_len;
						for(ii=0;ii<(et_len-1);ii++)
						{
							eet_x[ii]=eet_x[ii+1];
				//			eet_y[ii]=eet_y[ii+1];
						}
						eet_x[et_len-1]=target_x-TARGET_X;
				//		eet_y[et_len-1]=target_y-TARGET_Y;
						
						//锁定目标1s
						if(etx<3&&etx>-3)
						{
							if(locktime>0)locktime--;
						}
				//		else locktime=10000;
						
						//增量式PID控制
						umotor_x=umotor_x+((KP+KI+KD)*etx-(KP+2*KD)*etx_1+KD*etx_2)/10000.0;
				//		umotor_y=umotor_y+((KP+KI+KD)*ety-(KP+2*KD)*ety_1+KD*ety_2)/1000.0;
						
						if(umotor_x>6000.0)umotor_x=6000;
						else if(umotor_x<0)umotor_x=0;
						Get_umotor_x((u16)umotor_x);
						
				//		if(umotor_y>6000.0)umotor_y=6000;
				//		else if(umotor_y<0)umotor_y=0;
				//		Get_umotor_y((u16)umotor_y);
					}
				}
			else locktime=0;
		}
}

//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM1_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
	
	//定时器TIM1初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //使能指定的TIM1中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM1, ENABLE);  //使能TIMx					 
}












