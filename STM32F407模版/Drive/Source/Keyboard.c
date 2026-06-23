#include "stm32f4xx.h"
#include "Keyboard.h"
#include "Delay.h"


// I2C Start condition
void TM1637_I2CStart(void)
{
    GPIO_SetBits(TM1637_CLK_PORT, TM1637_CLK_PIN);
    GPIO_SetBits(TM1637_DIO_PORT, TM1637_DIO_PIN);
    Delay_us(2);
    GPIO_ResetBits(TM1637_DIO_PORT, TM1637_DIO_PIN);
}

// I2C Stop condition
void TM1637_I2CStop(void)
{
    GPIO_ResetBits(TM1637_CLK_PORT, TM1637_CLK_PIN);
    Delay_us(2);
    GPIO_ResetBits(TM1637_DIO_PORT, TM1637_DIO_PIN);
    Delay_us(2);
    GPIO_SetBits(TM1637_CLK_PORT, TM1637_CLK_PIN);
    Delay_us(2);
    GPIO_SetBits(TM1637_DIO_PORT, TM1637_DIO_PIN);
}

// I2C Acknowledge
void TM1637_I2CAck(void)
{
    GPIO_ResetBits(TM1637_CLK_PORT, TM1637_CLK_PIN);
    Delay_us(5);
    while (GPIO_ReadInputDataBit(TM1637_DIO_PORT, TM1637_DIO_PIN));
    GPIO_SetBits(TM1637_CLK_PORT, TM1637_CLK_PIN);
    Delay_us(2);
    GPIO_ResetBits(TM1637_CLK_PORT, TM1637_CLK_PIN);
}

// Write one byte to TM1637
void TM1637_I2CWrByte(uint8_t oneByte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        GPIO_ResetBits(TM1637_CLK_PORT, TM1637_CLK_PIN);
        if (oneByte & 0x01)
        {
            GPIO_SetBits(TM1637_DIO_PORT, TM1637_DIO_PIN);
        }
        else
        {
            GPIO_ResetBits(TM1637_DIO_PORT, TM1637_DIO_PIN);
        }
        Delay_us(3);
        oneByte >>= 1;
        GPIO_SetBits(TM1637_CLK_PORT, TM1637_CLK_PIN);
        Delay_us(3);
    }
}

// 数码管段码表（0-9）
const uint8_t segmentCodes[] = {
    // 数字0-9（0x00-0x09）
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,
    // 字母A-Z（0x0A-0x19）
    0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x6F, 0x76, 0x38, 0x00, // A B C D E F G H I (J不可直接显示)
    0x00, 0x76, 0x54, 0x37, 0x54, 0x73, 0x50, 0x6D, 0x78, 0x3E, // K L M N O P Q R S T
    0x3E, 0x1C, 0x00, 0x6E, 0x52, 0x49 , // U V W X Y Z
    0x00, 0x40  // 空格(0x1A), 负号(0x1B)
};

void SmgDisplay(const char* str, int showColon) {
     uint8_t digits[4] = {0x00}; // 默认全灭

    // 解析4位字符串
    for (int i = 0; i < 4 && str[i] != '\0'; i++) {
        char c = str[i];
        if (c >= '0' && c <= '9') {
            digits[i] = segmentCodes[c - '0'];  // 数字
        } else if (c >= 'A' && c <= 'Z') {
            digits[i] = segmentCodes[c - 'A' + 10]; // 字母A-Z
        } else if (c == '-') {
            digits[i] = segmentCodes[27];       // 负号
        } else {
            digits[i] = segmentCodes[26];       // 其他字符显示空格
        }
        
        // 在百位（第2位）添加冒号
        if (i == 1 && showColon) {
            digits[i] |= 0x80;  // 设置段码最高位点亮冒号
        }
    }

    // 数据传输（自动地址模式）
    TM1637_I2CStart();
    TM1637_I2CWrByte(0x40);      // 自动地址增加
    TM1637_I2CAck();
    TM1637_I2CStop();

    TM1637_I2CStart();
    TM1637_I2CWrByte(0xC0);      // 起始地址
    TM1637_I2CAck();

    for (int i = 0; i < 4; i++) {
        TM1637_I2CWrByte(digits[i]);
        TM1637_I2CAck();
    }
    TM1637_I2CStop();

    // 开启显示
    TM1637_I2CStart();
    TM1637_I2CWrByte(0x8F);      // 亮度最高
    TM1637_I2CAck();
    TM1637_I2CStop();
}

// Scan key from TM1637
uint8_t TM1637_ScanKey(void)
{
    uint8_t rekey = 0, i;
    GPIO_InitTypeDef GPIO_InitStruct;

    TM1637_I2CStart();
    TM1637_I2CWrByte(0x42); // Read key command
    TM1637_I2CAck();

    // Set DIO as input
    GPIO_InitStruct.GPIO_Pin = TM1637_DIO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(TM1637_DIO_PORT, &GPIO_InitStruct);

    GPIO_SetBits(TM1637_DIO_PORT, TM1637_DIO_PIN);

    for (i = 0; i < 8; i++)
    {
        GPIO_ResetBits(TM1637_CLK_PORT, TM1637_CLK_PIN);
        rekey >>= 1;
        Delay_us(30);
        GPIO_SetBits(TM1637_CLK_PORT, TM1637_CLK_PIN);
        if (GPIO_ReadInputDataBit(TM1637_DIO_PORT, TM1637_DIO_PIN))
        {
            rekey |= 0x80;
        }
        else
        {
            rekey |= 0x00;
        }
        Delay_us(30);
    }

    // Restore DIO as output
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(TM1637_DIO_PORT, &GPIO_InitStruct);

    TM1637_I2CAck();
    TM1637_I2CStop();
    return rekey;
}


// Initialize TM1637
void TM1637_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // Enable GPIOB clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // Configure CLK and DIO pins as output
    GPIO_InitStruct.GPIO_Pin = TM1637_CLK_PIN | TM1637_DIO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(TM1637_DIO_PORT, &GPIO_InitStruct);

    // Initialize pins to high
    GPIO_SetBits(TM1637_CLK_PORT, TM1637_CLK_PIN);
    GPIO_SetBits(TM1637_DIO_PORT, TM1637_DIO_PIN);
}