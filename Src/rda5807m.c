#include "rda5807m.h"
#include <stdio.h>

uint16_t RDA5807m_CTRL_REG=0x0000;
uint16_t RDA5807m_CHAN_REG=0x0000;

uint16_t RDA5807m_I2C_Buff[2];

void RDA5807m_Reset()
{
		RDA5807m_CTRL_REG=0x00;
	  RDA5807m_CTRL_REG=0x00;
		RDA5807m_CTRL_REG |= CTRL_SOFT_RST;
	  RDA5807m_Send();
}

void RDA5807m_Init()
{
		RDA5807m_CTRL_REG |= (CTRL_DHIZ | CTRL_DMUTE | CTRL_BASS | CTRL_ENABLE | CTRL_SEEK);
    RDA5807m_CHAN_REG |= (CHAN_STEP & STEP_100) | (CHAN_BAND & BAND_76_108) | CHAN_TUNE;	
		RDA5807m_CHAN_REG |= (208 << 6); // Дает настройку на Детское Радио
	  RDA5807m_Send();       
}

void RDA5807m_Seek()
{
	RDA5807m_CTRL_REG |= (CTRL_SEEK | CTRL_SEEKUP);
	RDA5807m_Send();
}

void RDA5807m_SetFreq(float Freq)
{
	uint16_t FreqVal=0;
	uint8_t Band=RDA5807m_CHAN_REG & CHAN_BAND;
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
			FreqVal=(Freq-76)*Step;
		break;
		case BAND_87_108:
			FreqVal=(Freq-87)*Step;
		break;
		case BAND_76_91:
			FreqVal=(Freq-76)*Step;
		break;
		case BAND_65_76:
			FreqVal=(Freq-65)*Step;
		break;
	}
	RDA5807m_CHAN_REG &= ~0xFFC0;
	RDA5807m_CHAN_REG |= (FreqVal << 6) | CHAN_TUNE;
	RDA5807m_Send();
	
}

void RDA5807m_Send()
{
		SwapBytes(&RDA5807m_CTRL_REG);
	  SwapBytes(&RDA5807m_CHAN_REG);	
    RDA5807m_I2C_Buff[0]=RDA5807m_CTRL_REG;
    RDA5807m_I2C_Buff[1]=RDA5807m_CHAN_REG;	
	  // TODO: i2c transmiting registers 
}

void SwapBytes(uint16_t *Data)
{
	uint16_t temp;
	temp=((*Data & 0x00FF) << 8);
	temp+=((*Data & 0xFF00) >> 8);
	*Data=temp;	
}
