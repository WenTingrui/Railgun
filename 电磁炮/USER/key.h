#ifndef __KEY_H
#define __KEY_H
#include "sys.h"
 
#include "stm32f10x.h"
#include "delay.h"

//C0,C2,A0,A2,A4,A6,C4,B0

#define		ROW_1		PBout(12)
#define		ROW_2		PBout(13)
#define		ROW_3		PBout(14)
#define		ROW_4		PBout(15)

#define		COL_1		PAout(8)
#define		COL_2		PAout(9)
#define		COL_3		PAout(10)
#define		COL_4		PAout(11)


#define		ROW1		GPIO_Pin_12
#define		ROW2		GPIO_Pin_13
#define		ROW3		GPIO_Pin_14
#define		ROW4		GPIO_Pin_15
#define		COL1		GPIO_Pin_8
#define		COL2		GPIO_Pin_9
#define		COL3		GPIO_Pin_10
#define		COL4		GPIO_Pin_11

typedef enum
{
	Column,
	Row,
	Normal
} MatirxMode;

extern void MatrixKeyConfiguration(void);
extern uint8_t GetMatrixKeyValue(void);
#endif
