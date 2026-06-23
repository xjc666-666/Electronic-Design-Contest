#include "stm32f4xx.h"
#include "I2C_Software.h"
#include "delay.h"

//
#define SDA_READ  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)
//

void I2C_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStruct.GPIO_Pin = SDA | SCL;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(GPIO_Port, &GPIO_InitStruct);

    I2C_Stop();
}

void I2C_Start(void)
{
    SDA_1();
    SCL_1();
    SDA_0();
    Delay_us(100);
    SCL_0();
    Delay_us(100);
}

void I2C_Stop(void)
{
    SCL_0();
    Delay_us(100);
    SDA_0();
    Delay_us(100);
    SCL_1();
    Delay_us(100);
    SDA_1();
    Delay_us(100);
}

void I2C_SendByte(uint8_t data)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        SCL_0();

        if(data & 0x80) {SDA_1();}
        else {SDA_0();}
    
        data <<= 1;
        Delay_us(100);
        SCL_1();
        Delay_us(100);
    }
    SCL_0();
    SDA_1();
    Delay_us(100);
    SCL_1();
    Delay_us(1000);
    SCL_0();
}

void I2C_Sendreg(uint8_t Addr, uint8_t data)
{
    I2C_Start();
    Delay_us(200);
    I2C_SendByte(0xC0);
    Delay_us(200);
    I2C_SendByte(Addr);
    Delay_us(200);
    I2C_SendByte(data);
    Delay_us(200);
    I2C_Stop();
}

uint8_t I2C_WaitAck(void)
{
    uint8_t errTime = 0;

    SDA_1();           // 释放SDA
    Delay_us(10);
    SCL_1();           // 拉高SCL, 准备采样ACK
    Delay_us(10);
    while(SDA_READ)    // 等待SDA拉低
    {
        errTime++;
        if(errTime > 100) // 超时退出
        {
            SCL_0();
            return 1;    // 没收到ACK
        }
        Delay_us(10);
    }
    SCL_0();           // 拉低SCL，I2C时钟回到低
    return 0;          // 收到ACK
}


// ack=1时发送ACK，ack=0时发送NACK
uint8_t I2C_ReceiveByte(uint8_t ack)
{
    uint8_t i, receive = 0;
    SDA_1(); // 释放SDA为输入
    for(i = 0; i < 8; i++)
    {
        SCL_0();
        Delay_us(10);
        SCL_1();
        receive <<= 1;
        if(SDA_READ) receive |= 0x01;
        Delay_us(10);
    }
    SCL_0();
    if(ack) // 发送ACK
        SDA_0();
    else    // 发送NACK
        SDA_1();
    Delay_us(10);
    SCL_1();
    Delay_us(10);
    SCL_0();
    SDA_1(); // 释放SDA
    return receive;
}
