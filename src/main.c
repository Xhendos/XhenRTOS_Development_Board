#include "stm32f103xb.h"

#include "util/delay.h"

#include "uart.h"

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

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;   /* Enable USART2 module */
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN |
                     RCC_APB2ENR_AFIOEN);   /* Enable GPIOA module */

    RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;/* Reset USART2 module */
    RCC->APB2RSTR |= (RCC_APB2RSTR_IOPARST |/* Reset GPIOA and AFIO module */
                      RCC_APB2RSTR_AFIORST);
    __delay_us(100.0);                  /* Give the modules some time to reset */
    RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;   
    RCC->APB2RSTR &= ~(RCC_APB2RSTR_IOPARST |
                       RCC_APB2RSTR_AFIORST);
    __delay_us(100.0);

    GPIOA->CRL |= (GPIO_CRL_MODE2_0 |   /* PA2 is an alternate function output Push-Pull pin */
                  GPIO_CRL_CNF2_1);
                                        /* PA3 is the Rx pin, but this is input floating pin by default */

    uart2_init();

    while(1)
    {
        uart2_send(0xAA);
    }

	return 0;
}
