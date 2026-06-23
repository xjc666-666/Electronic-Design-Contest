#ifndef _PLL_H_
#define _PLL_H_
#include "stdint.h"
//#define u32 unsigned long int
//#define u8 unsigned char
//#define u16 unsigned int
#define u64 unsigned long long int
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

typedef struct PLL_Return {
	unsigned int			N_Divier;//N分频器位数
	unsigned int			NUM_Divier;//小数分频器num位数
	unsigned int			DEN_Divier;//小数分频器den位数
	unsigned int			PRE_R_Divier;//R分频器Pre位数
	unsigned int			POST_R_Divier;//R分频器Post位数
	unsigned int			CHDIV1;//输出分频器1位数
	unsigned int			CHDIV2;//输出分频器2位数
	unsigned int			Prescalar;
	unsigned int			MULT;
	unsigned long			nowfpd;

	
}_PLL_Return;

typedef struct PLL_state {
	unsigned long			clkin; //时钟频率
	unsigned long			fpd; //鉴相频率

	unsigned int			N_Divier_BIT;//N分频器位数
	unsigned int			NUM_Divier_BIT;//小数分频器num位数
	unsigned int			DEN_Divier_BIT;//小数分频器den位数
	unsigned int			PRE_R_Divier_BIT;//R分频器Pre位数
	unsigned int			POST_R_Divier_BIT;//R分频器Post位数
	unsigned int			MULT_Divier_BIT;//R分频器Post位数

	unsigned int			CHDIV1_BIT;//输出分频器1位数
	unsigned int			CHDIV2_BIT;//输出分频器2位数
	unsigned int			Prescalar_BIT;//预分频器位数

	unsigned long			min_fpfd_freq; //最小鉴相频率
	unsigned long			max_fpfd_freq; //最大鉴相频率
	unsigned long long int		min_VCO_freq; //最小输出频率
	unsigned long long int		max_VCO_freq; //最大输出频率


	unsigned int			CHDIV1_MIN;//输出分频器1最大值
	unsigned int			CHDIV1_MAX;//输出分频器1最大值
	unsigned int			CHDIV2_MIN;//输出分频器1最大值
	unsigned int			CHDIV2_MAX;//输出分频器1最大值
	unsigned int			MULT_MAX;//输入倍频器最大值
	unsigned int			N_Divier_MAX;//N分频器最大值
	unsigned int			NUM_Divier_MAX;//小数分频器最大值
	unsigned int			DEN_Divier_MAX;//小数分频器最大值
	unsigned int			PRE_R_Divier_MAX;//小数分频器最大值
	unsigned int			POST_R_Divier_MAX;//小数分频器最大值

	_PLL_Return PLL;

}_PLL_state;

void PLL_Data_Init(_PLL_state val);
u8 PLL_Calulate(u64 fre);
_PLL_state  GetPLLNum(void);


#endif
