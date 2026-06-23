#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "stm32f4xx.h"

// Pin definitions (adjust according to your hardware setup)
#define TM1637_CLK_PORT       GPIOB
#define TM1637_CLK_PIN        GPIO_Pin_6
#define TM1637_DIO_PORT       GPIOB
#define TM1637_DIO_PIN        GPIO_Pin_7

// Function prototypes
void TM1637_I2CStart(void);
void TM1637_I2CStop(void);
void TM1637_I2CAck(void);
void TM1637_I2CWrByte(uint8_t oneByte);
void SmgDisplay(const char* str, int showColon);
uint8_t TM1637_ScanKey(void);
void TM1637_Init(void);

#endif /* __TM1637_H */