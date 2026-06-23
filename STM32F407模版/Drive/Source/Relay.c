#include "stm32f4xx.h"
#include "Relay.h"
#include "delay.h"
#include "GPIO.h"

#define RELAY_CONTROL_PORT   GPIOB //继电器端口
#define RELAY_CONTROL_PIN    GPIO_Pin_8
#define DEBUG_LED_PORT       GPIOC //扫频指示灯
#define DEBUG_LED_PIN        GPIO_Pin_13

void Relay_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // 1. 使能 GPIOB 和 GPIOC 时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // 2. 初始化继电器控制脚 PB8
    GPIO_InitStruct.GPIO_Pin = RELAY_CONTROL_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RELAY_CONTROL_PORT, &GPIO_InitStruct);

    // 继电器初始化为低电平（关闭）
    GPIO_ResetBits(RELAY_CONTROL_PORT, RELAY_CONTROL_PIN);

    // 3. 初始化调试LED PC13
    GPIO_InitStruct.GPIO_Pin = DEBUG_LED_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_LED_PORT, &GPIO_InitStruct);

    // LED初始化为灭（PC13高电平熄灭，低电平点亮）
    GPIO_SetBits(DEBUG_LED_PORT, DEBUG_LED_PIN);
}

// 写高电平
void Relay_high(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_SetBits(GPIOx, GPIO_Pin); // 标准库方式，置高
}

// 写低电平
void Relay_low(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_ResetBits(GPIOx, GPIO_Pin); // 标准库方式，置低
}

void LED_on(void)
{
	GPIO_ResetBits(DEBUG_LED_PORT, DEBUG_LED_PIN);
}

void LED_off(void)
{
	
	 GPIO_SetBits(DEBUG_LED_PORT, DEBUG_LED_PIN);
}
