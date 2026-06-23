#include "stm32f4xx.h"
#include "AD9959.h"
#include "GPIO.h"
#include "delay.h"

uint8_t FR1_DATA[3] = {0xD0,0x00,0x00};     //VCO gain control[23]=1系统时钟高于255Mhz; PLL[22:18]=10100,20倍频,20*25M=500MHZ; Charge pump control = 75uA 
uint8_t FR2_DATA[2] = {0x00,0x00};	        // 双方向扫描，即从起始值扫到结束值后，又从结束值扫到起始值
//uint8_t FR2_DATA[2] = {0x80,0x00};        // 单方向扫描，即从起始值扫到结束值后，又从起始值扫到结束值，以此类推
uint8_t CFR_DATA[3] = {0x00,0x03,0x02};     //default Value = 0x000302	   //通道功能寄存器
double ACC_FRE_FACTOR = 8.589934592;	    //频率因子8.589934592=(2^32)/500000000 其中500M=25M*20(倍频数可编程)

void Intserve(void)
{
    PDC_0;
    CS_1;
    SCK_0;
    UP_0;
    P0_0;
    P1_0;
    P2_0;
    P3_0;
    SD0_0;
    SD1_0;
    SD2_0;
    SD3_0;
}

void IntReset(void)
{
    RST_0;
    Delay_us(10);
    RST_1;
    Delay_us(10);
    RST_0;
}

void IO_Update(void)
{
    UP_0;
    Delay_us(10);
    UP_1;
    Delay_us(10);
    UP_0;
}

void AD9959_WriteData(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData)
{
    uint8_t ControlValue = 0;
    uint8_t ValueToWrite = 0;
    uint8_t ResisterIndex = 0;
    uint8_t i = 0;

    ControlValue = RegisterAddress;
// 写入地址
    SCK_0;
    CS_0;
    for(i = 0; i < 8; i++)
    {
        SCK_0;
        if(0x80 == (ControlValue & 0x80)) {SD0_1;}
        else {SD0_0;}
        SCK_1;
        ControlValue <<= 1;
    }
    SCK_0;

// 写入数据
    for(ResisterIndex = 0; ResisterIndex < NumberofRegisters; ResisterIndex++)
    {
        ValueToWrite = RegisterData[ResisterIndex];
        for(i = 0; i < 8; i++)
        {
            SCK_0;
            if(0x80 == (ValueToWrite & 0x80)) {SD0_1;}
            else {SD0_0;}
            SCK_1;
            ValueToWrite <<= 1;
        }
        SCK_0;
    }
    CS_1;
}

void AD9959_Init(void)
{
    PBout(1);
    PFout(14);
    PAout(1);
    PFout(7);
    PFout(9);
    PCout(0);
    PCout(2);
    PAout(2);
    PAout(0);
    PFout(8);
    PFout(10);
    PCout(1);
    PCout(3);

    Intserve();
    IntReset();

    AD9959_WriteData(FR1_ADD, 3, FR1_DATA);
    AD9959_WriteData(FR2_ADD, 2, FR2_DATA);
}

void Write_CFTW0(uint32_t fre)
{
    uint8_t CFTW0_DATA[4] = {0, 0, 0, 0};
    uint32_t temp;
    temp = (uint32_t)fre * ACC_FRE_FACTOR;
    CFTW0_DATA[0] = (uint8_t)(temp >> 24);
    CFTW0_DATA[1] = (uint8_t)(temp >> 16);
    CFTW0_DATA[2] = (uint8_t)(temp >> 8);
    CFTW0_DATA[3] = (uint8_t)(temp);
    AD9959_WriteData(CFTW0_ADD, 4, CFTW0_DATA);
}

void Write_ACR(uint16_t Amp)
{
    uint32_t A_temp = 0;
    uint8_t ACR_DATA[3] = {0, 0, 0};
    A_temp = Amp | 0x1000;

    ACR_DATA[1] = (uint8_t)(A_temp >> 8);
    ACR_DATA[2] = (uint8_t)(A_temp);
    AD9959_WriteData(ACR_ADD, 3, ACR_DATA);
}

void Write_CPOW0(uint16_t Phase)
{
	uint8_t CPOW0_data[2] = {0x00,0x00};
	CPOW0_data[1]=(uint8_t)Phase;
	CPOW0_data[0]=(uint8_t)(Phase>>8);

	AD9959_WriteData(CPOW0_ADD,2,CPOW0_data);//CPOW0 address 0x05.CHn设定相位
}

void Write_LSRR(uint8_t rsrr,uint8_t fsrr)
{
	uint8_t LSRR_data[2]={0x00,0x00};

	LSRR_data[1]=rsrr;	
	LSRR_data[0]=fsrr;//高8位下降斜率

	AD9959_WriteData(LSRR_ADD,2,LSRR_data);
}

void Write_RDW(uint32_t r_delta)
{
	uint8_t RDW_data[4] ={0x00,0x00,0x00,0x00};	//中间变量          
 
	RDW_data[3]=(uint8_t)r_delta;
	RDW_data[2]=(uint8_t)(r_delta>>8);
	RDW_data[1]=(uint8_t)(r_delta>>16);
	RDW_data[0]=(uint8_t)(r_delta>>24);		
	AD9959_WriteData(RDW_ADD,4,RDW_data);
}

void Write_FDW(uint32_t f_delta)
{
	uint8_t FDW_data[4] ={0x00,0x00,0x00,0x00};	//中间变量          
 
	FDW_data[3]=(uint8_t)f_delta;
	FDW_data[2]=(uint8_t)(f_delta>>8);
	FDW_data[1]=(uint8_t)(f_delta>>16);
	FDW_data[0]=(uint8_t)(f_delta>>24);		
	AD9959_WriteData(FDW_ADD,4,FDW_data);
}

void Write_Profile_Fre(uint8_t profile,uint32_t data)
{
	uint8_t profileAddr;
	uint8_t Profile_data[4] ={0x00,0x00,0x00,0x00};	//中间变量
	uint32_t Temp;            
	Temp=(uint32_t)data*ACC_FRE_FACTOR;	   //将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	Profile_data[3]=(uint8_t)Temp;
	Profile_data[2]=(uint8_t)(Temp>>8);
	Profile_data[1]=(uint8_t)(Temp>>16);
	Profile_data[0]=(uint8_t)(Temp>>24);
	profileAddr = PROFILE_ADDR_BASE + profile;
		
	AD9959_WriteData(profileAddr,4,Profile_data);
}

void Write_Profile_Ampli(uint8_t profile,uint16_t data)
{
	uint8_t profileAddr;
	uint8_t Profile_data[4] ={0x00,0x00,0x00,0x00};	//中间变量

	//为幅度调制时幅度数据为[31:22]位
	Profile_data[1]=(uint8_t)(data<<6);//[23:22]
	Profile_data[0]=(uint8_t)(data>>2);//[31:24]
		
	profileAddr = PROFILE_ADDR_BASE + profile;
		
	AD9959_WriteData(profileAddr,4,Profile_data);//写入32位数据
}

void Write_Profile_Phase(uint8_t profile,uint16_t data)
{
	uint8_t profileAddr;
	uint8_t Profile_data[4] ={0x00,0x00,0x00,0x00};	//中间变量

	//为相位调制时相位数据为[31:18]位
    Profile_data[1]=(uint8_t)(data<<2);//[23:18]
	Profile_data[0]=(uint8_t)(data>>6);//[31:24]
		
	profileAddr = PROFILE_ADDR_BASE + profile;
		
	AD9959_WriteData(profileAddr,4,Profile_data);//写入32位数据
}

void AD9959_Set_Fre(uint8_t Channel,uint32_t Freq)
{	 
	uint8_t CHANNEL[1] = {0x00};
		
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL);//控制寄存器写入CHn通道，选择CHn
    Write_CFTW0(Freq);//输出CHn设定频率																																			 
} 

void AD9959_Set_Amp(uint8_t Channel, uint16_t Ampli)
{ 
	uint8_t CHANNEL[1] = {0x00};

	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn
	Write_ACR(Ampli);							//	CHn设定幅度
}

void AD9959_Set_Phase(uint8_t Channel,uint16_t Phase)
{
	uint8_t CHANNEL[1] = {0x00};
	CHANNEL[0]=Channel;

	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn
	Write_CPOW0(Phase);//CHn设定相位
}

void AD9959_Modulation_Init(uint8_t Channel,uint8_t Modulation,uint8_t Sweep_en,uint8_t Nlevel)
{
	uint8_t i=0;
	uint8_t CHANNEL[1]={0x00};
	uint8_t FR1_data[3];
	uint8_t FR2_data[2];
	uint8_t CFR_data[3];
	for(i=0;i<3;i++)//设置默认值
	{
		FR1_data[i]=FR1_DATA[i];
		CFR_data[i]=CFR_DATA[i];
	}
	FR2_data[0]=FR2_DATA[0];
	FR2_data[1]=FR2_DATA[1];
		
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	
	FR1_data[1]=Nlevel;
	CFR_data[0]=Modulation;
	CFR_data[1]|=Sweep_en;
	CFR_data[2]=0x00;

	if(Channel!=0)//有通道被使能
	{
		AD9959_WriteData(FR1_ADD,3,FR1_data);//写功能寄存器1
		AD9959_WriteData(FR2_ADD,2,FR2_data);//写功能寄存器1
		AD9959_WriteData(CFR_ADD,3,CFR_data);//写通道功能寄存器
	}
}

void AD9959_SetFSK(uint8_t Channel, uint32_t *data,uint16_t Phase)
{
	uint8_t i=0;
	uint8_t CHANNEL[1]={0x00};	
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	Write_CPOW0(Phase);//设置相位
	
	Write_CFTW0(data[0]);
	for(i=0;i<15;i++)
	Write_Profile_Fre(i, data[i+1]);	
}

void AD9959_SetASK(uint8_t Channel, uint16_t *data,uint32_t fre,uint16_t Phase)
{
	uint8_t i=0;
	uint8_t CHANNEL[1]={0x00};	
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	Write_CFTW0(fre);//设置频率
	Write_CPOW0(Phase);//设置相位
	
	Write_ACR(data[0]);
	for(i=0;i<15;i++)
	Write_Profile_Ampli(i, data[i+1]);	
}

void AD9959_SetPSK(uint8_t Channel, uint16_t *data,uint32_t Freq)
{
	uint8_t i=0;
	uint8_t CHANNEL[1]={0x00};	
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	Write_CFTW0(Freq);
	
	Write_CPOW0(data[0]);
	for(i=0;i<15;i++)
		Write_Profile_Phase(i, data[i+1]);	
}

void AD9959_SetFre_Sweep(uint8_t Channel, uint32_t s_data,uint32_t e_data,uint32_t r_delta,uint32_t f_delta,uint8_t rsrr,uint8_t fsrr,uint16_t Ampli,uint16_t Phase)
{
	uint8_t CHANNEL[1]={0x00};
	uint32_t Fer_data=0;            
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	
	Write_CPOW0(Phase);//设置相位
	Write_ACR(Ampli); //幅度设置
	
	Write_LSRR(rsrr,fsrr);//斜率
	
	Fer_data=(uint32_t)r_delta*ACC_FRE_FACTOR;	 //频率转换成控制字
	Write_RDW(Fer_data);//上升步长
	
	Fer_data=(uint32_t)f_delta*ACC_FRE_FACTOR;
	Write_FDW(Fer_data);//下降步长
	
	Write_CFTW0(s_data);//起始频率
	Write_Profile_Fre(0, e_data);//结束频率
}

void AD9959_SetAmp_Sweep(uint8_t Channel, uint32_t s_Ampli,uint16_t e_Ampli,uint32_t r_delta,uint32_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Phase)
{
	uint8_t CHANNEL[1]={0x00};
	uint8_t ACR_data[3] = {0x00,0x00,0x00};
		
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	
	Write_CFTW0(fre); //幅度频率
	Write_CPOW0(Phase);//设置相位
	
	Write_LSRR(rsrr,fsrr);//斜率
	
	Write_RDW(r_delta<<22);//上升步长
	
	Write_FDW(f_delta<<22);//下降步长

	ACR_data[1] = (uint8_t)(s_Ampli>>8); //高位数据
	ACR_data[2] = (uint8_t)s_Ampli;  //低位数据
  AD9959_WriteData(ACR_ADD,3,ACR_data); //ACR 设定起始幅度
					
	Write_Profile_Ampli(0, e_Ampli);//结束幅度
}

void AD9959_SetPhase_Sweep(uint8_t Channel, uint16_t s_data,uint16_t e_data,uint16_t r_delta,uint16_t f_delta,uint8_t rsrr,uint8_t fsrr,uint32_t fre,uint16_t Ampli)
{
	uint8_t CHANNEL[1]={0x00};
	
	CHANNEL[0]=Channel;
	AD9959_WriteData(CSR_ADD,1,CHANNEL); //控制寄存器写入CHn通道，选择CHn；以下设置均针对CHn
	
	Write_CFTW0(fre); //幅度频率
	Write_ACR(Ampli); //幅度设置
	
	Write_LSRR(rsrr,fsrr);//斜率
	
	Write_RDW(r_delta<<18);//上升步长
	
	Write_FDW(f_delta<<18);//下降步长
	
	Write_CPOW0(s_data);//起始相位
	Write_Profile_Phase(0, e_data);//结束相位
}























































































































