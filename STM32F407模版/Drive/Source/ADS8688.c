#include "ADS8688.h"
#include "Delay.h"

// External variables
uint16_t My_Ad[8] = {0};
float ad_real[2] = {0.0, 0.0};


// Initialize GPIOG pins for ADS8688
void ADS8688_GPIO_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
    GPIOG->MODER |= (GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 |
                     GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 |
                     GPIO_MODER_MODER7_0);
    GPIOG->MODER &= ~(GPIO_MODER_MODER6);
    GPIOG->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3 |
                       GPIO_OSPEEDER_OSPEEDR4 | GPIO_OSPEEDER_OSPEEDR5 |
                       GPIO_OSPEEDER_OSPEEDR7);
    GPIOG->BSRRH = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
}

void ADS8688_SPI_WB(uint8_t com)
{
    uint8_t com_temp = com;
    uint8_t s;
    ADS_8688_nCS_L;
    for (s = 0; s < 8; s++)
    {
        if (com_temp & 0x80)
        {
            ADS_8688_SDI_H;
        }
        else
        {
            ADS_8688_SDI_L;
        }
        Delay_us(1); // Added for debugging
        ADS_8688_SCLK_H;
        Delay_us(1); // Added for debugging
        com_temp <<= 1;
        ADS_8688_SCLK_L;
        Delay_us(1); // Added for debugging
    }
    ADS_8688_nCS_H;
}

uint8_t ADS8688_SPI_RB(void)
{
    uint8_t Rdata = 0;
    uint8_t s;
    ADS_8688_nCS_L;
    for (s = 0; s < 8; s++)
    {
        Rdata <<= 1;
        ADS_8688_SCLK_H;
        Delay_us(1); // Added for debugging
        if (ADS_8688_SDO)
        {
            Rdata |= 0x01;
        }
        else
        {
            Rdata &= 0xFE;
        }
        ADS_8688_SCLK_L;
        Delay_us(1); // Added for debugging
    }
    ADS_8688_nCS_H;
    return Rdata;
}

void ADS8688_WriteCommandReg(uint16_t command)
{
    ADS_8688_nCS_L;
    ADS8688_SPI_WB((command >> 8) & 0xFF);
    ADS8688_SPI_WB(command & 0xFF);
    ADS_8688_nCS_H;
}

void ADS8688_Write_Program_Register(uint8_t Addr, uint8_t data)
{
    ADS_8688_nCS_L;
    ADS8688_SPI_WB((Addr << 1) | 0x01);
    ADS8688_SPI_WB(data);
    ADS_8688_nCS_H;
}

uint8_t ADS8688_READ_Program_Register(uint8_t Addr)
{
    uint8_t data;
    ADS_8688_nCS_L;
    ADS8688_SPI_WB(Addr << 1);
    data = ADS8688_SPI_RB(); // Dummy read
    data = ADS8688_SPI_RB(); // Actual data
    ADS_8688_nCS_H;
    return data;
}

void Enter_RESET_MODE(void)
{
    ADS8688_WriteCommandReg(RST);
}

void AUTO_RST_Mode(void)
{
    ADS8688_WriteCommandReg(AUTO_RST);
}

void MAN_Ch_n_Mode(uint16_t ch)
{
    ADS8688_WriteCommandReg(ch);
}

void Set_CH_Range_Select(uint8_t ch, uint8_t range)
{
    ADS8688_Write_Program_Register(ch, range);
}

void Get_AUTO_RST_Mode_Data(uint16_t *outputdata, uint8_t chnum)
{
    uint8_t i, datah, datal;
    uint16_t data;
    for (i = 0; i < chnum; i++)
    {
        ADS_8688_nCS_L;
        ADS8688_SPI_WB(0x00);
        ADS8688_SPI_WB(0x00);
        datah = ADS8688_SPI_RB();
        datal = ADS8688_SPI_RB();
        ADS_8688_nCS_H;
        data = (datah << 8) | datal;
        *(outputdata + i) = data;
    }
}

void ADS8688_Init_Single(void)
{
    ADS8688_GPIO_Init();
    ADS_8688_RST_PD_L;
    Delay_us(2);
    ADS_8688_RST_PD_H;
    ADS_8688_DAISY_IN_L;
    Enter_RESET_MODE();
    ADS8688_Write_Program_Register(0x01, 0xFF);
    ADS8688_READ_Program_Register(0x01);
    Delay_us(2);
    ADS8688_Write_Program_Register(0x02, 0x00);
    ADS8688_Write_Program_Register(0x01, 0xFF);
    Set_CH_Range_Select(CH1, 0x00);
    MAN_Ch_n_Mode(MAN_Ch_1);
}

void ADS8688_Init_Mult(void)
{
    ADS8688_GPIO_Init();
    ADS_8688_RST_PD_H;
    ADS_8688_DAISY_IN_L;
    Delay_us(10);
    Enter_RESET_MODE();
    ADS8688_Write_Program_Register(0x00, 0x00);
    Delay_us(10);
    ADS8688_Write_Program_Register(0x01, 0x07);
    Delay_us(10);
    ADS8688_Write_Program_Register(0x02, 0xF8);
    Delay_us(10);
    ADS8688_Write_Program_Register(0x03, 0x03);
    Delay_us(10);
    Set_CH_Range_Select(CH1, 0x00);
    Delay_us(10);
    Set_CH_Range_Select(CH2, 0x00);
    Delay_us(10);
    Set_CH_Range_Select(CH3, 0x00);
    Delay_us(10);
    ADS8688_Write_Program_Register(0xA0, 0x03);
    Delay_us(10);
    AUTO_RST_Mode();
}

void get_ADS_1and2(uint16_t *My_Ad, float *real_ad)
{
    Get_AUTO_RST_Mode_Data(My_Ad, 2);
    gat_real_ad(real_ad);
}

uint16_t Get_MAN_Ch_n_Mode_Data(void)
{
    uint8_t datah, datal;
    ADS_8688_nCS_L;
    ADS8688_SPI_WB(0x00);
    ADS8688_SPI_WB(0x00);
    datah = ADS8688_SPI_RB();
    datal = ADS8688_SPI_RB();
    ADS_8688_nCS_H;
    return (datah << 8) | datal;
}

uint16_t get_ADS_ch1(void)
{
    return Get_MAN_Ch_n_Mode_Data();
}

double get_realdata(uint16_t x, uint8_t t)
{
    double y;
    // Use standard offset (32768) for Channel 1 to avoid negative bias
    y = (int16_t)(((double)(x - 32768) * 20.48 / (double)65535) * 1000);
    return y;
}

void gat_real_ad(float *real_ad)
{
    real_ad[0] = (float)get_realdata(My_Ad[0], 0);
    real_ad[1] = (float)get_realdata(My_Ad[1], 1);
}