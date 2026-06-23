#include "stm32f4xx.h"
#include "GPIO.h"
#include "DAT_31R5_SPpos.h"
#include "delay.h"

uint8_t dBs1[6] = {0,0,1,0,0,0};
uint8_t dBs2[6] = {0,0,0,0,0,0};
// 16 8 4 2 1 0.5

void DAT_31R5_SPpos_Init(void)
{
		Config_GPIO(GPIOD, GPIO_Pin_13, GPIO_MODE_OUTPUT); // LE
		Config_GPIO(GPIOD, GPIO_Pin_14, GPIO_MODE_OUTPUT); // CLK
		Config_GPIO(GPIOD, GPIO_Pin_15, GPIO_MODE_OUTPUT); // DAT
		
		Config_GPIO(GPIOD, GPIO_Pin_10, GPIO_MODE_OUTPUT); // LE2
		Config_GPIO(GPIOD, GPIO_Pin_11, GPIO_MODE_OUTPUT); // CLK2
		Config_GPIO(GPIOD, GPIO_Pin_12, GPIO_MODE_OUTPUT); // DAT2
	
		LE_0;
		CLK_0;
		DAT_0;
	
		Delay_us(2);
	
		LE2_0;
		CLK2_0;
		DAT2_0;
	
		Delay_us(2);
}

void DAT_31R5_SPpos_SendReg(void)
{
		for(uint8_t i = 0; i < 6; i++)
		{
				if(dBs1[i])
				{
						DAT_1;
						Delay_us(2);
						CLK_1;
						Delay_us(2);
						CLK_0;
				}
				else
				{
						DAT_0;
						Delay_us(2);
						CLK_1;
						Delay_us(2);
						CLK_0;
				}
		}
		Delay_us(2);
}

void DAT_31R5_SPpos_SendReg2(void)
{
		for(uint8_t i = 0; i < 6; i++)
		{
				if(dBs2[i])
				{
						DAT2_1;
						Delay_us(2);
						CLK2_1;
						Delay_us(2);
						CLK2_0;
				}
				else
				{
						DAT2_0;
						Delay_us(2);
						CLK2_1;
						Delay_us(2);
						CLK2_0;
				}
		}
		Delay_us(2);
}

void DAT_31R5_SPpos_WriteReg(void)
{
		LE_1;
		Delay_us(2);
		LE_0;
}

void DAT_31R5_SPpos_WriteReg2(void)
{
		LE2_1;
		Delay_us(2);
		LE2_0;
}


void DAT_31R5_SPpos_Start(void)
{

    DAT_31R5_SPpos_Init();
    DAT_31R5_SPpos_SendReg();
    DAT_31R5_SPpos_WriteReg();
    DAT_31R5_SPpos_SendReg2();
    DAT_31R5_SPpos_WriteReg2();
}
