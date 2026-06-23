#ifndef _PE4302_H_
#define _PE4302_H_

#include "stm32f4xx.h"
#include "delay.h"


#define PE_LE_0					PCout(2)
#define PE_LE_1					PCout(3)
#define PE_CLK					PCout(1)
#define PE_DAT					PCout(0)

#define PE_LE_0_EN      GPIO_SetBits(GPIOC, GPIO_Pin_2)
#define PE_LE_1_EN      GPIO_SetBits(GPIOC, GPIO_Pin_3)

#define PE_LE_0_DIS     GPIO_ResetBits(GPIOC, GPIO_Pin_2)
#define PE_LE_1_DIS     GPIO_ResetBits(GPIOC, GPIO_Pin_3)

#define PE_CLK_0        GPIO_ResetBits(GPIOC, GPIO_Pin_1)
#define PE_CLK_1        GPIO_SetBits(GPIOC, GPIO_Pin_1)

#define PE_DAT_0        GPIO_ResetBits(GPIOC, GPIO_Pin_0)
#define PE_DAT_1        GPIO_SetBits(GPIOC, GPIO_Pin_0)

void PE4302_Init(void);//PE4302  I/O口初始化
void PE4302_0_Set(uint8_t db);//PE4302芯片一设置衰减值   db范围：0~130，对应衰减值0~65dB
void PE4302_1_Set(uint8_t db);//PE4302芯片二设置衰减值   db范围：0~130，对应衰减值0~65dB
void PE4302_Position_Set(float db,uint8_t position);//选择PE4302芯片设置衰减值  db（衰减值）: 范围：0~31.5  position（选择）：范围 0~1 选择芯片
#endif
