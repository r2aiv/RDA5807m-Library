#include "rda5807m.h"
#include <stdio.h>

uint16_t RDA5807m_CTRL_REG=0x0000;
uint16_t RDA5807m_CHAN_REG=0x0000;

uint16_t RDA5807m_I2C_Buff[2];

void RDA5807m_Init()
{
    RDA5807m_CTRL_REG |= (CTRL_DHIZ | CTRL_DMUTE | CTRL_BASS | CTRL_ENABLE);
    RDA5807m_CHAN_REG |= (CHAN_SPACE & STEP_25) | (CHAN_BAND & BAND_87_108);
    RDA5807m_I2C_Buff[0]=RDA5807m_CTRL_REG;
    RDA5807m_I2C_Buff[1]=RDA5807m_CHAN_REG;

    // TODO: i2c transmiting registers

    printf("%.4x %.4x\r\n",RDA5807m_CTRL_REG,RDA5807m_CHAN_REG);
}

int main()
{
RDA5807m_Init();
return;
}