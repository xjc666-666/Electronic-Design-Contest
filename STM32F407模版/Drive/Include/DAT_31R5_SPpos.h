#ifndef __DAT_31R5_SPPOS_H
#define __DAT_31R5_SPPOS_H

#include "stm32f4xx.h"
#include "GPIO.h"

#define LE_1 Set_GPIO_High(GPIOD, GPIO_Pin_13);
#define LE_0 Set_GPIO_Low(GPIOD, GPIO_Pin_13);
#define CLK_1 Set_GPIO_High(GPIOD, GPIO_Pin_14);
#define CLK_0 Set_GPIO_Low(GPIOD, GPIO_Pin_14);
#define DAT_1 Set_GPIO_High(GPIOD, GPIO_Pin_15);
#define DAT_0 Set_GPIO_Low(GPIOD, GPIO_Pin_15);

#define LE2_1 Set_GPIO_High(GPIOD, GPIO_Pin_10);
#define LE2_0 Set_GPIO_Low(GPIOD, GPIO_Pin_10);
#define CLK2_1 Set_GPIO_High(GPIOD, GPIO_Pin_11);
#define CLK2_0 Set_GPIO_Low(GPIOD, GPIO_Pin_11);
#define DAT2_1 Set_GPIO_High(GPIOD, GPIO_Pin_12);
#define DAT2_0 Set_GPIO_Low(GPIOD, GPIO_Pin_12);

void DAT_31R5_SPpos_Init(void);
void DAT_31R5_SPpos_SendReg(void);
void DAT_31R5_SPpos_WriteReg(void);
void DAT_31R5_SPpos_SendReg2(void);
void DAT_31R5_SPpos_WriteReg2(void);
void DAT_31R5_SPpos_Start(void);

#endif
