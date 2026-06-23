#ifndef __ADS8688_H_
#define __ADS8688_H_

#include <stdint.h>
#include "stm32f4xx.h"

// GPIO pin definitions for ADS8688 (using GPIOG)
#define ADS_8688_DAISY_IN_H  GPIOG->BSRRL = GPIO_Pin_5  // Set DAISY_IN (PG5)
#define ADS_8688_DAISY_IN_L  GPIOG->BSRRH = GPIO_Pin_5  // Clear DAISY_IN (PG5)
#define ADS_8688_SCLK_H      GPIOG->BSRRL = GPIO_Pin_4  // Set SCLK (PG4)
#define ADS_8688_SCLK_L      GPIOG->BSRRH = GPIO_Pin_4  // Clear SCLK (PG4)
#define ADS_8688_nCS_H       GPIOG->BSRRL = GPIO_Pin_7  // Set nCS (PG7)
#define ADS_8688_nCS_L       GPIOG->BSRRH = GPIO_Pin_7  // Clear nCS (PG7)
#define ADS_8688_RST_PD_H    GPIOG->BSRRL = GPIO_Pin_2  // Set RST_PD (PG2)
#define ADS_8688_RST_PD_L    GPIOG->BSRRH = GPIO_Pin_2  // Clear RST_PD (PG2)
#define ADS_8688_SDI_H       GPIOG->BSRRL = GPIO_Pin_3  // Set SDI (PG3)
#define ADS_8688_SDI_L       GPIOG->BSRRH = GPIO_Pin_3  // Clear SDI (PG3)
#define ADS_8688_SDO         (GPIOG->IDR & GPIO_Pin_6)   // Read SDO (PG6)

// ADS8688 command definitions
#define NO_OP        0x0000
#define STDBY        0x8200
#define PWR_DN       0x8300
#define RST          0x8500
#define AUTO_RST     0xA000
#define MAN_Ch_1     0xC000
#define MAN_Ch_2     0xC400
#define MAN_Ch_3     0xC800
#define MAN_Ch_4     0xCC00
#define MAN_Ch_5     0xD000
#define MAN_Ch_6     0xD400
#define MAN_Ch_7     0xD800
#define MAN_Ch_8     0xDC00
#define MAN_AUX      0xE000

// Channel register addresses
#define CH1 0x05
#define CH2 0x06
#define CH3 0x07
#define CH4 0x08
#define CH5 0x09
#define CH6 0x0a
#define CH7 0x0b
#define CH8 0x0c

// External variables
extern uint16_t My_Ad[8];
extern float ad_real[2];

// Function prototypes
void ADS8688_GPIO_Init(void); // Added GPIO initialization
void ADS8688_Init_Single(void);
uint16_t get_ADS_ch1(void);
void ADS8688_Init_Mult(void);
double get_realdata(uint16_t x, uint8_t t);
void gat_real_ad(float *real_ad);
void get_ADS_1and2(uint16_t *My_Ad, float *real_ad);
void Get_AUTO_RST_Mode_Data(uint16_t *outputdata, uint8_t chnum);
void AUTO_RST_Mode(void);
void ADS8688_SPI_WB(uint8_t com);
uint8_t ADS8688_SPI_RB(void);
void ADS8688_WriteCommandReg(uint16_t command);
void ADS8688_Write_Program_Register(uint8_t Addr, uint8_t data);
uint8_t ADS8688_READ_Program_Register(uint8_t Addr);
void Enter_RESET_MODE(void);
void Set_CH_Range_Select(uint8_t ch, uint8_t range);


#endif /* __ADS8688_H_ */