#include "sys.h"
#include "gpu_uart3.h"	 
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

 //////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
    USART3->DR = (u8) ch;      
	return ch;
}
#endif 


 
//注意,读取USARTx->SR能避免莫名其妙的错误   	
//u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//u16 USART_TX_BUF[10]; 
//u8 approve=0;
//u8 i;
//u8 len=0;
//u16 temp=0;


  
void usart3_init(){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART2，GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART2，GPIOA时钟
  
   //USART3_TX   GPIOB10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.2
	
  //USART3_RX	  GPIOB11初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.3 

  //USART3 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = 115200;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART3, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART3, ENABLE);                    //使能串口1 

}

void USART3_IRQHandler(void)                	//串口1中断服务程序
	{
//		u8 Res;
//		if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
//			{
//				Res =USART_ReceiveData(USART3);	//读取接收到的数据
//				if(Res==0x5b)approve=1;//"["
//				if(approve==1&&Res!=0x5b)
//				{
//					if(Res!=0x5d)//"]"
//					{
//						USART_RX_BUF[len]=Res;
//						len++;
//					}
//					else
//					{
//						temp=0;
//						for(i=0;i<3;i++)temp=temp*10+(USART_RX_BUF[i]-0x30);
//						target_x =temp;
//						temp=0;
//						for(i=4;i<7;i++)temp=temp*10+(USART_RX_BUF[i]-0x30);
//						target_y =temp;					
//						approve=0;
//						temp=0;
//						len=0;			
//					}
//				}
//			} 

	} 

//void fasong(u32 data)
//{
//	u8 t,i=0;
//	if(data==0){USART_SendData(USART2,'0');//向串口1发送数据
//		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
//	}//等待发送结束
//	
//	while(data>0)
//	{
//		t=data%10;
//		data=data/10;
//		USART_TX_BUF[i]=t+0x30;
//		i++;
//	}
//	for(;i>0;i--)
//	{
//		USART_SendData(USART2,USART_TX_BUF[i-1]);//向串口1发送数据
//		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
//	}
//	
//	
//}
