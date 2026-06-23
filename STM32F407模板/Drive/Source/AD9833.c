/**
 ****************************************************************************************************
 * @file        ad9833.c
 * @author      XJC
 * @version     V2.1
 * @date        2025-06-30
 * @brief       AD9833驱动程序
 ****************************************************************************************************
 */

#include "AD9833.h"

/* 软件SPI延时函数 */
static inline void AD9833_SPI_Delay(void)
{
    volatile uint8_t i;
    for(i = 0; i < 30; i++) { __NOP(); }
}

/* 软件SPI发送16位数据，SPI模式2（CPOL=1, CPHA=0） */
static void AD9833_SPI_Write_16Bit(uint16_t data)
{
    uint8_t i;
    AD9833_SCLK_H;
    AD9833_SPI_Delay();
    for (i = 0; i < 16; i++)
    {
        if (data & 0x8000)
            AD9833_MOSI_H;
        else
            AD9833_MOSI_L;
        AD9833_SPI_Delay();

        AD9833_SCLK_L;
        AD9833_SPI_Delay();
        AD9833_SCLK_H;
        AD9833_SPI_Delay();
        data <<= 1;
    }
    AD9833_SCLK_H;
    AD9833_MOSI_L;
}

/* GPIO初始化 */
static void AD9833_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE, ENABLE);

    // CS引脚
    GPIO_InitStructure.GPIO_Pin = AD9833_CS1_PIN | AD9833_CS2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(AD9833_CS1_PORT, &GPIO_InitStructure);

    // SPI SCLK/MOSI引脚
    GPIO_InitStructure.GPIO_Pin = AD9833_SPI_SCLK_PIN | AD9833_SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(AD9833_SPI_SCLK_PORT, &GPIO_InitStructure);

    // 默认状态
    AD9833_CS1_H;
    AD9833_CS2_H;
    AD9833_SCLK_H;
    AD9833_MOSI_L;
    for(volatile int i=0; i<10000; ++i); // 粗略延时
}

/* SPI1初始化（兼容原接口，实际上初始化GPIO） */
void spi1_init(void)
{
    AD9833_GPIO_Init();
}

/* SPI1读写16位数据（兼容原接口） */
uint16_t spi1_read_write_16bit(uint16_t txdata)
{
    AD9833_SPI_Write_16Bit(txdata);
    return txdata;
}

/* AD9833初始化 */
void AD9833_Init(void)
{
    spi1_init();
    for(volatile int i=0; i<200000; ++i); // 粗略延时~100ms
    AD9833_Set_Register(AD9833_ALL, AD9833_Reg_Cmd | AD9833_Reset);
    for(volatile int i=0; i<20000; ++i); // 粗略延时~10ms
}

/* 设置AD9833寄存器 */
void AD9833_Set_Register(uint16_t ch, uint16_t reg)
{
    if (ch == AD9833_ALL)
    {
        AD9833_CS1_L; AD9833_CS2_L;
        spi1_read_write_16bit(reg);
        AD9833_CS1_H; AD9833_CS2_H;
    }
    else if (ch == AD9833_CH1)
    {
        AD9833_CS1_L;
        spi1_read_write_16bit(reg);
        AD9833_CS1_H;
    }
    else if (ch == AD9833_CH2)
    {
        AD9833_CS2_L;
        spi1_read_write_16bit(reg);
        AD9833_CS2_H;
    }
}

/* 设置频率 */
void AD9833_Set_Frequency(uint16_t ch, uint16_t Type, uint32_t Frequency)
{
	  Frequency*=10.73742;
    uint16_t Fre_H = AD9833_Reg_Freq0;
    uint16_t Fre_L = AD9833_Reg_Freq0;
    Fre_H |= (Frequency & 0xFFFC000) >> 14;
    Fre_L |= (Frequency & 0x3FFF);

    AD9833_Set_Register(ch, AD9833_B28);
    AD9833_Set_Register(ch, Fre_L);
    AD9833_Set_Register(ch, Fre_H);
    AD9833_Set_Register(ch, AD9833_Reg_Cmd | AD9833_B28 | Type);
}

/* 设置相位 */
void AD9833_Set_Phase(uint16_t Type, uint32_t Frequency, float Phase1, float Phase2)
{
    uint16_t Fre_H = AD9833_Reg_Freq0;
    uint16_t Fre_L = AD9833_Reg_Freq0;
    uint16_t Value1 = AD9833_Reg_Phase0;
    uint16_t Value2 = AD9833_Reg_Phase0;
    uint16_t P1 = 0, P2 = 0;

    P1 = (Phase1 >= 360 || Phase1 == 0) ? 0 : (uint16_t)((float)Phase1 / 360.0 * 4095.0);
    P2 = (Phase2 >= 360 || Phase2 == 0) ? 0 : (uint16_t)((float)Phase2 / 360.0 * 4095.0);

    Fre_H |= (Frequency & 0xFFFC000) >> 14;
    Fre_L |= (Frequency & 0x3FFF);
    Value1 |= P1 & 0x0FFF;
    Value2 |= P2 & 0x0FFF;

    AD9833_Set_Register(AD9833_ALL, AD9833_B28 | AD9833_Reset);
    AD9833_Set_Register(AD9833_ALL, Fre_L);
    AD9833_Set_Register(AD9833_ALL, Fre_H);
    AD9833_Set_Register(AD9833_CH1, Value1);
    AD9833_Set_Register(AD9833_CH2, Value2);
    AD9833_Set_Register(AD9833_ALL, AD9833_Reg_Cmd | AD9833_B28 | Type);
}

/* 分别控制两个通道 */
void AD9833_Set_Channel(uint16_t Type1, uint32_t Freq1, float Phase1,
                        uint16_t Type2, uint32_t Freq2, float Phase2)
{
    uint16_t Fre1_H, Fre1_L, Fre2_H, Fre2_L;
    uint16_t Value1 = AD9833_Reg_Phase0;
    uint16_t Value2 = AD9833_Reg_Phase1;
    uint16_t Ph1, Ph2;

    Ph1 = (Phase1 >= 360) ? 0 : (uint16_t)((Phase1 / 360.0) * 4095.0);
    Ph2 = (Phase2 >= 360) ? 0 : (uint16_t)((Phase2 / 360.0) * 4095.0);

    Value1 |= Ph1 & 0x0FFF;
    Value2 |= Ph2 & 0x0FFF;

    Fre1_H = AD9833_Reg_Freq0 | ((Freq1 & 0xFFFC000) >> 14);
    Fre1_L = AD9833_Reg_Freq0 | (Freq1 & 0x3FFF);

    Fre2_H = AD9833_Reg_Freq0 | ((Freq2 & 0xFFFC000) >> 14);
    Fre2_L = AD9833_Reg_Freq0 | (Freq2 & 0x3FFF);

    AD9833_Set_Register(AD9833_ALL, AD9833_B28 | AD9833_Reset);
    AD9833_Set_Register(AD9833_CH1, Fre1_L);
    AD9833_Set_Register(AD9833_CH1, Fre1_H);
    AD9833_Set_Register(AD9833_CH2, Fre2_L);
    AD9833_Set_Register(AD9833_CH2, Fre2_H);
    AD9833_Set_Register(AD9833_CH1, Value1);
    AD9833_Set_Register(AD9833_CH2, Value2);
    AD9833_Set_Register(AD9833_CH1, AD9833_Reg_Cmd | AD9833_B28 | Type1);
    AD9833_Set_Register(AD9833_CH2, AD9833_Reg_Cmd | AD9833_B28 | Type2);
}

/* 设置响应 */
void AD9833_Set_Resp(uint16_t ch, uint16_t Type, uint16_t Freq, float Phase)
{
    uint16_t Fre_H = AD9833_Reg_Freq0;
    uint16_t Fre_L = AD9833_Reg_Freq0;
    uint16_t Ph;

    Ph = (uint16_t)((float)Phase / 360 * 4095.0) & 0x0FFF;
    Fre_H |= (Freq & 0xFFFC000) >> 14;
    Fre_L |= (Freq & 0x3FFF);

    AD9833_Set_Register(ch, AD9833_B28);
    AD9833_Set_Register(ch, Fre_L);
    AD9833_Set_Register(ch, Fre_H);
    AD9833_Set_Register(ch, Ph);
    AD9833_Set_Register(ch, AD9833_Reg_Cmd | AD9833_B28 | Type);
}

/* 同时输出正弦波 */
void AD9833_Set_TwoChannel_Sine(double Freq1, float Phase1, double Freq2, float Phase2)
{
    uint16_t Fre1_H, Fre1_L, Fre2_H, Fre2_L;
    uint16_t Value1 = AD9833_Reg_Phase0;
    uint16_t Value2 = AD9833_Reg_Phase1;
    uint16_t Ph1, Ph2;
    double frequence_mid = 1000000;   // 适合25M晶振

    Ph1 = (Phase1 >= 360) ? 0 : (uint16_t)((Phase1 / 360.0) * 4095.0);
    Ph2 = (Phase2 >= 360) ? 0 : (uint16_t)((Phase2 / 360.0) * 4095.0);

    Value1 |= Ph1 & 0x0FFF;
    Value2 |= Ph2 & 0x0FFF;

    uint32_t frequence_hex = (uint32_t)((Freq1 / 1000000) * frequence_mid);
    Fre1_H = AD9833_Reg_Freq0 | ((frequence_hex & 0xFFFC000) >> 14);
    Fre1_L = AD9833_Reg_Freq0 | (frequence_hex & 0x3FFF);

    frequence_hex = (uint32_t)((Freq2 / 1000000) * frequence_mid);
    Fre2_H = AD9833_Reg_Freq0 | ((frequence_hex & 0xFFFC000) >> 14);
    Fre2_L = AD9833_Reg_Freq0 | (frequence_hex & 0x3FFF);

    AD9833_Set_Register(AD9833_ALL, AD9833_B28 | AD9833_Reset);
    AD9833_Set_Register(AD9833_CH1, Fre1_L);
    AD9833_Set_Register(AD9833_CH1, Fre1_H);
    AD9833_Set_Register(AD9833_CH2, Fre2_L);
    AD9833_Set_Register(AD9833_CH2, Fre2_H);
    AD9833_Set_Register(AD9833_CH1, Value1);
    AD9833_Set_Register(AD9833_CH2, Value2);
    AD9833_Set_Register(AD9833_ALL, AD9833_Reg_Cmd | AD9833_B28 | AD9833_Out_Sinus);
}
