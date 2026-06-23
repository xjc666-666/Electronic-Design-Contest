#include "AD5933.h"
#include "delay.h"
#include "stm32f4xx.h"
#include "math.h"

//extern 
	uint32_t Frequency;
double AD5933_Correction[10];

float magnitude;
float resistance[200];
float rads[200];

int AD5933_Dat_Re[200];
int AD5933_Dat_Im[200];

long ReadTemp,realArr[3],imageArr[3];




// I2C GPIO初始化函数
void I2C_GPIO_INIT(void)      
{
		PDout(6); PDout(7);
		Set_GPIO_High(GPIOD, GPIO_Pin_6);
		Set_GPIO_High(GPIOD, GPIO_Pin_7);
}

// I2C获取ACK
void GetAck(void)
{   
	uint8_t ucErrTime=0;
	
	SDA_IN;      
	SDA_1; Delay_us(1);	   
	SCL_1; Delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			STOP();
			return;
		}
	}
	SCL_0; 	
}

// I2C发送NAck
void SendNACK(void)
{
	SCL_0;
	SDA_OUT;
	SDA_1;
	Delay_us(2);
	SCL_1;
	Delay_us(2);
	SCL_0;
}

// I2C开始信号
void START(void)   
{
	SDA_OUT;    
	SDA_1;	  	  
	SCL_1;
	Delay_us(4);
 	SDA_0;
	Delay_us(4);
	SCL_0;
}

// I2C停止信号
void STOP(void)
{
	SDA_OUT;
	SCL_0;
	SDA_0;
 	Delay_us(4);
	SCL_1; 
	SDA_1;
	Delay_us(4);		
}

// I2C发送一个字节数据
void SendByte(uint8_t txd)	
{
    uint8_t t;   
	SDA_OUT; 	    
    SCL_0;
    for(t = 0;t < 8;t++)
    {        
		if(txd & 0x80) SDA_1;
		else SDA_0;
        txd <<= 1; 	  
		Delay_us(2); 
		SCL_1;
		Delay_us(2); 
		SCL_0;	
		Delay_us(2);
    }	 
}

// I2C读取一个字节数据
uint8_t ReadByte(void) 
{
	uint8_t i,receive = 0;
	SDA_IN;
    for(i=0;i<8;i++ )
	{
        SCL_0; 
        Delay_us(2);
		SCL_1;
        receive <<= 1;
        if(READ_SDA)receive++;   
		Delay_us(1); 
    }					 
	SendNACK();
    return receive;
}

// I2C写入一个字节数据到指定地址
void WriteByte(char nAddr,uint nValue)
{   
	int nTemp = 0x1A;    
	
    START(); 
	SendByte(nTemp);    	
	GetAck();   
	
	SendByte(nAddr);    
	GetAck();
	
	SendByte(nValue);	
	GetAck();
		
	STOP();			
}

// 设置地址指针
void SetPointer(char nAddr)  
{          
	int nTemp = 0x1A; 
	
  START(); 
	SendByte(nTemp);   
	GetAck();  	

	SendByte(0xB0);   
	GetAck();

	SendByte(nAddr);	
	GetAck();	

	STOP();		
    return;
}

// 读取指定地址的字节数据
int Rece_Byte(char nAddr)
{   
	int nTemp ;    
	
    SetPointer(nAddr);   
    nTemp = 0x1B;      
    START(); 

	SendByte(nTemp);     	
	GetAck();  

	nTemp = ReadByte();		
	SendNACK();
	
	STOP();				
	return nTemp;
}

// 读取温度
uint16_t AD5933_Tempter(void)
{
	unsigned int Tm;        //保存实部，虚部，温度
   
    WriteByte(0x80,0x90);  //启动温度测量
    Tm = Rece_Byte(0x92);     //读出温度，保存在Tm中
    Tm <<= 8;
    Tm += Rece_Byte(0x93);
	Tm <<= 2;
	return Tm;
}

void Fre_To_Hex(float fre,uint8_t *buf)
{
	uint32_t dat;
	dat = (536870912/(double)(AD5933_MCLK*1000000)) * fre;  
	buf[0] = dat>>16;
	buf[1] = dat>>8;
	buf[2] = dat;
}


float AD5933_Sweep (float Fre_Begin,float Fre_UP,uint16_t UP_Num,uint16_t OUTPUT_Vatage,uint16_t Gain,uint16_t SWeep_Rep)
{
	uint8_t SValue[3], IValue[3], NValue[2], CValue[2];
	uint16_t buf=0;
	
	Fre_To_Hex(Fre_Begin,SValue);
	Fre_To_Hex(Fre_UP,IValue);
	
	NValue[0] = UP_Num>>8;
	NValue[1] = UP_Num;
	
#ifdef AD5933_MCLK_USE_OUT
	buf=OUTPUT_Vatage|Gain|SWeep_Rep|AD5933_OUT_MCLK;
#else
	buf = OUTPUT_Vatage|Gain|SWeep_Rep|AD5933_IN_MCLK;
#endif
	
	CValue[0] = buf>>8;
	CValue[1] = buf;
	
	Scale_imp(SValue,IValue,NValue,CValue);
	
	return 0;
}

//SValue[3]起始频率，IValue[3]频率增量，NValue[2]增量数，CValue[2]控制字，ki增益系数，Ps扫频为1重复为0
float Scale_imp (uint8_t *SValue,uint8_t *IValue,uint8_t *NValue,uint8_t *CValue)
{
		int i,AddrTemp;
		uint8_t Gain = ((~CValue[0]) & 0x01) ? 5 : 1;
		uint8_t SWeep_Rep = ((CValue[0]&0xF0) == (AD5933_Fre_UP>>8)) ? 1 : 0;
		uint8_t Mode = CValue[0]&0x0f;
			 
		AddrTemp=0X82; //初始化起始频率寄存器
		for(i = 0;i <3;i++)
		{
				WriteByte(AddrTemp,SValue[i]);
				AddrTemp++;
		}     
		
		AddrTemp=0X85; //初始化频率增量寄存器
		for(i = 0;i <3;i++)
		{
				WriteByte(AddrTemp,IValue[i]);
				AddrTemp++;
		} 
		
		AddrTemp=0X88; //初始化频率点数寄存器
		for(i = 0;i <2;i++)
	 {
				WriteByte(AddrTemp,NValue[i]);
				AddrTemp++;
	 } 
	 
	 //初始化控制寄存器，1011 0001 0000 0000待机模式，2V，一倍放大，内部时钟
	 
		AddrTemp=0X80; 
		WriteByte(AddrTemp,Mode|(AD5933_Standby>>8));
	 
		AddrTemp++;
		WriteByte(AddrTemp,CValue[1]);
	 
		AddrTemp++;
		WriteByte(0x80,Mode|(AD5933_SYS_Init>>8));//控制寄存器写入初始化频率扫描命令
		
	    Delay_ms(10);
		WriteByte(0X80,Mode|(AD5933_Begin_Fre_Scan>>8));//控制寄存器写入开始频率扫描命令
	 
		while(1)
	    { 
			while(1)
			{
				 ReadTemp=Rece_Byte(0x8F);  //读取状态寄存器检查DFT是否完成
				
				 if (ReadTemp&0x02)
				 break; 
			}  
		
			realArr[0]=Rece_Byte(0x94);
			realArr[1]=Rece_Byte(0x95);
			realArr[2]=(realArr[0]*0x100)+realArr[1];
		
			imageArr[0]=Rece_Byte(0x96);
			imageArr[1]=Rece_Byte(0x97);
			imageArr[2]=(imageArr[0]<<8)+imageArr[1]; 

			
			
			if (realArr[2]>=0x8000)  //计算实部的原码(除符号位外，取反加一)
			{
					realArr[2]^=0xFFFF; 
					realArr[2]^=0x8000; 
					realArr[2]+=1;
					realArr[2]^=0x8000;
			}
			if (imageArr[2]>=0x8000)  //计算虚部的原码(除符号位外，取反加一)
			{
					imageArr[2]^=0xFFFF; 
					imageArr[2]^=0x8000; 
					imageArr[2]+=1;
					imageArr[2]^=0x8000;
			}

			ReadTemp=Rece_Byte(0x8F);
			
			if (ReadTemp&0x04)
			break;
			
			if (SWeep_Rep==1)
			WriteByte(0X80,CValue[0]);	
			
			else
			WriteByte(0X80,CValue[0]);	
	    }  
		
	return magnitude;
}

// 获取电阻值
float DA5933_Get_Rs(void)
{
	AD5933_Sweep(Frequency,0,0,AD5933_OUTPUT_2V,AD5933_Gain_1,AD5933_Fre_UP);
	magnitude = sqrtf(realArr[2]*realArr[2]+imageArr[2]*imageArr[2]);
	return Fre_20K_Rr_1K / magnitude;
}

// 获取电容值
float DA5933_Get_Cap()
{
	AD5933_Sweep(1000,50,1,AD5933_OUTPUT_1V,AD5933_Gain_1,AD5933_Fre_UP);
	magnitude = sqrtf(realArr[2]*realArr[2]+imageArr[2]*imageArr[2]);
	return Fre_20K_Rr_1K / magnitude ;	
}

// 获取电感值
float DA5933_Get_L()
{
	float l;
	AD5933_Sweep(10000,1000,10,AD5933_OUTPUT_2V,AD5933_Gain_1,AD5933_Fre_UP);
	return l;
}



