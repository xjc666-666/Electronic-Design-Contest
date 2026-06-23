#ifndef __AD5593_H
#define __AD5593_H

#include "stm32f4xx.h"
#include <stdint.h>

// ----------- 宏定义 ----------
#define AD5593R_MODE_CONF           (0 << 4)
#define AD5593R_MODE_DAC_WRITE      (1 << 4)
#define AD5593R_MODE_ADC_READBACK   (4 << 4)
#define AD5593R_MODE_REG_READBACK   (7 << 4)
#define AD5593R_REG_ADC_SEQ         0x02
#define AD5593R_REG_RESET           0x07
#define AD5593R_NUM_CHANNELS        8

typedef struct {
    uint8_t i2c_addr; // 7位I2C地址
    uint16_t cached_dac[AD5593R_NUM_CHANNELS];
} AD5593R_HandleTypeDef;

// ----------- 软件I2C底层操作 ----------
void AD5593R_I2C_WriteBytes(uint8_t dev_addr, uint8_t *data, uint16_t len);
void AD5593R_I2C_ReadBytes(uint8_t dev_addr, uint8_t *data, uint16_t len);

// ----------- AD5593R操作接口 ----------
void AD5593R_Init(AD5593R_HandleTypeDef *dev, uint8_t addr);
int32_t AD5593R_WriteDAC(AD5593R_HandleTypeDef *dev, uint8_t chan, uint16_t value);
int32_t AD5593R_ReadADC(AD5593R_HandleTypeDef *dev, uint8_t chan, uint16_t *value);
int32_t AD5593R_RegWrite(AD5593R_HandleTypeDef *dev, uint8_t reg, uint16_t value);
int32_t AD5593R_RegRead(AD5593R_HandleTypeDef *dev, uint8_t reg, uint16_t *value);

#endif