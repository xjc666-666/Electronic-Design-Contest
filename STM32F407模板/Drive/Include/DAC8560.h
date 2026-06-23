#ifndef __DAC8560_H
#define __DAC8560_H


#include "stm32f4xx.h"

// 片选、时钟、数据IO定义
#define CS_H    GPIO_SetBits(GPIOA, GPIO_Pin_6)
#define CS_L    GPIO_ResetBits(GPIOA, GPIO_Pin_6)
#define SCLK_H  GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define SCLK_L  GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define DIN_H   GPIO_SetBits(GPIOC, GPIO_Pin_4)
#define DIN_L   GPIO_ResetBits(GPIOC, GPIO_Pin_4)

void DAC8560_Init(void);
void WriteDac8560(uint32_t Data);
void SetVoltage(float v);


#endif
