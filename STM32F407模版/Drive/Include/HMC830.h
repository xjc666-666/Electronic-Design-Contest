/**********************************************************
                       HMC830 Driver for STM32F407
**********************************************************/
#ifndef __HMC830_H
#define __HMC830_H

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>

// Pin definitions for HMC830
#define HMC830_CEN  GPIO_Pin_8   // PC8: Chip Enable
#define HMC830_SEN  GPIO_Pin_11  // PC11: Serial Enable
#define HMC830_SDI  GPIO_Pin_10  // PC10: Serial Data Input
#define HMC830_SCK  GPIO_Pin_9   // PC9: Serial Clock
#define HMC830_LD   GPIO_Pin_12  // PC12: Lock Detect (Input)

#define HMC830_CEN_PORT  GPIOC
#define HMC830_SEN_PORT  GPIOC
#define HMC830_SDI_PORT  GPIOC
#define HMC830_SCK_PORT  GPIOC
#define HMC830_LD_PORT   GPIOC

#define HMC830_CEN_SET   GPIO_SetBits(HMC830_CEN_PORT, HMC830_CEN)
#define HMC830_CEN_RESET GPIO_ResetBits(HMC830_CEN_PORT, HMC830_CEN)
#define HMC830_SEN_SET   GPIO_SetBits(HMC830_SEN_PORT, HMC830_SEN)
#define HMC830_SEN_RESET GPIO_ResetBits(HMC830_SEN_PORT, HMC830_SEN)
#define HMC830_SDI_SET   GPIO_SetBits(HMC830_SDI_PORT, HMC830_SDI)
#define HMC830_SDI_RESET GPIO_ResetBits(HMC830_SDI_PORT, HMC830_SDI)
#define HMC830_SCK_SET   GPIO_SetBits(HMC830_SCK_PORT, HMC830_SCK)
#define HMC830_SCK_RESET GPIO_ResetBits(HMC830_SCK_PORT, HMC830_SCK)
#define HMC830_LD_READ   GPIO_ReadInputDataBit(HMC830_LD_PORT, HMC830_LD)

// External register variables
extern uint32_t REG0_830;
extern uint32_t REGF0_830;
extern uint32_t REGF1_830;
extern uint32_t REG12_830;

// Function prototypes
void Hmc830Config(void);
void Hmc830SpiWrite(uint8_t addr, uint8_t *data);
void Hmc830Init(uint8_t *data);
void Hmc830WriteFrequency(uint32_t fre);
void Hmc830ReadId(uint32_t reg);
void Hmc830SetFrequency(uint64_t wfre);

#endif /* __HMC830_H */