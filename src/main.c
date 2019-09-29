#include "stm32f103xb.h"

#include "util/delay.h"

#include "spi.h"
#include "display/nokia5110.h"

int main()
{
    RCC->CR |= RCC_CR_HSEON;            /* Turn on the external high speed oscillator clock source */
    while(!(RCC->CR & RCC_CR_HSERDY));  /* Wait untill the external clock source is stable */

    RCC->CFGR &= ~(RCC_CFGR_HPRE);      /* Do not divide the clock source for AHB */
    RCC->CFGR |= RCC_CFGR_PPRE1_2 |     /* Divide APB1 and APB2 by 2 (36 MHz is maximum) */
                 RCC_CFGR_PPRE2_2 |     
                 RCC_CFGR_PLLSRC |      /* HSE is the input clock source for the PLL */
                 RCC_CFGR_PLLMULL_2;    /* Multiply the clock source by 6 (12 MHz * 6 = 72 MHz) */

                                        /* Ratio system clock and flash access 
                                         * should be 2 wait states for 48 < sysclk ≤ 72 */
    FLASH->ACR &= ~(FLASH_ACR_LATENCY_0);   
    
    RCC->CR |= RCC_CR_PLLON;            
    while(!(RCC->CR & RCC_CR_PLLRDY));  /* Wait untill the PLL is locked (which means ready) */
                                        /* We should have a 72 MHz clock speed at this point */
    
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN |    /* Enable alternative function I/O module */
                    RCC_APB2ENR_IOPAEN |    /* Enable GPIO port A module */ 
                    RCC_APB2ENR_IOPBEN |    /* Enable GPIO port B module */
                    RCC_APB2ENR_SPI1EN;     /* Enable SPI1 module */
    
    RCC->APB2RSTR |= RCC_APB2RSTR_AFIORST |
                     RCC_APB2RSTR_IOPARST |      
                     RCC_APB2RSTR_IOPBRST |      
                     RCC_APB2RSTR_SPI1RST;          
    __delay_us(100.0);                  /* Give the modules some time to reset */
    RCC->APB2RSTR &= ~(RCC_APB2RSTR_AFIORST |
                       RCC_APB2RSTR_IOPARST |    
                       RCC_APB2RSTR_IOPBRST |    
                       RCC_APB2RSTR_SPI1RST);    


   /*****************************
    *           *               *
    * Pin name  * Pin function  *
    *****************************
    *   PA4     *   SPI1_NSS    *
    *   PA5     *   SPI1_SCK    *
    *   PA6     *   SPI1_MISO   *
    *   PA7     *   SPI1_MOSI   *
    ****************************/

    GPIOA->CRL = 0;
    GPIOA->CRL |= (GPIO_CRL_MODE4 |
                   GPIO_CRL_MODE5 |
                   GPIO_CRL_MODE7);
    GPIOA->CRL |= (GPIO_CRL_CNF5_1 |    /* PA5 is alternative function output push pull */
                   GPIO_CRL_CNF6_1 |    /* PA6 is input with pull up */
                   GPIO_CRL_CNF7_1);    /* PA7 is alternative function output push pull */

    GPIOA->CRL |= (GPIO_CRL_MODE3 |     /* PA3 is general purpose output push pull */
                   GPIO_CRL_MODE0);     /* PA0 is general purpose output push pull */
    GPIOA->CRL &= ~(GPIO_CRL_CNF3 |
                    GPIO_CRL_CNF0); 
  

    Nokia5110_Init(NOKIA_NORMAL);       /* Initialise Nokia 5110 display. This function does all the reset and init registers */

    __delay_us(1.0);
    Nokia5110_WriteChar('p');

    Nokia5110_WriteString("hello, world");
    while(1);

    return 0;
}
