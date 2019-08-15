 #include "key.h"
#include "sys.h"
#include "delay.h"
//R1,R2,R3,R4,C1,C2,C3,C4
//C0,C2,A0,A2,A4,A6,C4,B0
void MatrixModeConfig(MatirxMode mode);

void MatrixKeyConfiguration(void)
{
	GPIO_InitTypeDef    GPIO;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
						   RCC_APB2Periph_GPIOC, ENABLE);
    
    //Register IO 
    GPIO.GPIO_Pin   =  COL1 | COL2 | COL3 | COL4;
    GPIO.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO);
	
	GPIO.GPIO_Pin   = ROW1 |ROW2 |ROW3 |ROW4 ;
	GPIO_Init(GPIOB, &GPIO);
//	
//	GPIO.GPIO_Pin   = ROW3 | COL3 | COL4;
//	GPIO_Init(GPIOC, &GPIO);
//	
	ROW_1 = 1;
	ROW_2 = 1;
	ROW_3 = 1;
	ROW_4 = 1;
	COL_1 = 1;
	COL_2 = 1;
	COL_3 = 1;
	COL_4 = 1;
	//MatrixModeConfig(Column);
}

//R1,R2,R3,R4,C1,C2,C3,C4
//C0,C2,A0,A2,A4,A6,C4,B0
void MatrixModeConfig(MatirxMode mode)
{
	GPIO_InitTypeDef    GPIO;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
						   RCC_APB2Periph_GPIOC, ENABLE);
    
	switch(mode)
	{
		case Column:
			GPIO.GPIO_Pin   = ROW1 |ROW2 |ROW3 |ROW4;
			GPIO.GPIO_Mode  = GPIO_Mode_IPU;
			GPIO_Init(GPIOB, &GPIO);
		
//			GPIO.GPIO_Pin   = ROW3 ;
//			GPIO_Init(GPIOC, &GPIO);
//		
//			GPIO.GPIO_Pin   =  ROW4;
//			GPIO_Init(GPIOA, &GPIO);
		//==============================================//
			GPIO.GPIO_Pin   = COL1 | COL2|COL3| COL4;
			GPIO.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO.GPIO_Mode  = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOA, &GPIO);
		
//			GPIO.GPIO_Pin   = COL3| COL4;
//			GPIO_Init(GPIOC, &GPIO);
		
			
			//BIT_ADDR(GPIOA_IDR_Addr,n)
			BIT_ADDR(GPIOA_ODR_Addr,8) = 0;
			BIT_ADDR(GPIOA_ODR_Addr,9) = 0;
			BIT_ADDR(GPIOA_ODR_Addr,10) = 0;
			BIT_ADDR(GPIOA_ODR_Addr,11) = 0;
			break;
			
		case Row:
			GPIO.GPIO_Pin   = ROW1 | ROW2|ROW3 | ROW4;
			GPIO.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO.GPIO_Mode  = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOB, &GPIO);
		
//			GPIO.GPIO_Pin   = ROW3 ;
//			GPIO_Init(GPIOC, &GPIO);
//		
//			GPIO.GPIO_Pin   =  ROW4;
//			GPIO_Init(GPIOA, &GPIO);
		//==============================================//
			GPIO.GPIO_Pin   = COL1 | COL2|COL3| COL4;
			GPIO.GPIO_Mode  = GPIO_Mode_IPU;
			GPIO_Init(GPIOA, &GPIO);
		
//			GPIO.GPIO_Pin   = COL3| COL4;
//			GPIO_Init(GPIOC, &GPIO);
		
		
			BIT_ADDR(GPIOB_ODR_Addr,12) = 0;
			BIT_ADDR(GPIOB_ODR_Addr,13) = 0;
			BIT_ADDR(GPIOB_ODR_Addr,14) = 0;
			BIT_ADDR(GPIOB_ODR_Addr,15) = 0;
			break;
	}
}


/************************************/
/*return 1 to 16 			        */
/*erturn 0 is error or not respnose */
/************************************/
uint8_t GetMatrixKeyValue(void)
{
	uint8_t value = 0;
	
	MatrixModeConfig(Column);
	if (BIT_ADDR(GPIOB_IDR_Addr,12) == 0)
	{
		delay_ms (5);
		if (BIT_ADDR(GPIOB_IDR_Addr,12) == 0)
		{
			value += 0;
		}
	}
	else if (BIT_ADDR(GPIOB_IDR_Addr,13) == 0)
	{
		delay_ms(5);
		if (BIT_ADDR(GPIOB_IDR_Addr,13) == 0)
		{
			value += 4;
		}
	}
	else if (BIT_ADDR(GPIOB_IDR_Addr,14) == 0)
	{
		delay_ms(5);
		if (BIT_ADDR(GPIOB_IDR_Addr,14) == 0)
		{
			value += 8;
		}
	}
	else if (BIT_ADDR(GPIOB_IDR_Addr,15) == 0)
	{
		delay_ms(5);
		if (BIT_ADDR(GPIOB_IDR_Addr,15) == 0)
		{
			value += 12;
		}
	}
	else
	{
		value = 0;
	}
	
	MatrixModeConfig(Row);
	if (BIT_ADDR(GPIOA_IDR_Addr,8) == 0)
	{
		delay_ms(5);
		if (BIT_ADDR(GPIOA_IDR_Addr,8) == 0)
		{
			value += 1;
		}
	}
	else if (BIT_ADDR(GPIOA_IDR_Addr,9) == 0)
	{
		delay_ms(5);
		if (BIT_ADDR(GPIOA_IDR_Addr,9) == 0)
		{
			value += 2;
		}
	}
	else if (BIT_ADDR(GPIOA_IDR_Addr,10) == 0)
	{
		delay_ms(5);
		if (BIT_ADDR(GPIOA_IDR_Addr,10) == 0)
		{
			value += 3;
		}
	}
	else if (BIT_ADDR(GPIOA_IDR_Addr,11) == 0)
	{
		delay_ms(5);
		if (BIT_ADDR(GPIOA_IDR_Addr,11) == 0)
		{
			value += 4;
		}
	}
	else
	{
		value = 0;
	}
	
	
	return value;
}

