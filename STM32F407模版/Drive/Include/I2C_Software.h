#ifndef __I2C_SOFTWARE_H
#define __I2C_SOFTWARE_H

#include "stm32f4xx.h"

#define GPIO_Port GPIOB
#define SCL	GPIO_Pin_8
#define SDA	GPIO_Pin_9

#define SDA_1() GPIO_SetBits(GPIO_Port, SDA)
#define SDA_0() GPIO_ResetBits(GPIO_Port, SDA)
#define SCL_1() GPIO_SetBits(GPIO_Port, SCL)
#define SCL_0() GPIO_ResetBits(GPIO_Port, SCL)

void I2C_GPIO_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(uint8_t data);
void I2C_Sendreg(uint8_t Addr, uint8_t data);
uint8_t I2C_WaitAck(void);
uint8_t I2C_ReceiveByte(uint8_t ack);

#endif

