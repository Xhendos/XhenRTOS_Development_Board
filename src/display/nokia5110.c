#include "stm32f103xb.h"
#include "../spi.h"
#include "../util/delay.h"

#include "nokia5110.h"

void Nokia5110_Init(NokiaDisplayMode_t nokiaDisplayMode)
{   

    GPIOA->BSRR |= (1 << 4);        /* Set the Chip Select (CS) high. This means 'I dont want to send data' */    
    GPIOA->BSRR |= (1 << 16);       /* Reset the Nokia 5110 (this must be done according to the datasheet
                                     * because register- and RAM content is undefined */
    __delay_us(2.0);                /* Give the Nokia 5110 a little bit of time */
    GPIOA->BSRR |= (1 << 0);        

    Spi1Init(FPCLK_DIV_32, CLOCKPHASE_0, 1, 1, 0, 1);
    Spi1Enable();

    GPIOA->BSRR |= (1 << 20);       /* Set the CS pin low. At this point the Nokia 5110 is waiting for data */

    Spi1Transmit(0x21);             /* Use extended instruction set */
    Spi1Transmit(0xB8);             /* B8 Vop */
    Spi1Transmit(0x04);             /* Tempearture coefficient */
    Spi1Transmit(0x14);             /* Bias */
    Spi1Transmit(0x20);             /* Use basic instruction set */

    switch(nokiaDisplayMode)
    {
        case 0:
            Spi1Transmit(0x08);
            break;
        case 1:
            Spi1Transmit(0x0C);
            break;
        case 2:
            Spi1Transmit(0x09);
            break;
        case 3:
            Spi1Transmit(0x0D);
            break;
    }


}

