#include "stm32f4xx.h"
#include "AD7608.h"


void AD7608_STARTCONV(void)
{
  AD7608_CONVST_A_L;
  AD7608_CONVST_B_L;
  Delay(0xF);
  AD7608_CONVST_A_H;
  AD7608_CONVST_B_H;
}

double ad7606_ReadDATA(void)
{
  uint32_t usData = 0;
	double adc_data=0.0;
  for (uint8_t i = 0; i < 18; i++)
  {
    AD7608_SCLK_L;
    usData = usData << 1;
    if(AD7608_DOUTA)
    {
      usData |= 0x0001;
    }
    AD7608_SCLK_H;
  }
  //return usData;		
	return usData*0.076509150571+1.0279873682576;
}

void AD7608_RESET(void)
{
  AD7608_RESET_H;
  Delay(0xFF);
  AD7608_RESET_L;
}

void AD7608_SETOS(uint8_t osv)
{
  switch(osv)
  {
  case 0://000
    AD7608OS0_L;
    AD7608OS1_L;
    AD7608OS2_L;
    break;
  case 1://001
    AD7608OS0_H;
    AD7608OS1_L;
    AD7608OS2_L;
    break;
  case 2://010
    AD7608OS0_L;
    AD7608OS1_H;
    AD7608OS2_L;
    break;
  case 3://011
    AD7608OS0_H;
    AD7608OS1_H;
    AD7608OS2_L;
    break;
  case 4://100
    AD7608OS0_L;
    AD7608OS1_L;
    AD7608OS2_H;
    break;
  case 5://101
    AD7608OS0_H;
    AD7608OS1_L;
    AD7608OS2_H;
    break;
  case 6://110
    AD7608OS0_L;
    AD7608OS1_H;
    AD7608OS2_H;
    break;
	case 7://111
    AD7608OS0_H;
    AD7608OS1_H;
    AD7608OS2_H;
    break;
  }
}

//时钟初始化
void RCC_Configuration(void)
{
    ErrorStatus HSEStartUpStatus;

    // 复位RCC寄存器到默认值
    RCC_DeInit();
    // 打开外部高速晶振
    RCC_HSEConfig(RCC_HSE_ON);

    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    if(HSEStartUpStatus == SUCCESS)
    {
        // 使能Prefetch Buffer、设置Flash等待周期
        FLASH_PrefetchBufferCmd(ENABLE);
        FLASH_SetLatency(FLASH_Latency_5); 

        // 配置AHB/APB时钟
        RCC_HCLKConfig(RCC_SYSCLK_Div1);   // AHB = SYSCLK / 1
        RCC_PCLK2Config(RCC_HCLK_Div2);     // APB2 = AHB / 2
        RCC_PCLK1Config(RCC_HCLK_Div4);     // APB1 = AHB / 4

        // 配置PLL,  HSE=8MHz, PLLN=336, PLLM=8, PLLP=2, PLLQ=7, SYSCLK=168MHz
        RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 7); // 8MHz/8=1MHz, 1*336=336MHz, 336/2=168MHz
        RCC_PLLCmd(ENABLE);

        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        while(RCC_GetSYSCLKSource() != 0x08);
    }

    // 打开外设时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); // DMA在AHB1
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA, ENABLE);
    //不用打开
}

//GPIO初始化
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // GPIOA 0,1,2,11,12 推挽输出 上拉
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // GPIOC 13,14,15 推挽输出 上拉
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // GPIOA 3,4,5 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void NVIC_Configuration(void) 
{ 
#ifdef  VECT_TAB_RAM   
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  
#else
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);    
#endif 
} 

void Delay(uint32_t nCount) 
{ 
  for(; nCount != 0; nCount--); 
} 

#ifdef  DEBUG 
void assert_failed(u8* file, u32 line) 
{  
  while (1) { } 
} 
#endif 


