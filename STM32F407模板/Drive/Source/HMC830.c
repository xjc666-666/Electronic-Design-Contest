/**********************************************************
                       HMC830 Driver for STM32F407
**********************************************************/
#include "HMC830.h"
#include "delay.h" // Ensure you have a delay library for STM32F4xx

// Global variables
uint8_t mode = 0, lock = 0;
uint8_t dat_reg[4] = {0};
uint8_t rfDividReg[3] = {0x00, 0xE0, 0x90};
uint32_t REG0_830  = 0x400A7975; // Chip ID
uint32_t REGF0_830 = 0x0F0000C0;
uint32_t REGF1_830 = 0x0F0000E0;
uint32_t REG12_830 = 0x52000000;

/**
 * @brief  Configures GPIO pins for HMC830 communication
 * @param  None
 * @retval None
 */
void Hmc830Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable GPIOC clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // Initialize output pins: CEN, SEN, SDI, SCK
    GPIO_InitStructure.GPIO_Pin = HMC830_CEN | HMC830_SEN | HMC830_SDI | HMC830_SCK;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Initialize input pin: LD
    GPIO_InitStructure.GPIO_Pin = HMC830_LD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Set initial states
    HMC830_SCK_SET;
    HMC830_SEN_RESET;
    HMC830_CEN_SET;

    Delay_ms(20);
}

/**
 * @brief  SPI write function for HMC830
 * @param  addr: Register address
 * @param  data: Pointer to 3-byte data array
 * @retval None
 */
void Hmc830SpiWrite(uint8_t addr, uint8_t *data)
{
    uint8_t i, j;
    uint8_t dat_reg;

    HMC830_SEN_SET;
    addr = addr << 3; // Shift address to align with protocol

    // Write 3 bytes of data
    for (j = 0; j < 3; j++)
    {
        dat_reg = data[j];
        for (i = 0; i < 8; i++)
        {
            if (dat_reg & 0x80)
                HMC830_SDI_SET;
            else
                HMC830_SDI_RESET;
            dat_reg <<= 1;
            Delay_us(50);
            HMC830_SCK_RESET;
            Delay_us(50);
            HMC830_SCK_SET;
            Delay_us(50);
            HMC830_SCK_RESET;
            Delay_us(50);
        }
    }

    // Write address bits
    for (i = 0; i < 5; i++)
    {
        if (addr & 0x80)
            HMC830_SDI_SET;
        else
            HMC830_SDI_RESET;
        addr <<= 1;
        HMC830_SCK_RESET;
        Delay_us(50);
        HMC830_SCK_SET;
        Delay_us(50);
        HMC830_SCK_RESET;
        Delay_us(50);
    }

    // Write remaining bits (3 dummy bits)
    for (i = 0; i < 3; i++)
    {
        HMC830_SDI_RESET;
        HMC830_SCK_RESET;
        Delay_us(50);
        HMC830_SCK_SET;
        Delay_us(50);
        HMC830_SCK_RESET;
        Delay_us(50);
    }

    HMC830_SEN_RESET;
    Delay_us(50);
    HMC830_SEN_SET;
    Delay_us(50);
    HMC830_SEN_RESET;
    Delay_us(50);
}

/**
 * @brief  Initializes HMC830 with default register settings
 * @param  data: Pointer to 4-byte data array for frequency settings
 * @retval None
 */
void Hmc830Init(uint8_t *data)
{
    uint8_t dat_reg[3] = {0};
    uint8_t dat0 = data[0];
    uint8_t dat1 = data[1];
    uint8_t dat2 = data[2];
    uint8_t dat3 = data[3];

    // Register 0x00
    dat_reg[0] = 0x00;
    dat_reg[1] = 0x00;
    dat_reg[2] = 0x20;
    Hmc830SpiWrite(0x00, dat_reg);

    // Register 0x01
    dat_reg[0] = 0x00;
    dat_reg[1] = 0x00;
    dat_reg[2] = 0x02;
    Hmc830SpiWrite(0x01, dat_reg);

    // Register 0x02 (Reference divider, e.g., 50MHz phase detector)
    dat_reg[0] = 0x00;
    dat_reg[1] = 0x00;
    dat_reg[2] = 0x02; // 50MHz
    Hmc830SpiWrite(0x02, dat_reg);

    // Register 0x05 (VCO divider)
    dat_reg[0] = rfDividReg[0];
    dat_reg[1] = rfDividReg[1];
    dat_reg[2] = rfDividReg[2];
    Hmc830SpiWrite(0x05, dat_reg);

    // Register 0x05 (VCO settings)
    dat_reg[0] = 0x00;
    dat_reg[1] = 0x28;
    dat_reg[2] = 0x98;
    Hmc830SpiWrite(0x05, dat_reg);

    // Register 0x05 (VCO reset)
    dat_reg[0] = 0x00;
    dat_reg[1] = 0x00;
    dat_reg[2] = 0x00;
    Hmc830SpiWrite(0x05, dat_reg);

    // Register 0x06 (Mode settings)
    dat_reg[0] = 0x20;
    dat_reg[1] = 0x0B;
    dat_reg[2] = 0x4A; // Fractional mode
    Hmc830SpiWrite(0x06, dat_reg);

    // Register 0x07
    dat_reg[0] = 0x00;
    dat_reg[1] = 0x0C;
    dat_reg[2] = 0xCD;
    Hmc830SpiWrite(0x07, dat_reg);

    // Register 0x09 (Charge pump settings)
    dat_reg[0] = 0x54;
    dat_reg[1] = 0x3F;
    dat_reg[2] = 0xFF;
    Hmc830SpiWrite(0x09, dat_reg);

    // Register 0x0A
    dat_reg[0] = 0x00;
    dat_reg[1] = 0x20;
    dat_reg[2] = 0x46;
    Hmc830SpiWrite(0x0A, dat_reg);

    // Register 0x0F
    dat_reg[0] = 0x00;
    dat_reg[1] = 0x00;
    dat_reg[2] = 0x81;
    Hmc830SpiWrite(0x0F, dat_reg);

    // Register 0x03 (Integer part)
    dat_reg[0] = 0x00;
    dat_reg[1] = 0x00;
    dat_reg[2] = dat0;
    Hmc830SpiWrite(0x03, dat_reg);

    // Register 0x04 (Fractional part)
    dat_reg[0] = dat3;
    dat_reg[1] = dat2;
    dat_reg[2] = dat1;
    Hmc830SpiWrite(0x04, dat_reg);
}

/**
 * @brief  Configures VCO frequency
 * @param  fre: VCO frequency in kHz (1500000 to 3000000)
 * @retval None
 */
void Hmc830WriteFrequency(uint32_t fre)
{
    uint32_t fsysclk = 50000000; // Phase detector frequency: 50MHz
    uint32_t data_24 = 0xFFFFFF;
    uint32_t fdout, fdout1;
    uint32_t res0, res1, sum, sum0, k;
    uint8_t i, j;

    Hmc830Config();
    Delay_ms(50);

    fdout = fre * 1000; // Convert to Hz
    fdout1 = fdout;    // No divider adjustment here

    dat_reg[0] = fdout1 / fsysclk; // Integer part
    fdout = fdout1 % fsysclk;      // Fractional part

    sum = data_24 / fsysclk;
    res0 = data_24 % fsysclk;
    sum = fdout * sum;

    for (i = 0; i < 7; i++)
    {
        j = i + 1;
        k = 1;
        while (j > 0)
        {
            k *= 10;
            j--;
        }
        res1 = res0 % (fsysclk / k);
        res0 = res0 / (fsysclk / k);
        sum0 = (res0 * fdout) / k;
        res0 = res1;
        if ((res0 > 4) && (res0 < 10))
            sum += 1;
        sum += sum0;
    }

    dat_reg[1] = sum;
    dat_reg[2] = sum >> 8;
    dat_reg[3] = sum >> 16;

    Hmc830Init(dat_reg);
    Delay_ms(50);
}

/**
 * @brief  Reads HMC830 chip ID
 * @param  reg: Expected chip ID (e.g., 0x400A7975)
 * @retval None
 */
void Hmc830ReadId(uint32_t reg)
{
    uint8_t i;
    uint32_t pll_reg = reg;

    HMC830_SEN_RESET;
    Delay_ms(10);
    HMC830_SEN_SET;
    Delay_ms(10);

    // Send address bits
    for (i = 0; i < 7; i++)
    {
        pll_reg <<= 1;
        HMC830_SCK_RESET;
        if (pll_reg & 0x80000000)
            HMC830_SDI_SET;
        else
            HMC830_SDI_RESET;
        Delay_us(5);
        HMC830_SCK_SET;
        Delay_us(5);
    }

    HMC830_SCK_RESET;
    HMC830_SDI_RESET;
    Delay_us(5);

    // Read 24-bit data
    pll_reg = 0x00000000;
    for (i = 0; i < 24; i++)
    {
        pll_reg <<= 1;
        HMC830_SCK_SET;
        Delay_us(5);
        if (HMC830_LD_READ)
            pll_reg |= 0x00000001;
        else
            pll_reg &= ~0x00000001;
        HMC830_SCK_RESET;
        Delay_us(5);
    }

    HMC830_SCK_SET;
    Delay_us(5);
    HMC830_SCK_RESET;
    Delay_ms(10);
    HMC830_SEN_RESET;
    Delay_ms(20);

    if (reg == REG0_830)
    {
        if (pll_reg == 0x000A7975)
        {
            printf("ID_D:%d\t", pll_reg);
            mode = 0xEF; // HMC mode
        }
    }
    else
    {
        lock = (pll_reg & 0x00000002) ? 0x01 : 0x00;
			  printf("no");
    }

#ifdef DEBUG
    printf("ID:%X\r\n", pll_reg);
#endif
}

/**
 * @brief  Sets output frequency (25MHz to 3GHz)
 * @param  wfre: Output frequency in kHz
 * @retval None
 */
void Hmc830SetFrequency(uint64_t wfre)
{
    uint16_t vco02reg = 0x6010;
    uint16_t rfdivid = 1;
    uint8_t noise_con = 0;
    uint32_t vco_fre;

    if (wfre > 3000000)
        wfre = 3000000;
    if (wfre < 25000)
        wfre = 25000;

    if (wfre > 1500000)
    {
        rfdivid = 1;
    }
    else
    {
        rfdivid = 3000000 / wfre;
        if (rfdivid < 2)
        {
            rfdivid = 1;
            noise_con = 1;
        }
        else if (rfdivid < 3)
        {
            rfdivid = 2;
            noise_con = 1;
        }
        else if (rfdivid > 62)
        {
            rfdivid = 62;
        }
        else
        {
            rfdivid = (rfdivid % 2 == 0) ? rfdivid : (rfdivid - 1);
        }
    }

    noise_con = (rfdivid < 3) ? 1 : 0;
    if (noise_con)
        vco02reg |= (1 << 15);
    else
        vco02reg &= ~(1 << 15);
    vco02reg |= (rfdivid << 7);

    vco_fre = wfre * rfdivid;

    rfDividReg[1] = vco02reg >> 8;
    rfDividReg[2] = vco02reg & 0xFF;

    Hmc830WriteFrequency(vco_fre);
}