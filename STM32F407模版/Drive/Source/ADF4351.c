#include "ADF4351.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "Delay.h"
// Reference divider value (R) for 25 MHz input frequency, R = 125
// Formula: Input frequency / (2 * R) = 0.1 MHz
uint32_t R = 125; // R ranges from 0 to 1023, default for 25 MHz crystal

//-----------------------------------------------------------------
// Function: GPIO_AD4351_Init
// Description: Initializes GPIO pins for ADF4351
// Parameters: None
// Returns: None
// Notes: Configures GPIOD pins 8, 10, 12, 14 as push-pull outputs
//-----------------------------------------------------------------
void GPIO_AD4351_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable GPIOD clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    // Configure pins PD8 (CLK), PD10 (DATA), PD12 (LE), PD14 (CE)
    GPIO_InitStructure.GPIO_Pin = ADF4351_CLK_PIN | ADF4351_DATA_PIN | ADF4351_LE_PIN | ADF4351_CE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; // Push-pull output
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // Explicitly push-pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // 2 MHz speed
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // No pull-up/pull-down
    GPIO_Init(ADF4351_PORT, &GPIO_InitStructure);
	
	// Configure input pin: PD9 (LD)
    GPIO_InitStructure.GPIO_Pin = ADF4351_LD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; // Input mode
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // No pull-up/pull-down (adjust if needed)
    GPIO_Init(ADF4351_PORT, &GPIO_InitStructure);
	
}

//-----------------------------------------------------------------
// Function: ADF4351_Init
// Description: Initializes ADF4351 with default register settings
// Parameters: date - Initial N divider value for Register 0
// Returns: None
// Notes: Configures registers 0-5 for ADF4351
//-----------------------------------------------------------------
void ADF4351_Init(uint32_t date)
{
	  GPIO_AD4351_Init();
    ADF_CE_Set(); // Enable chip
    Delay_us(5);  // Small Delay for stabilization (ensure Delay_us is defined)
    ADF_CLK_Clr();
    ADF_LE_Set();
    ADF_DATA_Clr();

    // Configure ADF4351 registers
    ADF4351_Wdata(0x00580005); // Register 5: Digital lock detect mode
    ADF4351_Wdata(0x0060A43C); // Register 4: 5 dBm output, mute till lock, 10 kHz band select clock
    ADF4351_Wdata(0x006004B3); // Register 3: Charge cancellation, ABP=1
    ADF4351_Wdata(0x0D003FC2 | (R << 14)); // Register 2: 5 mA charge pump, LDP=1, LDF=1
    ADF4351_Wdata(0x08008011); // Register 1: Prescaler 8/9
    ADF4351_Wdata(0x00000000 | (date << 15)); // Register 0: N divider
}

//-----------------------------------------------------------------
// Function: ADF4351_Wdata
// Description: Writes 32-bit data to ADF4351
// Parameters: dat - 32-bit data to write
// Returns: None
// Notes: Serial data transfer using CLK, DATA, and LE pins
//-----------------------------------------------------------------
void ADF4351_Wdata(uint32_t dat)
{
    uint8_t i;
    ADF_CLK_Clr();
    ADF_LE_Clr();

    // Write 32 bits, MSB first
    for (i = 0; i < 32; i++)
    {
        if (dat & 0x80000000)
            ADF_DATA_Set();
        else
            ADF_DATA_Clr();
        dat <<= 1;
        ADF_CLK_Set();
        ADF_CLK_Clr();
    }
    ADF_LE_Set(); // Latch data
}

//-----------------------------------------------------------------
// Function: ADF4351_WriteFre
// Description: Sets ADF4351 output frequency
// Parameters: F - Frequency in MHz * 10 (e.g., 1000 for 100.0 MHz)
// Returns: None
// Notes: Configures RF divider based on frequency range and sets Register 0
//-----------------------------------------------------------------
void ADF4351_WriteFre(double Fre)
{
	
  uint32_t  F = (uint32_t)(Fre * 10); // Convert to 0.1 MHz units

    // Set Register 4 (RF Divider) based on frequency range
    if (F >= 690 && F <= 1370)
        ADF4351_Wdata(0x0050443C); // RF Divider = 8
    else if (F > 1370 && F <= 2740)
        ADF4351_Wdata(0x0040443C); // RF Divider = 4
    else if (F > 2740 && F <= 5490)
        ADF4351_Wdata(0x0030143C); // RF Divider =  spezz2
    else if (F > 5490 && F <= 10990)
        ADF4351_Wdata(0x0020143C); // RF Divider = 1
    else if (F > 10990 && F <= 21990)
        ADF4351_Wdata(0x0010143C); // RF Divider = 1/2
    else if (F > 21990)
        ADF4351_Wdata(0x0000143C); // RF Divider = 1/4
    else
        ADF4351_Wdata(0x0060443C); // RF Divider = 16 (default)

    // Set Register 0 (Frequency word)
    ADF4351_Wdata(0x00000000 | (F << 15));
}

uint8_t ADF4351_ReadLD(void)
{
    return GPIO_ReadInputDataBit(ADF4351_PORT, ADF4351_LD_PIN);
}

