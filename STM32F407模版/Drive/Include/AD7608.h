#ifndef __AD7608_H
#define __AD7608_H

#include "stm32f4xx.h"

#define AD7608OS0_H GPIO_SetBits(GPIOB,GPIO_Pin_13)
#define AD7608OS0_L GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define AD7608OS1_H GPIO_SetBits(GPIOB,GPIO_Pin_14)
#define AD7608OS1_L GPIO_ResetBits(GPIOB,GPIO_Pin_14)
#define AD7608OS2_H GPIO_SetBits(GPIOB,GPIO_Pin_15)
#define AD7608OS2_L GPIO_ResetBits(GPIOB,GPIO_Pin_15)

#define AD7608_CONVST_A_H GPIO_SetBits(GPIOA,GPIO_Pin_11)
#define AD7608_CONVST_A_L GPIO_ResetBits(GPIOA,GPIO_Pin_11)
#define AD7608_CONVST_B_H GPIO_SetBits(GPIOA,GPIO_Pin_12)
#define AD7608_CONVST_B_L GPIO_ResetBits(GPIOA,GPIO_Pin_12)

#define AD7608_SCLK_H GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define AD7608_SCLK_L GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define AD7608_RESET_H GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define AD7608_RESET_L GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define AD7608_CS_H GPIO_SetBits(GPIOA,GPIO_Pin_2)
#define AD7608_CS_L GPIO_ResetBits(GPIOA,GPIO_Pin_2)

#define AD7608_BUSY GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)
#define AD7608_DOUTA GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)
#define AD7608_DOUTB GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)

//#define AD7608_SYNC_H GPIO_SetBits(GPIOC,GPIO_Pin_6)
//#define AD7608_SYNC_L GPIO_ResetBits(GPIOC,GPIO_Pin_6)


//用法:
//double datatemp[8];
//    RCC_Configuration();
//    GPIO_Configuration();
//    NVIC_Configuration();
//	
//	
//    //AD7608设置过采样和抗混叠
//    AD7608_SETOS(0X05);
//    AD7608_RESET();
//    AD7608_CONVST_A_H;
//    AD7608_CONVST_B_H;
//    AD9833_Set_Frequency(AD9833_CH1,AD9833_Out_Sinus ,1000);

//    while (1) 
//    {
//        if(!AD7608_BUSY)
//        {
//            AD7608_CS_L;
//            for(uint8_t i = 0; i < 8; i++)
//            {
//                datatemp[7-i] = ad7606_ReadDATA();
//            }
//            AD7608_CS_H;
//            AD7608_STARTCONV();
//            while(AD7608_BUSY);

//            // 采样后串口输出8通道数据
//            for(uint8_t i = 0; i < 8; i++)
//            {
//                USART_SendDouble(USART1, datatemp[i], 2); // 保留2位小数
//            }
//            Serial_SendString(USART1, "\r\n"); // 一组采样后换行
//						Delay_ms(1000);
//        }
//			
//    }






void RCC_Configuration(void); 
void GPIO_Configuration(void);
void NVIC_Configuration(void); 
void Delay(uint32_t nCount);
void AD7608_SETOS(uint8_t osv);
void AD7608_RESET(void);
double ad7606_ReadDATA(void);
void AD7608_STARTCONV(void);

#endif
