/**
 ****************************************************************************************************
 * @file        ad9833.h
 * @author      科一电子（适配STM32F407标准库）
 * @version     V2.1
 * @date        2025-06-30
 * @brief       AD9833驱动程序 
 *              连接方式：
 *              5V      -->     VCC
 *              GND     -->     GND
 *              PA7     -->     SDA (MOSI)
 *              PA6     -->     SCLK
 *              PE2     -->     CS1
 *              PE3     -->     CS2
 ****************************************************************************************************
 */
#ifndef _AD9833_H_
#define _AD9833_H_

#include "stm32f4xx.h"

/*********************************GPIO引脚定义*********************************************/
#define AD9833_CS1_PORT     GPIOE
#define AD9833_CS1_PIN      GPIO_Pin_2
#define AD9833_CS2_PORT     GPIOE
#define AD9833_CS2_PIN      GPIO_Pin_3

#define AD9833_SPI_SCLK_PORT GPIOA
#define AD9833_SPI_SCLK_PIN  GPIO_Pin_6
#define AD9833_SPI_MOSI_PORT GPIOA
#define AD9833_SPI_MOSI_PIN  GPIO_Pin_7

/*********************************引脚控制宏*********************************************/

#define AD9833_CS1_L    (AD9833_CS1_PORT->BSRRH = AD9833_CS1_PIN)
#define AD9833_CS1_H    (AD9833_CS1_PORT->BSRRL = AD9833_CS1_PIN)
#define AD9833_CS2_L    (AD9833_CS2_PORT->BSRRH = AD9833_CS2_PIN)
#define AD9833_CS2_H    (AD9833_CS2_PORT->BSRRL = AD9833_CS2_PIN)

#define AD9833_SCLK_H   (AD9833_SPI_SCLK_PORT->BSRRL = AD9833_SPI_SCLK_PIN)
#define AD9833_SCLK_L   (AD9833_SPI_SCLK_PORT->BSRRH = AD9833_SPI_SCLK_PIN)
#define AD9833_MOSI_H   (AD9833_SPI_MOSI_PORT->BSRRL = AD9833_SPI_MOSI_PIN)
#define AD9833_MOSI_L   (AD9833_SPI_MOSI_PORT->BSRRH = AD9833_SPI_MOSI_PIN)

/*********************************AD9833通道选择*********************************************/
#define AD9833_ALL      0
#define AD9833_CH1      1
#define AD9833_CH2      2

/*********************************AD9833寄存器*********************************************/
#define AD9833_Reg_Cmd      (0 << 14)
#define AD9833_Reg_Freq0    (1 << 14)
#define AD9833_Reg_Freq1    (2 << 14)
#define AD9833_Reg_Phase0   (6 << 13)
#define AD9833_Reg_Phase1   (7 << 13)

/*********************************AD9833命令控制位*********************************************/
#define AD9833_B28          (1 << 13)
#define AD9833_HlB          (1 << 12)
#define AD9833_fSel0        (0 << 11)
#define AD9833_fSel1        (1 << 11)
#define AD9833_pSel0        (0 << 10)
#define AD9833_pSel1        (1 << 10)
#define AD9833_Pin_SW       (1 << 9)
#define AD9833_Reset        (1 << 8)
#define AD9833_Sleep1       (1 << 7)
#define AD9833_Sleep2       (1 << 6)
#define AD9833_Opbiten      (1 << 5)
#define AD9833_Sign_Pib     (1 << 4)
#define AD9833_Div2         (1 << 3)
#define AD9833_Mode         (1 << 1)

/*********************************AD9833输出波形类型*********************************************/
#define AD9833_Out_Sinus       ((0 << 5) | (0 << 1) | (0 << 3))
#define AD9833_Out_Triangle    ((0 << 5) | (1 << 1) | (0 << 3))
#define AD9833_Out_Msb         ((1 << 5) | (0 << 1) | (1 << 3))
#define AD9833_Out_Msb2        ((1 << 5) | (0 << 1) | (0 << 3))

/*******************************函数声明*******************************************/
void AD9833_Init(void);
void AD9833_Set_Register(uint16_t ch, uint16_t reg);
void AD9833_Set_Frequency(uint16_t ch, uint16_t Type, uint32_t Frequency);
void AD9833_Set_Phase(uint16_t Type, uint32_t Frequency, float Phase1, float Phase2);
void AD9833_Set_Channel(uint16_t Type1, uint32_t Freq1, float Phase1, 
                        uint16_t Type2, uint32_t Freq2, float Phase2);
void AD9833_Set_Resp(uint16_t ch, uint16_t Type, uint16_t Freq, float Phase);
void AD9833_Set_TwoChannel_Sine(double Freq1, float Phase1, double Freq2, float Phase2);

#endif /* _AD9833_H_ */
