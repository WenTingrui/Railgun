 #include "bujin.h"
#include "sys.h"
#include "delay.h"
void Bujin_clk(u16 n)
{
	u32 i;u16 times=10;
	for(i=0;i<n;i++)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_5);	GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9);	
		delay_ms (times);
		GPIO_SetBits(GPIOB,GPIO_Pin_5|GPIO_Pin_7);	GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_9);	
		delay_ms (times);
		GPIO_SetBits(GPIOB,GPIO_Pin_7);	GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_5|GPIO_Pin_9);	
		delay_ms (times);
		GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);	GPIO_ResetBits(GPIOB,GPIO_Pin_5|GPIO_Pin_9);	
		delay_ms (times);
		GPIO_SetBits(GPIOB,GPIO_Pin_6);	GPIO_ResetBits(GPIOB,GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_9);	
		delay_ms (times);
		GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_9);	GPIO_ResetBits(GPIOB,GPIO_Pin_7|GPIO_Pin_5);	
		delay_ms (times);
		GPIO_SetBits(GPIOB,GPIO_Pin_9);	GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_5);	
		delay_ms (times);
		GPIO_SetBits(GPIOB,GPIO_Pin_5|GPIO_Pin_9);	GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);	
		delay_ms (times);
	}
	
	
}
//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void Bujin_unit()
{  
	 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9);						 //PB.5 �����
	

}


