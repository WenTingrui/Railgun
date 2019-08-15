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

char data_send[50];//����������������ָ�char�ͱ�����
int target_x=999;//С�����ĵĺ�����
int target_y=1;//С�����ĵ�������
int sum_et_x=0;//����������20�κͣ�ƽ���˲��ã�
int sum_et_y=0;
int eet_x[et_len]={0};//���������20������
int eet_y[et_len]={0};
u32 time;//���ʱ��
u32 locktime=10000;//Ŀ������ʱ�䣬����ʱ������ɣ����ֹͣ������

void GpuSend(char * buf)
{
	u8 i=0;
	while (1)
	{
		if (buf[i]!=0)
		{
			USART_SendData(USART2, buf[i]); //����һ��byte������
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET); //�ȴ����ͽ���
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
	 fire_unit();
	MatrixKeyConfiguration();
	 
		Get_umotor_x (3000);//���м�
		Get_umotor_y (0);//����0
   	while(1)
	{
//		printf("-----------------\r\n");//�����Ļ����
//		printf("ѡ��ģʽ��\r\n");
		printf("CLS(0);PIC(0,0,1);\r\n");
		delay_ms(1000);
		while(GetMatrixKeyValue()==0);//�ȴ�����
		mode=GetMatrixKeyValue();
		while(GetMatrixKeyValue()!=0);//�ȴ��ɿ�
		switch(mode)
		{
		//ģʽ1������2������data����
		case 1:
//			printf("ģʽ1/����2\r\n\r\n���뷢�����(mm)��Dȷ�ϣ�");
		GpuSend("CLS(0);PIC(0,0,2);SCC(36,65535);SBC(36);DS32(116,218,'.',1);\r\n");
			delay_ms(100);
			data=input_data_theta(0);//���վ�������
		sprintf((char*)data_int,"%d",data/10);//data��������ת��Ϊchar��
			sprintf((char*)data_fra,"%d",data%10);//dataС������ת��Ϊchar��
			while(GetMatrixKeyValue()==0);//�ȴ�����
			temp=GetMatrixKeyValue();
			while(GetMatrixKeyValue()!=0);//�ȴ��ɿ�
			if(temp==16)//����D,���������
			{
//				printf("\r\n���ڳ��...\r\n"); 
				GpuSend("DS32(200,218,'��',1);");
				delay_ms(100);
				//printf("\r\n���ڳ��...\r\n");
				GpuSend("AGIF(0,40,80,7,14,10,1);\r\n");
				delay_ms(100);
				//ִ�к���
				Get_umotor_x (3000);
				Get_umotor_y (2000);
				GPIO_SetBits(GPIOA,GPIO_Pin_5);	//��ʼ���
				time=data;                                     //���ʱ�亯����ϵת����time��������
				TIM4_Int_Init(1000-1,72-1);
				while(time!=0);//�ȴ�������
				TIM_Cmd(TIM4, DISABLE); 
//				printf("�����ϣ����䣡\r\n"); 
				GpuSend("CLS(0);PIC(0,0,2);SCC(36,65535);SBC(36);");
				GpuSend("DS32(80,218,'");GpuSend(data_int);GpuSend("',1);");
				GpuSend("DS32(116,218,'.',1);");
				GpuSend("DS32(128,218,'");GpuSend(data_fra);GpuSend("',1);");
				GpuSend("DS32(200,218,'��',1);DS48(40,80,'Finish!',1);\r\n");
				delay_ms(100);
				GPIO_SetBits(GPIOA,GPIO_Pin_6);	//����
				delay_ms (100);                                //��Ȧͨ��ʱ��
				GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//ֹͣ����
				//��λ
				delay_ms (1000);
				Get_umotor_x (3000);
				Get_umotor_y (0);
			}
			break;
		
		//ģʽ2������3������data��theta
		case 2:
//			printf("ģʽ2/����3\r\n\r\n���뷢�����(mm)��Dȷ�ϣ�");
			GpuSend("CLS(0);PIC(0,0,3);SCC(36,65535);SBC(36);DS32(116,218,'.',1);DS32(116,252,'.',1);\r\n");
			delay_ms (100);
			data=input_data_theta(0);//���վ�������
			sprintf((char*)data_int,"%d",data/10);//data��������ת��Ϊchar��
			sprintf((char*)data_fra,"%d",data%10);//dataС������ת��Ϊchar��
			while(GetMatrixKeyValue()==0);//�ȴ�����
			temp=GetMatrixKeyValue();
			while(GetMatrixKeyValue()!=0);//�ȴ��ɿ�
			if(temp==16)//����D
			{
				GpuSend("DS32(200,218,'��',1);\r\n");
				delay_ms(100);
				//printf("\r\n���뷢��ƫ��(��/100)��Dȷ�ϣ�"); 
				theta=input_data_theta(1);//���սǶ�����
				sprintf((char*)theta_int,"%d",theta/100);//theta��������ת��Ϊchar��
				sprintf((char*)theta_fra,"%d",theta%100);//thetaС������ת��Ϊchar��
				
				while(GetMatrixKeyValue()==0);//�ȴ�����
				temp=GetMatrixKeyValue();
				while(GetMatrixKeyValue()!=0);//�ȴ��ɿ�
				if(temp==16)//����D,���������
				{
					GpuSend("DS32(200,252,'��',1);\r\n");
					delay_ms(100);
//					printf("\r\n��׼��...\r\n"); 
					//ִ�к���
					Get_umotor_x (theta);
					Get_umotor_y (2000);
					//printf("���ڳ��...\r\n"); 
					GpuSend("AGIF(0,40,80,7,14,10,1);\r\n");
					delay_ms(100);
					GPIO_SetBits(GPIOA,GPIO_Pin_5);	//��ʼ���
					time=data;                                     //ʱ�亯����ϵת����time��������
					TIM4_Int_Init(1000-1,72-1);
					while(time!=0);//�ȴ�������
					TIM_Cmd(TIM4, DISABLE); 
					//printf("�����ϣ����䣡\r\n");
					GpuSend("CLS(0);PIC(0,0,3);SCC(36,65535);SBC(36);");
					GpuSend("DS32(80,218,'");GpuSend(data_int);GpuSend("',1);");
					GpuSend("DS32(116,218,'.',1);");
					GpuSend("DS32(128,218,'");GpuSend(data_fra);GpuSend("',1);");
					GpuSend("DS32(92,252,'");GpuSend(theta_int);GpuSend("',1);");
					GpuSend("DS32(116,252,'.',1);");
					GpuSend("DS32(128,252,'");GpuSend(theta_fra);GpuSend("',1);");
					GpuSend("DS32(200,218,'��',1);DS32(200,252,'��',1);DS48(40,80,'Finish!',1);\r\n");
					delay_ms(100);
					GPIO_SetBits(GPIOA,GPIO_Pin_6);	//����
					delay_ms (100);
					GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//ֹͣ����
					//��λ
					delay_ms (1000);
					Get_umotor_x (3000);
					Get_umotor_y (0);
				}
			}
			break;
			
		//ģʽ3������1��һ������Զ�����
		case 3:
			printf("ģʽ3/����1\r\n\r\n��Dһ������\r\n");
			while(GetMatrixKeyValue()==0);//�ȴ�����
			temp=GetMatrixKeyValue();
			while(GetMatrixKeyValue()!=0);//�ȴ��ɿ�
			if(temp==16)//����D,���������
			{
				printf("������ѰĿ��...\r\n");
				Get_umotor_x (0);//�����
				Get_umotor_y (0);//����20
				delay_ms (100);
				//��������ɨ��
				i=0; dir=1;
				locktime=10000;
				while(target_x ==999)
				{
					if(dir==1)
					{
						i=i+60;
						Get_umotor_x (i);
						if(i==6000)dir=0;
						delay_ms (20);
					}
					else
					{
						i=i-60;
						Get_umotor_x (i);
						if(i==0)dir=1;
						delay_ms (20);
					}
				}			
				//��Ѱ��Ŀ�꣬����PID
				umotor_x =i+500;
				printf("������׼...\r\n");
				TIM1_Int_Init(100-1,72-1);//1ms����Ƶ��
				while(locktime !=0)
				{printf("%d,%d\r\n",etx,locktime );delay_ms (100);}
				;
				TIM_Cmd(TIM1, DISABLE);
				//������ɣ�����
					printf("���ڳ��...\r\n");
					GPIO_SetBits(GPIOA,GPIO_Pin_5);	//��ʼ���
					time=0;                                     //ʱ�亯����ϵת����time��������
					TIM4_Int_Init(100-1,72-1);
					while(time!=0);//�ȴ�������
					TIM_Cmd(TIM4, DISABLE); 
					printf("�����ϣ����䣡\r\n"); 
					GPIO_SetBits(GPIOA,GPIO_Pin_6);	//����
					delay_ms (100);
					GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//ֹͣ����		
					//��λ
					delay_ms (1000);
					Get_umotor_x (3000);
					Get_umotor_y (0);
			}
			break;
		
		//ģʽ4������2��d=250��������30�㣨�����-30�㣩
		case 5:
			printf("ģʽ4/����2\r\n\r\n��Dһ������\r\n");
			GPIO_SetBits(GPIOA,GPIO_Pin_5);	//��ʼ���
			time=666;                                     //ʱ����⣡
			TIM4_Int_Init(1000-1,72-1);
			while(GetMatrixKeyValue()==0);//�ȴ�����
			temp=GetMatrixKeyValue();
			while(GetMatrixKeyValue()!=0);//�ȴ��ɿ�
			if(temp==16)//����D,���������
			{
				printf("������ѰĿ��...\r\n");
				Get_umotor_x (0);//�����
				Get_umotor_y (0);//����20
				delay_ms (100);
				//��������ɨ��
				i=6; dir=1;
				while(i!=0)
				{
					if(dir==1)
					{
						i=i+6;
						Get_umotor_x (i);
						if(i==6000)dir=0;
						delay_ms (4);
					}
					else
					{
						i=i-6;
						Get_umotor_x (i);
						delay_ms (4);
					}
					if((target_x>X_GO_MIN && target_x<X_GO_MAX && time==0&&dir==1)||(target_x>X_RE_MIN && target_x<X_RE_MAX && time==0&&dir==0))
					{
						printf("���䣡\r\n");
						GPIO_SetBits(GPIOA,GPIO_Pin_6);	//����
						delay_ms (100);
						GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//ֹͣ����	
						TIM_Cmd(TIM4, DISABLE); 
						time=1;
					}
				}
					
			}
			break;
		
		//ģʽ5������
		case 6:
	
			break;

		default:;
			
		}
	}	 
 }
