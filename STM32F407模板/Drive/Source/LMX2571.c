#include "LMX2571.h"
#include "math.h"
#include "stdint.h"
#include "delay.h"

typedef struct LMX2571_PLL
{
	u8 FSK_EN;
	u8 OUTBUF_TX_PWR;
	u8 OUTBUF_RX_PWR;
	u8 OUTBUF_TX_EN;
	u8 OUTBUF_RX_EN;
	u8 LF_R4;
	u8 LF_R3;
	u8 CHDIV2;
	u8 CHDIV1;
	u8 PFD_DELAY;
	u8 MULT;
	u8 PLL_R;
	u8 PLL_R_PRE;
	u8 PLL_N_PRE;
	u8 FRAC_ORDER;
	u16 PLL_N;
	u32 PLL_NUM;
	u32 PLL_DEN;
	u8 EXTVCO_SEL;
} _LMX2571_PLL;

struct LMX2571_FUNC
{

	u8 XTAL_EN;
	u8 RESET;
	u8 POWERDOWN;
	u8 DITHERING;
	u8 CP_IDN_IDP;
	u8 CP_GAIN;
	u8 SDO_LD_SEL;
	u8 LD_EN;
	u16 MULT_WAIT;
	u8 OUTBUF_AUTOMUTE;
	u8 OUTBUF_TX_TYPE;
	u8 OUTBUF_RX_TYPE;
	u8 IPBUFDIFF_TERM;
	u8 IPBUF_SE_DIFF_SEL;
	u8 XTAL_PWRCTRL;
	u8 FSK_I2S_FS_POL;
	u8 FSK_I2S_CLK_POL;
	u8 FSK_LEVEL;
	u8 FSK_DEV_SEL;
	u8 FSK_MODE_SEL;
	u8 RXTX_CTRL;
	u8 RXTX_POL;
	u8 F1F2_INIT;
	u8 F1F2_CTRL;
	u8 F1F2_MODE;
	u8 F1F2_SEL;
	u8 FCAL_EN;
} LMX2571_FUNC;

_LMX2571_PLL F[2];
/*
*DITHERING:Set the level of dithering.
0 = Disabled
1 = Weak
2 = Medium
3 = Strong
*CP_IDN_IDP:Set the base charge pump current for internal VCO in synthesizer mode operation.
0 = Tri-state
1 = 156.25  * 2�A
2 = 312.5  * 2�A
...
31 = 3593.75 * 2 �A
*CP_GAIN:Set the multiplication factor to the base charge pump current for internal VCO in synthesizer mode operation
For example, if the
gain here is 2x and if the total base charge pump current CP_IDN_IDP is 2.5 mA, then the final charge pump
current applied to the loop filter is 5 mA.
0 = 1x
1 = 2x
2 = 1.5x
3 = 2.5x
*SDO_LD_SEL:Defines the MUXout pin function.
0 = Register readback serial data output
1 = Lock detect output
*LD_EN:Enables lock detect function.
0 = Disabled
1 = Enabled
*MULT_WAIT:A 20-�s settling time is required for MULT, if it is enabled.
0 = Do not use this setting
1 = 1 OSCin clock cycle
...
2047 = 2047 OSCin clock cycles
*OUTBUF_AUTOMUTE:If this bit is set, the output buffers will be muted until PLL is locked.
0 = Disabled
1 = Enabled
*OUTBUF_TX_TYPE:Sets the output buffer type of RFoutTx.
0 = Open drain
1 = Push pull
*OUTBUF_RX_TYPE:Sets the output buffer type of RFoutRx.
0 = Open drain
1 = Push pull
*IPBUFDIFF_TERM:Enables independent 50 O input termination on both OSCin and OSCin* pins.
0 = Disabled
1 = Enabled
*IPBUF_SE_DIFF_SEL:Selects between single-ended and differential OSCin input.
0 = Single-ended input
1 = Differential input
*XTAL_PWRCTRL:Set the value of the series resistor being used to limit the power dissipation through the crystal when crystal is being used as OSCin input.
0 = 0 O
1 = 100 O
2 = 200 O
3 = 300 O
4-7 = Reserved
*XTAL_EN:Enables the crystal oscillator buffer for use as OSCin input. This bit will overwrite IPBUF_SE_DIFF_SEL.
0 = Disabled
1 = Enabled
*FSK_I2S_FS_POL:Sets the polarity of the I2S Frame Sync input in FSK I2S mode.
0 = Active HIGH
1 = Active LOW
*FSK_I2S_CLK_POL:FSK_I2S_CLK_POL
Sets the polarity of the I2S CLK input in FSK I2S mode.
0 = Rising edge strobe
1 = Falling edge strobe
*FSK_LEVEL:Define the desired FSK level in FSK PIN mode and FSK SPI mode.
0 = Disabled
1 = 2FSK
2 = 4FSK
3 = 8FSK
*FSK_DEV_SEL:
In FSK SPI mode, these bits select one of the FSK deviations as
defined in registers R25-32 or R9-16.
0 = FSK_DEV0_Fx
1 = FSK_DEV1_Fx
...
7 = FSK_DEV7_Fx
*FSK_MODE_SEL:FSK_MODE_SEL0 and FSK_MODE_SEL1 define the FSK
operation mode. FSK_MODE_SEL[1:0] =
00 = FSK PIN mode
01 = FSK SPI mode
10 = FSK I2S mode
11 = FSK SPI FAST mode
*RXTX_CTRL:Sets the control mode of TX/RX switching.
0 = Switching is controlled by register programming
1 = Switching is controlled by toggling the TrCtl pin
*RXTX_POL:Defines the polarity of the TrCtl pin.
0 = Active LOW = TX
1 = Active HIGH = TX
*F1F2_INIT:Toggling this bit re-calibrates F1F2 if F1, F2 are modified after calibration
0 = Clear bit value
1 = Re-calibrate
*F1F2_CTRL:Sets the control mode of F1/F2 switching. Switching by TrCtl pin requires F1F2_MODE = 1.
0 = Switching is controlled by register programming
1 = Switching is controlled by toggling the TrCtl pin
*F1F2_MODE:Calibrates F1 and F2 during device initialization (initial power on programming).
*F1F2_SEL:Selects F1 or F2 configuration registers.
0 = F1 registers
1 = F2 registers
*FCAL_EN:Activates all kinds of calibrations, suggest keep it enabled all the time.
0 = Disabled
1 = Enabled
*/

u32 LMX2571regbuf[61] =
	{
		0x000083, 0x010000, 0x020000, 0x030000, 0x042070, 0x050101, 0x069B81, 0x070EC4, 0x08000E, 0x090000, 0x0A0000, 0x0B0000,
		0x0C0000, 0x0D0000, 0x0E0000, 0x0F0000, 0x100000, 0x110000, 0x120000, 0x132710, 0x142070, 0x150101, 0x169B81, 0x170EC4,
		0x18000E, 0x190000, 0x1A0000, 0x1B0000, 0x1C0000, 0x1D0000, 0x1E0000, 0x1F0000, 0x200000, 0x210000, 0x221000, 0x230C83,
		0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2711FB, 0x28101C, 0x290810, 0x2A0210, 0x2B4830, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x2F4000,
		0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x3A8C00, 0xFFFFFF, 0x3CA000

};
u32 LMX2571reg[61];
u64 nowfre;

void LMX2571_update_data(u8 write);
void WriteToLMX2571(u32 buf);

struct LMX2571_platform_data
{
	unsigned char clkin;
	unsigned char channel_spacing;
	unsigned char power_up_frequency;

	unsigned char ref_div_factor; /* 10-bit R counter */
	unsigned char ref_doubler_en;
	unsigned char ref_div2_en;

	unsigned char r2_user_settings;
	unsigned char r3_user_settings;
	unsigned char r4_user_settings;
	int gpio_lock_detect;
};

void SetOutFre(u8 f, u64 fre)
{
	_PLL_state val;

	//	fre+=1000;
	if (PLL_Calulate(fre) == 0)
	{
		nowfre = fre;
		val = GetPLLNum();

		F[f].CHDIV2 = val.PLL.CHDIV2;
		F[f].CHDIV1 = val.PLL.CHDIV1 - 4;
		F[f].MULT = val.PLL.MULT;
		F[f].PLL_R = val.PLL.POST_R_Divier;
		F[f].PLL_R_PRE = val.PLL.PRE_R_Divier;
		F[f].PLL_N_PRE = 1 - val.PLL.Prescalar / 2;
		F[f].PLL_N = val.PLL.N_Divier;
		F[f].PLL_NUM = val.PLL.NUM_Divier;
		F[f].PLL_DEN = val.PLL.DEN_Divier;
		if (F[f].PLL_DEN == 1)
			F[f].PLL_DEN = 0;

		LMX2571regbuf[1 + (f == LMX2571_F1 ? 0 : 16)] = LMX2571_PLL_DEN_F1_23_16(F[f].PLL_DEN >> 16) | LMX2571_PLL_NUM_F1_23_16(F[f].PLL_NUM >> 16);
		LMX2571regbuf[2 + (f == LMX2571_F1 ? 0 : 16)] = (u16)F[f].PLL_NUM;
		LMX2571regbuf[3 + (f == LMX2571_F1 ? 0 : 16)] = (u16)F[f].PLL_DEN;
		LMX2571regbuf[4 + (f == LMX2571_F1 ? 0 : 16)] = LMX2571_PLL_N_F1(F[f].PLL_N) | LMX2571_PLL_N_PRE_F1(F[f].PLL_N_PRE) | (LMX2571_FRAC_ORDER_F1(F[f].FRAC_ORDER));
		LMX2571regbuf[5 + (f == LMX2571_F1 ? 0 : 16)] = LMX2571_PLL_R_PRE_F1(F[f].PLL_R_PRE) | LMX2571_PLL_R_F1(F[f].PLL_R);
		LMX2571regbuf[6 + (f == LMX2571_F1 ? 0 : 16)] = LMX2571_MULT_F1(F[f].MULT) | LMX2571_PFD_DELAY_F1(F[f].PFD_DELAY) | LMX2571_CHDIV1_F1(F[f].CHDIV1) | LMX2571_CHDIV2_F1(F[f].CHDIV2) | LMX2571_LF_R3_F1(F[f].LF_R3);
		LMX2571regbuf[7 + (f == LMX2571_F1 ? 0 : 16)] = LMX2571_LF_R4_F1(F[f].LF_R4) | LMX2571_OUTBUF_RX_EN_F1(F[f].OUTBUF_RX_EN) | LMX2571_OUTBUF_TX_EN_F1(F[f].OUTBUF_TX_EN) | LMX2571_OUTBUF_RX_PWR_F1(F[f].OUTBUF_RX_PWR);
		LMX2571regbuf[8 + (f == LMX2571_F1 ? 0 : 16)] = LMX2571_OUTBUF_TX_PWR_F1(F[f].OUTBUF_TX_PWR) | LMX2571_EXTVCO_SEL_F1(F[f].EXTVCO_SEL) | LMX2571_EXTVCO_CHDIV_F1(0) | LMX2571_FSK_EN_F1(F[f].FSK_EN);
		LMX2571_update_data(1);
	}
	WriteToLMX2571(LMX2571regbuf[0]);
}

void WriteToLMX2571(u32 buf)
{
	u8 i = 0;

	LMX2571_LE_0;
	Delay_us(1);
	LMX2571_CLK_0;

	for (i = 24; i > 0; i--)
	{
		if (0x800000 == (buf & 0x800000))
		{
			LMX2571_DATA_1;
		}
		else
		{
			LMX2571_DATA_0;
		}
		LMX2571_CLK_1;
		buf <<= 1;
		LMX2571_CLK_0;
	}
	LMX2571_DATA_0;
	LMX2571_LE_0;
}
u16 LMX2571_CaluateDetraFre(long int detraFre)
{
	long double sfsd;
	long int dF = detraFre;
	_PLL_state val;
	u16 ruturnvalue;
	val = GetPLLNum();
	if (detraFre < 0)
		dF = -dF;

	sfsd = ((long double)dF * 16777216 / val.PLL.nowfpd) * ((long double)(1 << val.PLL.CHDIV2) * val.PLL.CHDIV1 / val.PLL.Prescalar);

	if (sfsd > 65535)
		return 0;

	if (sfsd * 10 > ((long int)sfsd) * 10 + 5)
	{
		ruturnvalue = (u16)sfsd + 1;
	}
	else
		ruturnvalue = (u16)sfsd;

	if (detraFre < 0)
	{
		return (u16)(-ruturnvalue - 1);
	}
	else
	{
		return (u16)ruturnvalue;
	}
}

void LMX2571_FASTFSKWrite(u16 data)
{
	WriteToLMX2571((u32)data | 0x210000);
}
void LMX2571_FASTFSK(long int detraFre)
{
	u16 cdata;

	cdata = LMX2571_CaluateDetraFre(detraFre);
	LMX2571_FASTFSKWrite(cdata);
}
void LMX2571_Reset(void)
{
	u8 i = 0;

	LMX2571_FUNC.RESET = LMX2571_Enabled;
	LMX2571_update_data(0);
	WriteToLMX2571(LMX2571regbuf[0]);
	LMX2571_FUNC.RESET = LMX2571_Disabled;

	for (i = 60; i < 250; i--)
		LMX2571reg[i] = 0;
	Delay_ms(2);
}

void LMX2571_default_pdata(void)
{

	LMX2571_Reset();
	LMX2571_update_data(1);
}
void LMX2571_update_data(u8 write)
{
	u8 i = 0;

	LMX2571regbuf[47] = LMX2571_DITHERING(LMX2571_FUNC.DITHERING);
	LMX2571regbuf[41] = (LMX2571regbuf[41] & 0xFFF0) | LMX2571_CP_IDN(LMX2571_FUNC.CP_IDN_IDP);
	LMX2571regbuf[40] = 0x1C | LMX2571_CP_IUP(LMX2571_FUNC.CP_IDN_IDP) | LMX2571_CP_GAIN(LMX2571_FUNC.CP_GAIN);
	LMX2571regbuf[39] = 0x11F2 | LMX2571_SDO_LD_SEL(LMX2571_FUNC.SDO_LD_SEL) | LMX2571_LD_EN(LMX2571_FUNC.LD_EN);
	LMX2571regbuf[35] = LMX2571_MULT_WAIT(LMX2571_FUNC.MULT_WAIT) | LMX2571_OUTBUF_AUTOMUTE(LMX2571_FUNC.OUTBUF_AUTOMUTE) | LMX2571_OUTBUF_TX_TYPE(LMX2571_FUNC.OUTBUF_TX_TYPE) | LMX2571_OUTBUF_RX_TYPE(LMX2571_FUNC.OUTBUF_RX_TYPE);
	LMX2571regbuf[34] = LMX2571_IPBUFDIFF_TERM(LMX2571_FUNC.IPBUFDIFF_TERM) | LMX2571_IPBUF_SE_DIFF_SEL(LMX2571_FUNC.IPBUF_SE_DIFF_SEL) | LMX2571_XTAL_PWRCTRL(LMX2571_FUNC.XTAL_PWRCTRL) | LMX2571_XTAL_EN(LMX2571_FUNC.XTAL_EN) |
						LMX2571_FSK_I2S_FS_POL(LMX2571_FUNC.FSK_I2S_FS_POL) | LMX2571_FSK_I2S_CLK_POL(LMX2571_FUNC.FSK_I2S_CLK_POL) | LMX2571_FSK_LEVEL(LMX2571_FUNC.FSK_LEVEL) | LMX2571_FSK_DEV_SEL(LMX2571_FUNC.FSK_DEV_SEL) | LMX2571_FSK_MODE_SEL(LMX2571_FUNC.FSK_MODE_SEL);
	LMX2571regbuf[0] = 0x0002 | LMX2571_RESET(LMX2571_FUNC.RESET) | LMX2571_POWERDOWN(LMX2571_FUNC.POWERDOWN) | LMX2571_RXTX_CTRL(LMX2571_FUNC.RXTX_CTRL) | LMX2571_RXTX_POL(LMX2571_FUNC.RXTX_POL) |
					   LMX2571_F1F2_INIT(LMX2571_FUNC.F1F2_INIT) | LMX2571_F1F2_CTRL(LMX2571_FUNC.F1F2_CTRL) | LMX2571_F1F2_MODE(LMX2571_FUNC.F1F2_MODE) | LMX2571_F1F2_SEL(LMX2571_FUNC.F1F2_SEL) | LMX2571_FCAL_EN(LMX2571_FUNC.FCAL_EN);

	if (write)
	{
		for (i = 60; i < 250; i--)
			if (LMX2571regbuf[i] != LMX2571reg[i])
			{
				LMX2571reg[i] = LMX2571regbuf[i];
				if (LMX2571regbuf[i] != 0xffffff)
					WriteToLMX2571((LMX2571reg[i] & 0x0000ffff) | (i << 16));
			}
	}
}

void LMX2571_FuncInitOnlyfreCreat(void)
{
	LMX2571_FUNC.DITHERING = LMX2571_DITHERING_Medium;
	LMX2571_FUNC.CP_IDN_IDP = 16; // 8 = 1250 �A
	LMX2571_FUNC.CP_GAIN = LMX2571_CP_GAIN_1x;
	LMX2571_FUNC.SDO_LD_SEL = LMX2571_SDO_LD_SEL_Lock_detect_output;
	LMX2571_FUNC.LD_EN = LMX2571_LD_EN_Enabled;
	LMX2571_FUNC.MULT_WAIT = 520; // 520 = 20 �s
	LMX2571_FUNC.OUTBUF_AUTOMUTE = LMX2571_Disabled;
	LMX2571_FUNC.OUTBUF_TX_TYPE = Push_pull;
	LMX2571_FUNC.OUTBUF_RX_TYPE = Push_pull;
	LMX2571_FUNC.IPBUFDIFF_TERM = LMX2571_Disabled;
	LMX2571_FUNC.IPBUF_SE_DIFF_SEL = LMX2571_Disabled;
	LMX2571_FUNC.XTAL_PWRCTRL = LMX2571_XTAL_PWRCTRL_0o;
	LMX2571_FUNC.XTAL_EN = LMX2571_Disabled;
	LMX2571_FUNC.FSK_I2S_FS_POL = LMX2571_FSK_I2S_FS_POL_Active_HIGH;
	LMX2571_FUNC.FSK_I2S_CLK_POL = LMX2571_FSK_I2S_CLK_POL_Rising_edge_strobe;
	LMX2571_FUNC.FSK_LEVEL = LMX2571_FSK_LEVEL_Disabled;
	LMX2571_FUNC.FSK_DEV_SEL = LMX2571_FSK_DEV_SEL_FSK_DEV0_Fx;
	LMX2571_FUNC.FSK_MODE_SEL = LMX2571_FSK_MODE_SEL_FSK_SPI_FAST_mode;
	LMX2571_FUNC.RESET = LMX2571_Disabled;
	LMX2571_FUNC.POWERDOWN = LMX2571_Disabled;
	LMX2571_FUNC.RXTX_CTRL = LMX2571_register_programming;
	LMX2571_FUNC.RXTX_POL = LMX2571_RXTX_POL_Active_LOW;
	LMX2571_FUNC.F1F2_INIT = LMX2571_F1F2_INIT_Clear_bit_value;
	LMX2571_FUNC.F1F2_CTRL = LMX2571_register_programming;
	LMX2571_FUNC.F1F2_MODE = LMX2571_F1F2_Enable_F1F2_calibration;
	LMX2571_FUNC.F1F2_SEL = LMX2571_F1F2_SEL_F1_registers;
	LMX2571_FUNC.FCAL_EN = LMX2571_Enabled;

	F[LMX2571_F1].FSK_EN = LMX2571_Enabled;
	F[LMX2571_F1].OUTBUF_TX_PWR = 9;
	F[LMX2571_F1].OUTBUF_RX_PWR = 9;
	F[LMX2571_F1].OUTBUF_TX_EN = LMX2571_Enabled;
	F[LMX2571_F1].OUTBUF_RX_EN = LMX2571_Enabled;
	F[LMX2571_F1].LF_R4 = 4;
	F[LMX2571_F1].LF_R3 = 4;
	F[LMX2571_F1].CHDIV2 = 1;
	F[LMX2571_F1].CHDIV1 = 1;
	F[LMX2571_F1].PFD_DELAY = 4;
	F[LMX2571_F1].MULT = 1;
	F[LMX2571_F1].PLL_R = 1;
	F[LMX2571_F1].PLL_R_PRE = 1;
	F[LMX2571_F1].PLL_N_PRE = 1;
	F[LMX2571_F1].FRAC_ORDER = 3;
	F[LMX2571_F1].PLL_N = 1;
	F[LMX2571_F1].PLL_NUM = 1;
	F[LMX2571_F1].PLL_DEN = 1;
	F[LMX2571_F1].EXTVCO_SEL = LMX2571_Disabled;
	F[1] = F[0];
}
void Set_PLL(void)
{
	_PLL_state val;

	val.clkin = 100000000;
	val.fpd = 100000000;
	val.N_Divier_BIT = 12;
	val.NUM_Divier_BIT = 24;
	val.DEN_Divier_BIT = 24;
	val.PRE_R_Divier_BIT = 8;
	val.POST_R_Divier_BIT = 8;
	val.MULT_Divier_BIT = 4;
	val.min_fpfd_freq = 100000;
	val.max_fpfd_freq = 150000000;
	val.min_VCO_freq = 4300000000;
	val.max_VCO_freq = 5376000000;

	LMX2571_FuncInitOnlyfreCreat();
	PLL_Data_Init(val);
	LMX2571_default_pdata();
}

void LMX2571_Init(void)
{

	Config_GPIO(GPIOC, GPIO_Pin_0, GPIO_MODE_OUTPUT);//FDV
	Config_GPIO(GPIOC, GPIO_Pin_1, GPIO_MODE_OUTPUT);//FD0
	Config_GPIO(GPIOC, GPIO_Pin_2, GPIO_MODE_OUTPUT);//FD1
	Config_GPIO(GPIOC, GPIO_Pin_3, GPIO_MODE_OUTPUT);//FD2
	Config_GPIO(GPIOC, GPIO_Pin_4, GPIO_MODE_INPUT);//MUX
	Config_GPIO(GPIOC, GPIO_Pin_5, GPIO_MODE_OUTPUT);//CTL
	Config_GPIO(GPIOC, GPIO_Pin_6, GPIO_MODE_OUTPUT);//CLK
	Config_GPIO(GPIOC, GPIO_Pin_7, GPIO_MODE_OUTPUT);//LE
	Config_GPIO(GPIOB, GPIO_Pin_4, GPIO_MODE_OUTPUT);//DATA
	Config_GPIO(GPIOB, GPIO_Pin_5, GPIO_MODE_OUTPUT);//CE
	
	LMX2571_CE_1;

	Set_PLL();
}
