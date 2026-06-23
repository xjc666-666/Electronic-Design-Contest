#ifndef __RELAY_H
#define __RELAY_H

#include "stm32f4xx.h"

//¼ÌµçÆ÷µÈ¿ØÖÆÂß¼­
void Relay_init(void);
void Relay_high(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void Relay_low(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void LED_on(void);
void LED_off(void);

#endif
