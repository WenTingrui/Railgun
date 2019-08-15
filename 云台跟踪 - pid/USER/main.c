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

#define et_len 20
#define X_GO_MAX 200
#define X_GO_MIN 180
#define X_RE_MAX 140
#define X_RE_MIN 120

extern float umotor_x;
extern int etx;

int target_x=999;//С�����ĵĺ�����
int target_y=1;//С�����ĵ�������
int sum_et_x=0;//����������20�κͣ�ƽ���˲��ã�
int sum_et_y=0;
int eet_x[et_len]={0};//���������20������
int eet_y[et_len]={0};
u32 time;//���ʱ��
u32 locktime=10000;//Ŀ������ʱ�䣬����ʱ������ɣ����ֹͣ������

u32 input_data_theta()
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
		printf("%d",temp);
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
	 
	delay_init();	    	 //��ʱ������ʼ��	  
 	TIM3_PWM_Init(20000-1,72-1);//������	 
	TIM2_PWM_Init(20000-1,72-1);//������	
	 uart_csb_init();
	 fire_unit();
	MatrixKeyConfiguration();
	 
		Get_umotor_x (3000);//���м�
		Get_umotor_y (0);//����0
	 
	 TIM1_Int_Init(100-1,72-1);//0.1ms����Ƶ��
   	while(1)
	{
		printf("%d,%d\r\n",etx,(u32)umotor_x);
		delay_ms (100);
		

				
//				while(locktime !=0);
//				TIM_Cmd(TIM1, DISABLE);
				
			
		
	}	 
 }
