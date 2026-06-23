#include "stm32f4xx.h"
#include "Delay.h"
#include "TLV5638.h"

/* 管脚电平控制*/
#define SCLK_0 GPIO_ResetBits(GPIOA, GPIO_Pin_3)
#define SCLK_1 GPIO_SetBits(GPIOA, GPIO_Pin_3)
#define DIN_0  GPIO_ResetBits(GPIOA, GPIO_Pin_2)
#define DIN_1  GPIO_SetBits(GPIOA, GPIO_Pin_2)
#define CS_0   GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#define CS_1   GPIO_SetBits(GPIOA, GPIO_Pin_1)

/* GPIO初始化 */
void TLV5638_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_2 | GPIO_Pin_1; // SCLK, DIN, CS
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    CS_1;
    DIN_0;
    SCLK_0;
}

/* 发送16位数据 */
void TLV5638_Send2Byte(uint16_t data)
{
    CS_0;
    uint16_t temp = data;
    for(uint8_t i = 0; i < 16; i++)
    {
        if(temp & 0x8000) DIN_1;
        else DIN_0;

        SCLK_1;
        Delay_us(1);
        SCLK_0;
        Delay_us(1);

        temp <<= 1;
    }
    CS_1;
}

/* 设置DACB输出 */
void TLV5638_Set_DACB(uint16_t DAC_Value)
{
    TLV5638_Send2Byte(LOAD_B_BUFFER | DAC_Value);
}

/* 设置DACA输出 */
void TLV5638_Set_DACA(uint16_t DAC_Value)
{
    TLV5638_Send2Byte(LOAD_A_UPDATE_BUFFER_B | DAC_Value);
}

/* 关机 */
void TLV5638_PWR_DOWN(void)
{
    TLV5638_Send2Byte(PWR_DOWN);
}

/* 参考电压1.024V */
void TLV5638_Set_Vref_1024(void)
{
    TLV5638_Send2Byte(VREF1024);
}

/* 参考电压2.048V */
void TLV5638_Set_Vref_2048(void)
{
    TLV5638_Send2Byte(VREF2048);
}

/* 完整初始化 */
void TLV5638_Init(uint16_t VREF)
{
    TLV5638_GPIO_Init();
    if(VREF == VREF1024)      TLV5638_Set_Vref_1024();
    else if(VREF == VREF2048) TLV5638_Set_Vref_2048();
}

void set_voltage_mV(uint16_t voltage_mV, uint16_t VREF)
{
    uint16_t dac_value = 0;
    if (VREF==VREF1024)
        dac_value = (uint16_t)((float)voltage_mV / 1024 * 4095); // 12位映射
    else if (VREF==VREF2048)
        dac_value = (uint16_t)((float)voltage_mV / 2048 * 4095);

    // 设置 DAC 输出（可以根据需要选择A或B通道）
    TLV5638_Set_DACB((dac_value/2)-15);
		//VCA824板子的话就是-4.5
}