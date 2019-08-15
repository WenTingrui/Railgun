#include "sys.h"
#include "gpu_uart3.h"	 
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

 //////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
    USART3->DR = (u8) ch;      
	return ch;
}
#endif 


 
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
//u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//u16 USART_TX_BUF[10]; 
//u8 approve=0;
//u8 i;
//u8 len=0;
//u16 temp=0;


  
void usart3_init(){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART2��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART2��GPIOAʱ��
  
   //USART3_TX   GPIOB10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.2
	
  //USART3_RX	  GPIOB11��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.3 

  //USART3 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = 115200;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART3, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���1 

}

void USART3_IRQHandler(void)                	//����1�жϷ������
	{
//		u8 Res;
//		if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
//			{
//				Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
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
//	if(data==0){USART_SendData(USART2,'0');//�򴮿�1��������
//		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
//	}//�ȴ����ͽ���
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
//		USART_SendData(USART2,USART_TX_BUF[i-1]);//�򴮿�1��������
//		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//	}
//	
//	
//}
