#include "rda5807m.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

uint16_t RDA5807m_CTRL_REG=0x0000;
uint16_t RDA5807m_CHAN_REG=0x0000;

uint16_t RDA5807m_I2C_OutBuff[2];
uint16_t RDA5807m_I2C_InBuff[2];

extern I2C_HandleTypeDef hi2c1;

void RDA5807m_Reset()
{
		RDA5807m_CTRL_REG=0x00;
	  RDA5807m_CTRL_REG=0x00;
		RDA5807m_CTRL_REG |= CTRL_SOFT_RST;
	  RDA5807m_Send();
}

void RDA5807m_Init()
{
		RDA5807m_CTRL_REG |= CTRL_DEFAULT;
    RDA5807m_CHAN_REG |= CHAN_DEFAULT; // Запихнуть в макрос CHAN_DEFAULT	
		RDA5807m_CHAN_REG |= (208 << 6) | CHAN_TUNE;														 // Дает настройку на Детское Радио
	  RDA5807m_Send();       
}

void RDA5807m_Seek()
{
	RDA5807m_CTRL_REG |= 0x0000;
	RDA5807m_CHAN_REG &=	 ~0xFFF0; // Не трогаем диапазон и шаг, остальное стираем
	RDA5807m_CTRL_REG |= (CTRL_DEFAULT | CTRL_SEEK | CTRL_SEEKUP);	
	RDA5807m_Send();
}

void RDA5807m_SetBand(unsigned char Band)
{
	RDA5807m_CTRL_REG = 0x0000;
	RDA5807m_CTRL_REG |= CTRL_DEFAULT;
	RDA5807m_CHAN_REG &= ~0x000C;
	RDA5807m_CHAN_REG |= Band;
	RDA5807m_Send();		
}

void RDA5807m_SetStep(unsigned char Step)
{
	RDA5807m_CTRL_REG = 0x0000;
	RDA5807m_CTRL_REG |= CTRL_DEFAULT;
	RDA5807m_CHAN_REG &= ~0x0003;
	RDA5807m_CHAN_REG |= Step;
	RDA5807m_Send();		
}

void RDA5807m_SetFreq(float Freq)
{
	uint16_t FreqVal=0;
	
	float Step=0;
	
	switch(RDA5807m_CHAN_REG & CHAN_STEP)
	{
		case STEP_100:
			Step=0.1;
		break;
		case STEP_200:
			Step=0.2;
		break;
		case STEP_50:
			Step=0.05;
		break;
		case STEP_25:
			Step=0.025;
		break;
		default:
			Step=0;
		break;		
	}
	
	switch(RDA5807m_CHAN_REG & CHAN_BAND)
	{
		case BAND_76_108:
			if((Freq < 76)||(Freq > 108)) return;
			FreqVal=(int)((Freq-76)/Step);		
		break;
		case BAND_87_108:
			if((Freq < 87)||(Freq > 108)) return;
			FreqVal=(int)((Freq-87)/Step);			
		break;
		case BAND_76_91:
			if((Freq < 76)||(Freq > 91)) return;	
			FreqVal=(int)((Freq-76)/Step);			
		break;
		case BAND_65_76:
			if((Freq < 65)||(Freq > 91)) return;
			FreqVal=(int)((Freq-65)/Step);		
		break;
	}
	
	RDA5807m_CTRL_REG = 0x0000;
	RDA5807m_CTRL_REG |= CTRL_DEFAULT;
	RDA5807m_CHAN_REG &= ~0xFFC0;
	RDA5807m_CHAN_REG |= (FreqVal << 6) | CHAN_TUNE;
	RDA5807m_Send();	
}

void RDA5807m_Send()
{
		SwapBytes(&RDA5807m_I2C_OutBuff[0],&RDA5807m_CTRL_REG);
	  SwapBytes(&RDA5807m_I2C_OutBuff[1],&RDA5807m_CHAN_REG);   
		HAL_I2C_Master_Transmit(I2C_HANDLER,(0x10 << 1),(void *)&RDA5807m_I2C_OutBuff,sizeof(RDA5807m_I2C_OutBuff),10);
		//HAL_Delay(100);
	  for(auto int i=0;i<100000;i++);
	  // TODO: Заменить на дефайны (I2C и т.п.)
}

void SwapBytes(uint16_t *Dst,uint16_t *Src) // Src is ABCD, Dst is CDAB
{
	uint16_t temp;
	temp=((*Src & 0x00FF) << 8);
	temp+=((*Src & 0xFF00) >> 8);
	*Dst=temp;	
}


uint16_t RDA5807m_GetChan()
{
	uint16_t Chan;
	memset(RDA5807m_I2C_InBuff,0,sizeof(RDA5807m_I2C_InBuff));
	HAL_I2C_Master_Receive(I2C_HANDLER,(0x10 << 1),(void *)&RDA5807m_I2C_InBuff, sizeof(RDA5807m_I2C_InBuff),10);	
	SwapBytes(&RDA5807m_I2C_InBuff[0],&RDA5807m_I2C_InBuff[0]);
	Chan=RDA5807m_I2C_InBuff[0] & 0x3FF;
	HAL_Delay(100);
	return Chan;
	
}

uint16_t RDA5807m_GetRSSI()
{
	uint16_t RSSI;
	memset(RDA5807m_I2C_InBuff,0,sizeof(RDA5807m_I2C_InBuff));
	HAL_I2C_Master_Receive(I2C_HANDLER,(0x10 << 1),(void *)&RDA5807m_I2C_InBuff, sizeof(RDA5807m_I2C_InBuff),10);
	SwapBytes(&RDA5807m_I2C_InBuff[1],&RDA5807m_I2C_InBuff[1]);
	RSSI=(RDA5807m_I2C_InBuff[1] & 0x7F00) >> 9;
	HAL_Delay(100);
	return RSSI;
}

float RDA5807m_GetFreq()
{
	float Step=0;
	uint16_t Chan=0;
	uint8_t LowerFreq=0;
	
	switch(RDA5807m_CHAN_REG & CHAN_STEP)
	{
		case STEP_100:
			Step=0.1;
		break;
		case STEP_200:
			Step=0.2;
		break;
		case STEP_50:
			Step=0.05;
		break;
		case STEP_25:
			Step=0.025;
		break;
		default:
			Step=0;
		break;		
	}
	
	switch(RDA5807m_CHAN_REG & CHAN_BAND)
	{
		case BAND_76_108:
			LowerFreq=76;		
		break;
		case BAND_87_108:
			LowerFreq=87;			
		break;
		case BAND_76_91:
			LowerFreq=76;		
		break;
		case BAND_65_76:
			LowerFreq=65;	
		break;
	}
	
	Chan=RDA5807m_GetChan();
	
	return Chan*Step+LowerFreq;
	
}
