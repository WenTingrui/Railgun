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
#define MAX_TIME_COUNT 40000  //���������׼ʱ�� Լ20s 

extern float umotor_x;
extern int etx;

char data_send[50];//����������������ָ�char�ͱ�����
int target_x=999;//С�����ĵĺ�����
int target_y=1;//С�����ĵ�������
int sum_et_x=0;//����������20�κͣ�ƽ���˲��ã�
int sum_et_y=0;
int eet_x[et_len]={0};//���������20������
int eet_y[et_len]={0};
u32 time;//���ʱ��
u32 locktime=10000;//Ŀ������ʱ�䣬����ʱ������ɣ����ֹͣ������
u32 max_time_count;

void GpuSend(char * buf)
{
	u8 i=0;
	while (1)
	{
		if (buf[i]!=0)
		{
			USART_SendData(USART3, buf[i]); //����һ��byte������
			while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) != SET); //�ȴ����ͽ���
			i++;
		}
		else return;
	}
}
u32 input_data_theta(int mode)
{
	u8 i=0, temp;
	u32 number=0;
	for(i=0;i<4;i++)//����һ��4λ��d(mm)/��(")
	{
		while(GetMatrixKeyValue()==0);//�ȴ�����
		temp=GetMatrixKeyValue();
		if(temp==5||temp==6||temp==7)temp--;
		else if(temp==9||temp==10||temp==11)temp=temp-2;
		else if(temp==14)temp=0;
		number=number*10+temp;
//		printf("%d",temp);
		if(mode==0) //����data
			if(i<3)
				sprintf((char*)data_send,"DS32(%d,218,'%d',1);\r\n",80+12*i,temp);//dataת��ΪUsartGPUָ��
			else
				sprintf((char*)data_send,"DS32(%d,218,'%d',1);\r\n",92+12*i,temp);//dataת��ΪUsartGPUָ��
		else //����theta
			if(i<2)
				sprintf((char*)data_send,"DS32(%d,252,'%d',1);\r\n",92+12*i,temp);//thetaת��ΪUsartGPUָ��
			else
				sprintf((char*)data_send,"DS32(%d,252,'%d',1);\r\n",104+12*i,temp);//thetaת��ΪUsartGPUָ��
		GpuSend(data_send);
		delay_ms(100);
		while(GetMatrixKeyValue()!=0);//�ȴ��ɿ�
	}
	return number;
}

int main(void)
 {	
	u8 mode=0;
	u8 temp,dir=1;
	 u16 i;
	u32 data=0,theta=0;
	char data_int[10];//������������������������������֣�char�ͱ�����
	char data_fra[10];//�����������������������С�����֣�char�ͱ�����
	char theta_int[10];//��������������������Ƕ��������֣�char�ͱ�����
	char theta_fra[10];//��������������������Ƕ�С�����֣�char�ͱ�����
	 
	delay_init();	    	 //��ʱ������ʼ��	  
 	TIM3_PWM_Init(20000-1,72-1);//������	 
	TIM2_PWM_Init(20000-1,72-1);//������	
	 uart_csb_init();
	 usart3_init();
	 fire_unit();
	MatrixKeyConfiguration();
	 
		Get_umotor_x (3000);//���м�
		Get_umotor_y (0);//����0
   	while(1)
	{
//		
		GPIO_SetBits(GPIOA,GPIO_Pin_5);	//��ʼ���
				time=1000;                                     //���ʱ�亯����ϵת����time��������
				TIM4_Int_Init(1000-1,72-1);
				while(time!=0);//�ȴ�������
				TIM_Cmd(TIM4, DISABLE);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);	//������	
		
				time=1000;                                     //���ʱ�亯����ϵת����time��������
				TIM4_Int_Init(1000-1,72-1);
				while(time!=0);//�ȴ�������
				TIM_Cmd(TIM4, DISABLE);
		
		GPIO_SetBits(GPIOA,GPIO_Pin_6);	//����
				time=20;                                     //���ʱ�亯����ϵת����time��������
				TIM4_Int_Init(1000-1,72-1);
				while(time!=0);//�ȴ�������
				TIM_Cmd(TIM4, DISABLE);
				GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//ֹͣ����
		
				time=5000;                                     //���ʱ�亯����ϵת����time��������
				TIM4_Int_Init(1000-1,72-1);
				while(time!=0);//�ȴ�������
				TIM_Cmd(TIM4, DISABLE);
		
	}	 
 }
