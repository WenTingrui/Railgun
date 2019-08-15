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
extern int target_x;//׷�ٵ���С�����ĺ�����
extern int target_y;
extern int sum_et_x;
extern int sum_et_y;
extern int eet_x[];
extern int eet_y[];
int et_len=20;
u8 ii;
int etx=0,etx_1=0,etx_2=0;//���������
int ety=0,ety_1=0,ety_2=0;
float umotor_x;//����������Сȡֵ0~6000
float umotor_y=0;//����������Сȡֵ0~6000

//��ʱ��1�жϷ������

void TIM1_UP_IRQHandler(void)   //TIM1�ж�
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			if(max_time_count>0)
			{
				max_time_count --;
				if(target_x !=999)
					{
						//��һʱ��������ʱ�̵�ƫ��etֵ
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
						
						//����Ŀ��1s
						if(etx<3&&etx>-3)
						{
							if(locktime>0)locktime--;
						}
				//		else locktime=10000;
						
						//����ʽPID����
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

//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM1_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM1��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM1�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIMx					 
}












