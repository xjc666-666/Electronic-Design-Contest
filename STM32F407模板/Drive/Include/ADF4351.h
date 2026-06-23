#ifndef _ADF4351_H_
#define _ADF4351_H_

#include "stm32f4xx.h"


// Pin definitions for ADF4351 (adjust as needed for your hardware)
#define ADF4351_PORT        GPIOD
#define ADF4351_CLK_PIN     GPIO_Pin_8
#define ADF4351_DATA_PIN    GPIO_Pin_10
#define ADF4351_LE_PIN      GPIO_Pin_12
#define ADF4351_CE_PIN      GPIO_Pin_14
#define ADF4351_LD_PIN      GPIO_Pin_9

// Pin control macros
#define ADF_CE_Set()        GPIO_SetBits(ADF4351_PORT, ADF4351_CE_PIN)
#define ADF_CE_Clr()        GPIO_ResetBits(ADF4351_PORT, ADF4351_CE_PIN)

#define ADF_LE_Set()        GPIO_SetBits(ADF4351_PORT, ADF4351_LE_PIN)
#define ADF_LE_Clr()        GPIO_ResetBits(ADF4351_PORT, ADF4351_LE_PIN)

#define ADF_DATA_Set()      GPIO_SetBits(ADF4351_PORT, ADF4351_DATA_PIN)
#define ADF_DATA_Clr()      GPIO_ResetBits(ADF4351_PORT, ADF4351_DATA_PIN)

#define ADF_CLK_Set()       GPIO_SetBits(ADF4351_PORT, ADF4351_CLK_PIN)
#define ADF_CLK_Clr()       GPIO_ResetBits(ADF4351_PORT, ADF4351_CLK_PIN)

// Function prototypes
void ADF4351_Wdata(uint32_t date);
void ADF4351_Init(uint32_t date);
void GPIO_AD4351_Init(void);
void ADF4351_WriteFre(double Fre);
uint8_t ADF4351_ReadLD(void); 

//ÓÃ·¨

//u32 FF=350;	
//ADF4351_Init(FF);
//    ADF4351_WriteFre(48.5);

//if(ADF4351_ReadLD()==0)
//			{
//				Delay_ms(1);
//				if(ADF4351_ReadLD()==0)
//				{
//			  ADF4351_WriteFre(48.5);
//				}
//			}


#endif
