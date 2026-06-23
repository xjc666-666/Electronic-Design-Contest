#include "stm32f4xx.h"
#include "DAC8560.h"
#include "delay.h"

void DAC8560_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // 开启GPIOA、GPIOC时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // 配置PA6、PA7为推挽输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置PC4为推挽输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    // 置初始状态
    CS_H;
    SCLK_L;
    DIN_L;
}

void WriteDac8560(uint32_t Data)
{
    SCLK_H;
    DIN_L;
    CS_H;
    Delay_ms(1);
    CS_L;
    for(uint8_t i = 0; i < 24; i++)
    {
        SCLK_H;
        if(Data & 0x800000)
            DIN_H;
        else
            DIN_L;
        Data <<= 1;
        SCLK_L;
    }
    CS_H;
    Delay_ms(1);
    DIN_L;
}

void SetVoltage(float v) // v单位为mV  范围：-2V ~ 2V  再往上就不准了，最大2.5V
{
    // 拟合参数
    const float k = 0.9895263157895; //标定电压值
    const float b = 13.7326315789474; //标定电压值

	  // 限幅
    if(v > 2000.0f) v = 2000.0f;
	  if(v < -2000.0f) v = -2000.0f;
	
    // 反推设定值
	  float set = v>0?v:((v - b) / k);

    // 电压(mV) -> 数据
    uint16_t data = (uint16_t)((set / 2379.0f + 1.0f ) * 32768.0f);

    // 组装为24位数据，高8位控制字为0x30
    uint32_t dac_data = ((0x30 << 16) | data);

    WriteDac8560(dac_data);
}
