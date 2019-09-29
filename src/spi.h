#ifndef _SPI_H
#define _SPI_H

typedef enum SpiBaudrate
{
    FPCLK_DIV_2 = 0,
    FPCLK_DIV_4,
    FPCLK_DIV_8,
    FPCLK_DIV_16,
    FPCLK_DIV_32,
    FPCLK_DIV_64,
    FPCLK_DIV_128,
    FPCLK_DIV_256
} SpiBaudrate_t;

typedef enum
{
    CLOCKPHASE_0 = 0,
    CLOCKPHASE_1,
    CLOCKPHASE_2,
    CLOCKPHASE_3
} SpiClockPhase_t;

/*
 * 
 *
 *
 */
void Spi1Init(SpiBaudrate_t spiBaudrate, SpiClockPhase_t spiClockPhase, uint8_t Ssm, uint8_t Ssi, uint8_t LsbFirst, uint8_t Mstr);


void Spi1Enable(void);

void Spi1Transmit(uint8_t data);

#endif  /* _SPI_H */
