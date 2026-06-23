#include "AD5593.h"
#include "I2C_Software.h"
#include "delay.h"

// ------- 软件I2C实现 -------
void AD5593R_I2C_WriteBytes(uint8_t dev_addr, uint8_t *data, uint16_t len)
{
    I2C_Start();
    I2C_SendByte((dev_addr << 1) | 0); // 0:写
    if (I2C_WaitAck()) { I2C_Stop(); return; }
    for(uint16_t i = 0; i < len; i++)
    {
        I2C_SendByte(data[i]);
        if (I2C_WaitAck()) { I2C_Stop(); return; }
    }
    I2C_Stop();
}

void AD5593R_I2C_ReadBytes(uint8_t dev_addr, uint8_t *data, uint16_t len)
{
    I2C_Start();
    I2C_SendByte((dev_addr << 1) | 1); // 1:读
    if (I2C_WaitAck()) { I2C_Stop(); return; }
    for(uint16_t i = 0; i < len; i++)
    {
        data[i] = I2C_ReceiveByte(i < (len-1) ? 1 : 0); // 最后一个NOACK
    }
    I2C_Stop();
}

// ------- AD5593R接口 -------
void AD5593R_Init(AD5593R_HandleTypeDef *dev, uint8_t addr)
{
    dev->i2c_addr = addr & 0x7F;
    for(int i=0; i<AD5593R_NUM_CHANNELS; i++) dev->cached_dac[i] = 0;
}

int32_t AD5593R_WriteDAC(AD5593R_HandleTypeDef *dev, uint8_t chan, uint16_t value)
{
    uint8_t buf[3];
    buf[0] = AD5593R_MODE_DAC_WRITE | (chan & 0x0F);
    buf[1] = (value >> 8) & 0x0F;
    buf[2] = value & 0xFF;
    AD5593R_I2C_WriteBytes(dev->i2c_addr, buf, 3);
    dev->cached_dac[chan] = value;
    return 0;
}

int32_t AD5593R_ReadADC(AD5593R_HandleTypeDef *dev, uint8_t chan, uint16_t *value)
{
    uint8_t buf[3];
    uint16_t mask = (1 << chan);
    buf[0] = AD5593R_MODE_CONF | AD5593R_REG_ADC_SEQ;
    buf[1] = mask >> 8;
    buf[2] = mask & 0xFF;
    AD5593R_I2C_WriteBytes(dev->i2c_addr, buf, 3);

    buf[0] = AD5593R_MODE_ADC_READBACK;
    AD5593R_I2C_WriteBytes(dev->i2c_addr, buf, 1);

    uint8_t rbuf[2];
    AD5593R_I2C_ReadBytes(dev->i2c_addr, rbuf, 2);
    *value = ((rbuf[0] & 0x0F) << 8) | rbuf[1];
    return 0;
}

int32_t AD5593R_RegWrite(AD5593R_HandleTypeDef *dev, uint8_t reg, uint16_t value)
{
    uint8_t buf[3];
    buf[0] = AD5593R_MODE_CONF | reg;
    buf[1] = value >> 8;
    buf[2] = value & 0xFF;
    AD5593R_I2C_WriteBytes(dev->i2c_addr, buf, 3);
    return 0;
}

int32_t AD5593R_RegRead(AD5593R_HandleTypeDef *dev, uint8_t reg, uint16_t *value)
{
    uint8_t buf[2];
    buf[0] = AD5593R_MODE_REG_READBACK | reg;
    AD5593R_I2C_WriteBytes(dev->i2c_addr, buf, 1);
    AD5593R_I2C_ReadBytes(dev->i2c_addr, buf, 2);
    *value = ((uint16_t)buf[0] << 8) | buf[1];
    return 0;
}