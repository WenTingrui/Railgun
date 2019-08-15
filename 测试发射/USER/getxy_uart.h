#ifndef __GETXY_UART_H
#define __GETXY_UART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			10  	//定义最大接收字节数 10
	  	

    		
void uart_csb_init(void);
void fasong(u32 data);
#endif


