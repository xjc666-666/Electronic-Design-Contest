#ifndef _AD5933_H_
#define _AD5933_H_

#include "stm32f4xx.h"
#include "delay.h"
#include "GPIO.h"

#define uint unsigned int

#define SDA_IN PDin(6)
#define SDA_OUT PDout(6)
#define SDA_1 Set_GPIO_High(GPIOD, GPIO_Pin_6)
#define SDA_0 Set_GPIO_Low(GPIOD, GPIO_Pin_6)
#define READ_SDA GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6)

#define SCL_OUT PDout(7) 
#define SCL_1 Set_GPIO_High(GPIOD, GPIO_Pin_7)
#define SCL_0 Set_GPIO_Low(GPIOD, GPIO_Pin_7)

#define AD5933_SYS_Init			0x1000   // 系统初始化
#define AD5933_Begin_Fre_Scan	0x2000   // 扫频初始化
#define AD5933_Fre_UP			0x3000   // 跳到下一个频点
#define AD5933_Fre_Rep			0x4000   // 重复当前频率

#define AD5933_Get_Temp			0x9000   // 获取温度
#define AD5933_Sleep			0xA000   // 进入睡眠模式
#define AD5933_Standby			0xB000   // 进入待机模式

#define AD5933_OUTPUT_2V		0x0000   // 输出2V峰峰值
#define AD5933_OUTPUT_1V		0x0600   // 输出1V峰峰值
#define AD5933_OUTPUT_400mV		0x0400   // 输出400mV峰峰值
#define AD5933_OUTPUT_200mV		0x0200   // 输出200mV峰峰值

#define AD5933_Gain_1			0x0100   // 可编程增益放大器增益1倍
#define AD5933_Gain_5			0x0000   // 可编程增益放大器增益5倍

#define AD5933_IN_MCLK			0x0000   // 内部时钟
#define AD5933_OUT_MCLK			0x0004   // 外部时钟

#define AD5933_Reset			0x0010   // 复位

#define AD5933 1                // AD5933的I2C地址
#define AD5933_MCLK 16.776      // 536870912/MCLK;

#define Fre_20K_Rr_100K 9822 * 100000
#define Fre_20K_Rr_1K 9345 * 1000


extern long realArr[3],imageArr[3];

void I2C_GPIO_INIT(void);
void GetAck(void);
void SendNACK(void);
void START(void);
void STOP(void);
void SendByte(uint8_t txd);	// 发送一个字节数据子函数 
uint8_t ReadByte(void);  //读一个字节数据
void WriteByte(char nAddr,uint nValue);

void SetPointer(char nAddr);
int Rece_Byte(char nAddr);
uint16_t AD5933_Tempter(void);
float Scale_imp (uint8_t *SValue,uint8_t *IValue,uint8_t *NValue,uint8_t *CValue);
float Get_resistance(u16 num);
float AD5933_Sweep (float Fre_Begin,float Fre_UP,uint16_t UP_Num,uint16_t OUTPUT_Vatage,uint16_t Gain,uint16_t SWeep_Rep);
float DA5933_Get_Cap(void);
float DA5933_Get_L(void);
float DA5933_Get_Rs(void);

void GET_REG(void);

#endif

