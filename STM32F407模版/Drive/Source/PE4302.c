#include "PE4302.h"
/**************************************************************** 
* 函数功能：PE4302  I/O口初始化
* 入口参数：无
* 返回数值：无
* 其他： 
*****************************************************************/ 
void PE4302_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOC时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  // 修改点1：APB2→AHB1[1,7](@ref)

    //配置PC0~PC3引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;         // 模式设为通用输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         // 明确输出类型为推挽
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 速度保持50MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;      // 禁用上下拉
    
    
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/**************************************************************** 
* 函数功能：PE4302芯片一设置衰减值
* 入口参数：db（衰减值）: 范围：0~63，对应衰减值0~31.5dB
* 返回数值：无
* 其他： 
*****************************************************************/ 
void PE4302_0_Set(uint8_t db)
{
  uint8_t i;
  uint8_t W_DB;
  db = db&0X3F;
	
	PE_LE_1_DIS;
	Delay_us(500);
  PE_CLK_0;
	Delay_us(500);
  PE_LE_0_EN;
	Delay_us(500);
  for(i = 0; i < 6;i++)
  {
    W_DB = (db>>5);
    if(W_DB == 1)
    {
      PE_DAT_1;
    }
    else
    {
      PE_DAT_0;
    }
		Delay_us(500);
    PE_CLK_1;
		Delay_us(500);
    db = (db << 1)&0X3F;
    PE_CLK_0;
		Delay_us(500);
  }
  PE_LE_0_DIS;
	Delay_us(500);
	PE_DAT_0;
}
/**************************************************************** 
* 函数功能：PE4302芯片二设置衰减值
* 入口参数：db（衰减值）: 范围：0~63，对应衰减值0~31.5dB
* 返回数值：无
* 其他： 
*****************************************************************/ 
void PE4302_1_Set(uint8_t db)
{
  uint8_t i;
  uint8_t W_DB;
  db = db&0X3F;
	
	PE_LE_0_DIS;
	Delay_us(500);
  PE_CLK_0;
	Delay_us(500);
  PE_LE_1_EN;
	Delay_us(500);
  for(i = 0; i < 6;i++)
  {
    W_DB = (db>>5);
    if(W_DB == 1)
    {
      PE_DAT_1;
    }
    else
    {
      PE_DAT_0;
    }
		Delay_us(500);
    PE_CLK_1;
		Delay_us(500);
    db = (db << 1)&0X3F;
    PE_CLK_0;
		Delay_us(500);
  }
  PE_LE_1_DIS;
	Delay_us(500);
	PE_DAT_0;
}


/******************************************************************* 
* 函数功能：选择PE4302芯片设置衰减值
* 入口参数：db（衰减值）: 范围：0~31.5dB
					  position（选择要控制的芯片）：范围 0~1 选择芯片 
* 返回数值：无
* 其他：position：0：控制芯片U1(PE4302)衰减  1：控制芯片U2(PE4302)衰减
********************************************************************/ 
void PE4302_Position_Set(float db,uint8_t position)
{
	uint8_t temp=0;
	temp=(uint8_t)(db/0.5);
	if(position==0)
		PE4302_0_Set(temp);
	if(position==1)
		PE4302_1_Set(temp);
	
}
