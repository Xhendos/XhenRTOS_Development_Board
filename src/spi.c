#include "stm32f103xb.h"

#include "spi.h"
#include "util/delay.h"

void Spi1Init(SpiBaudrate_t spiBaudrate, SpiClockPhase_t spiClockPhase, uint8_t Ssm, uint8_t Ssi, uint8_t LsbFirst, uint8_t Mstr)
{
    SPI1->CR1 = 0;
    SPI1->CR1 |= (spiBaudrate << SPI_CR1_BR_Pos |
                  spiClockPhase << SPI_CR1_CPHA_Pos |
                  Ssm << SPI_CR1_SSM_Pos |
                  Ssi << SPI_CR1_SSI_Pos |
                  LsbFirst << SPI_CR1_LSBFIRST_Pos |
                  Mstr << SPI_CR1_MSTR_Pos);
}

void Spi1Enable(void)
{
    SPI1->CR1 |= SPI_CR1_SPE;
}

void Spi1Transmit(uint8_t data)
{
    SPI1->DR = data;                /* Put data in the data register */
    while(SPI1->SR & SPI_SR_BSY);   /* Wait untill the byte has been sent */
    
    __delay_us(0.01);               /* Without this line, some bytes are not transfered */

    return;
}
