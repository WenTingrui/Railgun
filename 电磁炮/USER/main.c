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
#define X_GO_MAX 160
#define X_GO_MIN 80
#define X_RE_MAX 80
#define X_RE_MIN 0
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
//		printf("-----------------\r\n");//清除屏幕命令
//		printf("选择模式：\r\n");
		printf("CLS(0);PIC(0,0,1);\r\n");
		delay_ms(1000);
		while(GetMatrixKeyValue()==0);//等待按下
		mode=GetMatrixKeyValue();
		while(GetMatrixKeyValue()!=0);//等待松开
		switch(mode)
		{
		//模式1，基本2，输入data即可
		case 1:
//			printf("模式1/基本2\r\n\r\n输入发射距离(mm)按D确认：");
		GpuSend("CLS(0);PIC(0,0,2);SCC(36,65535);SBC(36);DS32(116,218,'.',1);\r\n");
			delay_ms(100);
			data=input_data_theta(0);//接收距离数据
		sprintf((char*)data_int,"%d",data/10);//data整数部分转换为char型
			sprintf((char*)data_fra,"%d",data%10);//data小数部分转换为char型
			while(GetMatrixKeyValue()==0);//等待按下
			temp=GetMatrixKeyValue();
			while(GetMatrixKeyValue()!=0);//等待松开
			if(temp==16)//按键D,启动电磁炮
			{
//				printf("\r\n正在充电...\r\n"); 
				GpuSend("DS32(200,218,'√',1);");
				delay_ms(100);
				//printf("\r\n正在充电...\r\n");
				GpuSend("AGIF(0,40,80,7,14,10,1);\r\n");
				delay_ms(100);
				//执行函数
				Get_umotor_x (3000);
				Get_umotor_y (2000);
				GPIO_SetBits(GPIOA,GPIO_Pin_5);	//开始充电
				time=(u32)(0.0004013*(1.0*data)*(1.0*data)-0.3083*(1.0*data)+118.7);                                     //充电时间函数关系转换，time即毫秒数
				TIM4_Int_Init(1000-1,72-1);
				while(time!=0);//等待充电完成
				GPIO_ResetBits(GPIOA,GPIO_Pin_5);	//充电完成
				TIM_Cmd(TIM4, DISABLE); 
//				printf("充电完毕，发射！\r\n"); 
				GpuSend("CLS(0);PIC(0,0,2);SCC(36,65535);SBC(36);");
				GpuSend("DS32(80,218,'");GpuSend(data_int);GpuSend("',1);");
				GpuSend("DS32(116,218,'.',1);");
				GpuSend("DS32(128,218,'");GpuSend(data_fra);GpuSend("',1);");
				GpuSend("DS32(200,218,'√',1);DS48(40,80,'Finish!',1);\r\n");
				delay_ms(100);
				GPIO_SetBits(GPIOA,GPIO_Pin_6);	//发射
				delay_ms (40);                                //线圈通电时间
				GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//停止发射
				//复位
				delay_ms (500);
				GPIO_SetBits(GPIOA,GPIO_Pin_6);	//发射
				delay_ms (500);                                //线圈通电时间
				GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//停止发射
				Get_umotor_x (3000);
				Get_umotor_y (0);
			}
			break;
		
		//模式2，基本3，输入data和theta
		case 2:
//			printf("模式2/基本3\r\n\r\n输入发射距离(mm)按D确认：");
			GpuSend("CLS(0);PIC(0,0,3);SCC(36,65535);SBC(36);DS32(116,218,'.',1);DS32(116,252,'.',1);\r\n");
			delay_ms (100);
			data=input_data_theta(0);//接收距离数据
			sprintf((char*)data_int,"%d",data/10);//data整数部分转换为char型
			sprintf((char*)data_fra,"%d",data%10);//data小数部分转换为char型
			while(GetMatrixKeyValue()==0);//等待按下
			temp=GetMatrixKeyValue();
			while(GetMatrixKeyValue()!=0);//等待松开
			if(temp==16)//按键D
			{
				GpuSend("DS32(200,218,'√',1);\r\n");
				delay_ms(100);
				//printf("\r\n输入发射偏角(°/100)按D确认："); 
				theta=input_data_theta(1);//接收角度数据
				sprintf((char*)theta_int,"%d",theta/100);//theta整数部分转换为char型
				sprintf((char*)theta_fra,"%d",theta%100);//theta小数部分转换为char型
				
				while(GetMatrixKeyValue()==0);//等待按下
				temp=GetMatrixKeyValue();
				while(GetMatrixKeyValue()!=0);//等待松开
				if(temp==16)//按键D,启动电磁炮
				{
					GpuSend("DS32(200,252,'√',1);\r\n");
					delay_ms(100);
//					printf("\r\n瞄准中...\r\n"); 
					//执行函数
					Get_umotor_x (theta);
					Get_umotor_y (2000);
					//printf("正在充电...\r\n"); 
					GpuSend("AGIF(0,40,80,7,14,10,1);\r\n");
					delay_ms(100);
					GPIO_SetBits(GPIOA,GPIO_Pin_5);	//开始充电
					time=(u32)(0.0004013*(1.0*data)*(1.0*data)-0.3083*(1.0*data)+118.7); 
					TIM4_Int_Init(1000-1,72-1);
					while(time!=0);//等待充电完成
					TIM_Cmd(TIM4, DISABLE); 
					GPIO_ResetBits(GPIOA,GPIO_Pin_5);	//充电完成
					//printf("充电完毕，发射！\r\n");
					GpuSend("CLS(0);PIC(0,0,3);SCC(36,65535);SBC(36);");
					GpuSend("DS32(80,218,'");GpuSend(data_int);GpuSend("',1);");
					GpuSend("DS32(116,218,'.',1);");
					GpuSend("DS32(128,218,'");GpuSend(data_fra);GpuSend("',1);");
					GpuSend("DS32(92,252,'");GpuSend(theta_int);GpuSend("',1);");
					GpuSend("DS32(116,252,'.',1);");
					GpuSend("DS32(128,252,'");GpuSend(theta_fra);GpuSend("',1);");
					GpuSend("DS32(200,218,'√',1);DS32(200,252,'√',1);DS48(40,80,'Finish!',1);\r\n");
					delay_ms(100);
					GPIO_SetBits(GPIOA,GPIO_Pin_6);	//发射
					delay_ms (40);
					GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//停止发射
					//复位
				delay_ms (500);
				GPIO_SetBits(GPIOA,GPIO_Pin_6);	//发射
				delay_ms (500);                                //线圈通电时间
				GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//停止发射
				Get_umotor_x (3000);
				Get_umotor_y (0);
				}
			}
			break;
			
		//模式3，发挥1，一进入就自动跟踪
		case 3:
//				printf("模式3/发挥1\r\n\r\n按D一键启动\r\n");
			GpuSend("CLS(0);PIC(0,0,4);SCC(36,65535);SBC(36);DS32(116,218,'.',1);DS32(116,252,'.',1);\r\n");
			delay_ms(100);
			while(GetMatrixKeyValue()==0);//等待按下
			temp=GetMatrixKeyValue();
			while(GetMatrixKeyValue()!=0);//等待松开
			if(temp==16)//按键D,启动电磁炮
			{
//				printf("正在搜寻目标...\r\n");
				GpuSend("BOXF(40,90,136,140,15);DS24(40,90,'搜寻目标',1);DS32(40,110,'......',1);\r\n");
				delay_ms(100);
				Get_umotor_x (0);//最左边
				Get_umotor_y (0);//仰角20
				delay_ms (500);
				//恒速往复扫描
				i=0; dir=1;
				locktime=10000;
				while(target_x ==999)
				{
					if(dir==1)
					{
						i=i+20;
						Get_umotor_x (i);
						if(i==6000)dir=0;
						delay_ms (40);
					}
					else
					{
						i=i-20;
						Get_umotor_x (i);
						if(i==0)dir=1;
						delay_ms (40);
					}
				}			
				//搜寻到目标，启动PID
				umotor_x =i;
				printf("BOXF(40,90,136,140,15);DS24(40,90,'正在瞄准',1);DS32(40,110,'......',1);\r\n");
//				GpuSend("BOXF(40,90,136,140,15);DS24(40,90,'正在瞄准',1);DS32(40,110,'......',1);\r\n");
//				delay_ms(100);
				max_time_count =MAX_TIME_COUNT ;
				TIM1_Int_Init(100-1,72-1);//1ms控制频率
				while(locktime !=0)
				{printf("%d,%d,%d\r\n",etx,max_time_count,target_y );delay_ms (100);}
				;
				TIM_Cmd(TIM1, DISABLE);
				//锁定完成，发射
				Get_umotor_y (2000);//仰角20
					//printf("正在充电...\r\n");
					GpuSend("BOXF(40,90,136,140,15);AGIF(0,40,80,7,14,10,1);\r\n");
					delay_ms(100);
					GPIO_SetBits(GPIOA,GPIO_Pin_5);	//开始充电
				
				time=0;
				for(data=0;data<20;data++)
				{
					time+=target_y ;
					delay_ms (10);
				}
				time=time/20;
				time=(u32)(6.478*(1.0*time)*(1.0*time)-387.2*(1.0*time)+7351); 
					time=((u32)(0.0004013*(1.0*time)*(1.0*time)-0.3083*(1.0*time)+118.7)-200); 
					TIM4_Int_Init(1000-1,72-1);
					while(time!=0);//等待充电完成
					TIM_Cmd(TIM4, DISABLE); 
				GPIO_ResetBits(GPIOA,GPIO_Pin_5);	//充电完成
					//printf("充电完毕，发射！\r\n");
					GpuSend("CLS(0);PIC(0,0,4);SCC(36,65535);SBC(36);");
					GpuSend("DS32(80,218,'");GpuSend(data_int);GpuSend("',1);");
					GpuSend("DS32(116,218,'.',1);");
					GpuSend("DS32(128,218,'");GpuSend(data_fra);GpuSend("',1);");
					GpuSend("DS32(92,252,'");GpuSend(theta_int);GpuSend("',1);");
					GpuSend("DS32(116,252,'.',1);");
					GpuSend("DS32(128,252,'");GpuSend(theta_fra);GpuSend("',1);");
					GpuSend("DS32(200,218,'√',1);DS32(200,252,'√',1);DS48(40,80,'Finish!',1);\r\n");
					delay_ms(100);
					GPIO_SetBits(GPIOA,GPIO_Pin_6);	//发射
					delay_ms (40);
					GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//停止发射		
					//复位
				delay_ms (500);
				GPIO_SetBits(GPIOA,GPIO_Pin_6);	//发射
				delay_ms (500);                                //线圈通电时间
				GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//停止发射
				Get_umotor_x (3000);
				Get_umotor_y (0);
			}
			break;
		
		//模式4，发挥2，d=250，往复±30°（打完回-30°）
		case 5:
			//printf("模式4/发挥2\r\n\r\n按D一键启动\r\n");
			GpuSend("CLS(0);PIC(0,0,5);SCC(36,65535);SBC(36);DS32(116,218,'.',1);DS32(116,252,'.',1);");
			GpuSend("DS32(80,218,'2',1);DS32(92,218,'5',1);DS32(104,218,'0',1);DS32(128,218,'0',1);\r\n");
			delay_ms(100);
			GPIO_SetBits(GPIOA,GPIO_Pin_5);	//开始充电
			time=1856;                                     //时间待测！
			TIM4_Int_Init(1000-1,72-1);
			while(GetMatrixKeyValue()==0);//等待按下
			temp=GetMatrixKeyValue();
			while(GetMatrixKeyValue()!=0);//等待松开
			if(temp==16)//按键D,启动电磁炮
			{
				//printf("正在搜寻目标...\r\n");
				GpuSend("BOXF(40,90,136,140,15);DS24(40,90,'搜寻目标',1);DS32(40,110,'......',1);\r\n");
				delay_ms(100);
				Get_umotor_x (0);//最左边
				Get_umotor_y (0);//仰角20
				delay_ms (500);
				//恒速往复扫描
				i=6; dir=1;
				while(1)
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
						//printf("发射！\r\n");
						GpuSend("CLS(0);PIC(0,0,4);SCC(36,65535);SBC(36);");
						GpuSend("DS32(92,252,'");GpuSend(theta_int);GpuSend("',1);");
						GpuSend("DS32(116,252,'.',1);");
						GpuSend("DS32(128,252,'");GpuSend(theta_fra);GpuSend("',1);");
						GpuSend("DS32(200,252,'√',1);DS48(40,80,'Finish!',1);\r\n");
						delay_ms(100);
						Get_umotor_y (2000);//仰角20
						delay_ms (500);
						GPIO_SetBits(GPIOA,GPIO_Pin_6);	//发射
						delay_ms (20);
						GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//停止发射	
						delay_ms (500);
						Get_umotor_y (0);//仰角20
						TIM_Cmd(TIM4, DISABLE); 
						time=1;
						break;
					}
				}
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
				}
			}
			//复位
				GPIO_SetBits(GPIOA,GPIO_Pin_6);	//发射
				delay_ms (500);                                //线圈通电时间
				GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//停止发射
				Get_umotor_x (3000);
				Get_umotor_y (0);
			break;
		
		//模式5，其他
		case 6:
	//				printf("模式3/发挥1\r\n\r\n按D一键启动\r\n");
//			GpuSend("CLS(0);PIC(0,0,4);SCC(36,65535);SBC(36);DS32(116,218,'.',1);DS32(116,252,'.',1);\r\n");
			delay_ms(100);
			while(GetMatrixKeyValue()==0);//等待按下
			temp=GetMatrixKeyValue();
			while(GetMatrixKeyValue()!=0);//等待松开
			if(temp==16)//按键D,启动电磁炮
			{
//				printf("正在搜寻目标...\r\n");
//				GpuSend("BOXF(40,90,136,140,15);DS24(40,90,'搜寻目标',1);DS32(40,110,'......',1);\r\n");
				delay_ms(100);
				Get_umotor_x (0);//最左边
				Get_umotor_y (0);//仰角20
				delay_ms (500);
				//恒速往复扫描
				i=0; dir=1;
				locktime=10000;
				while(target_x ==999)
				{
					if(dir==1)
					{
						i=i+20;
						Get_umotor_x (i);
						if(i==6000)dir=0;
						delay_ms (40);
					}
					else
					{
						i=i-20;
						Get_umotor_x (i);
						if(i==0)dir=1;
						delay_ms (40);
					}
				}			
				//搜寻到目标，启动PID
				umotor_x =i;
				printf("BOXF(40,90,136,140,15);DS24(40,90,'正在瞄准',1);DS32(40,110,'......',1);\r\n");
//				GpuSend("BOXF(40,90,136,140,15);DS24(40,90,'正在瞄准',1);DS32(40,110,'......',1);\r\n");
//				delay_ms(100);
				max_time_count =MAX_TIME_COUNT ;
				TIM1_Int_Init(100-1,72-1);//1ms控制频率
				while(locktime !=0)
				{printf("%d,%d,%d\r\n",etx,max_time_count,target_y );delay_ms (100);}
				;
				TIM_Cmd(TIM1, DISABLE);
				//锁定完成，发射
				Get_umotor_y (2000);//仰角20
					//printf("正在充电...\r\n");
//					GpuSend("BOXF(40,90,136,140,15);AGIF(0,40,80,7,14,10,1);\r\n");
					delay_ms(100);
					GPIO_SetBits(GPIOA,GPIO_Pin_5);	//开始充电
				
				time=0;
				for(data=0;data<20;data++)
				{
					time+=target_y ;
					delay_ms (10);
				}
				time=time/20;
				time=(u32)(6.478*(1.0*time)*(1.0*time)-387.2*(1.0*time)+7351); 
					time=((u32)(0.0004013*(1.0*time)*(1.0*time)-0.3083*(1.0*time)+118.7)-200); 
					TIM4_Int_Init(1000-1,72-1);
					while(time!=0);//等待充电完成
					TIM_Cmd(TIM4, DISABLE); 
				GPIO_ResetBits(GPIOA,GPIO_Pin_5);	//充电完成
					//printf("充电完毕，发射！\r\n");
//					GpuSend("CLS(0);PIC(0,0,4);SCC(36,65535);SBC(36);");
////					GpuSend("DS32(80,218,'");GpuSend(data_int);GpuSend("',1);");
//					GpuSend("DS32(116,218,'.',1);");
//					GpuSend("DS32(128,218,'");GpuSend(data_fra);GpuSend("',1);");
//					GpuSend("DS32(92,252,'");GpuSend(theta_int);GpuSend("',1);");
//					GpuSend("DS32(116,252,'.',1);");
//					GpuSend("DS32(128,252,'");GpuSend(theta_fra);GpuSend("',1);");
//					GpuSend("DS32(200,218,'√',1);DS32(200,252,'√',1);DS48(40,80,'Finish!',1);\r\n");
					delay_ms(100);
					GPIO_SetBits(GPIOA,GPIO_Pin_6);	//发射
					delay_ms (40);
					GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//停止发射		
					//复位
				delay_ms (500);
				GPIO_SetBits(GPIOA,GPIO_Pin_6);	//发射
				delay_ms (500);                                //线圈通电时间
				GPIO_ResetBits(GPIOA,GPIO_Pin_6);	//停止发射
				Get_umotor_x (3000);
				Get_umotor_y (0);
			}
			break;

		default:;
			
		}
	}	 
 }
