#include "stm32f103xb.h"

#include "util/delay.h"

#include "uart.h"

int main()
{
    RCC->CR |= RCC_CR_HSEON;            /* Turn on the external high speed oscillator clock source */
    while(!(RCC->CR & RCC_CR_HSERDY));  /* Wait untill the external clock source is stable */

                                        /* Ratio system clock and flash access 
                                         * should be 2 wait states for 48 < sysclk ≤ 72 */
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    RCC->CFGR = 0;
    RCC->CFGR &= ~(RCC_CFGR_HPRE);      /* Do not divide the clock source for AHB */
    RCC->CFGR |= RCC_CFGR_PPRE1_2 |     /* Divide APB1 and APB2 by 2 (36 MHz is maximum) */
                 RCC_CFGR_PPRE2_2 |     
                 RCC_CFGR_PLLSRC |      /* HSE is the input clock source for the PLL */
                 RCC_CFGR_PLLXTPRE |
                 RCC_CFGR_PLLMULL12;

    RCC->CR |= RCC_CR_PLLON | RCC_CR_HSEBYP;

    while(RCC->CR & RCC_CR_PLLRDY);
    RCC->CFGR |= RCC_CFGR_SW_1;         /* Select the PLL clock as input source for our system clock */
                                        /* We should have a 72 MHz clock speed at this point */
    while(!(RCC->CFGR & RCC_CFGR_SW_1));

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;   /* Enable USART2 module */
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN |
                     RCC_APB2ENR_AFIOEN |
                     RCC_APB2ENR_USART1EN); /* Enable GPIOA module */

    RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;/* Reset USART2 module */
    RCC->APB2RSTR |= (RCC_APB2RSTR_IOPARST |/* Reset GPIOA and AFIO module */
                      RCC_APB2RSTR_AFIORST |
                      RCC_APB2RSTR_USART1RST);
    __delay_us(100.0);                  /* Give the modules some time to reset */
    RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;   
    RCC->APB2RSTR &= ~(RCC_APB2RSTR_IOPARST |
                       RCC_APB2RSTR_AFIORST |
                       RCC_APB2RSTR_USART1RST);
    __delay_us(100.0);

    GPIOA->CRL |= (GPIO_CRL_MODE2 |    /* PA9 is an alternate function output Push-Pull pin */
                  GPIO_CRL_CNF2_1);
    GPIOA->CRL &= ~(GPIO_CRL_MODE3 |
                    GPIO_CRL_CNF3_1 |
                    GPIO_CRL_CNF2_0);


    GPIOA->CRH |= (GPIO_CRH_MODE9 |    /* PA9 is an alternate function output Push-Pull pin */
                  GPIO_CRH_CNF9_1);
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 |
                    GPIO_CRH_CNF10_1);


    USART1->BRR = 0x138;
    USART1->CR1 |= (USART_CR1_RE |
                    USART_CR1_TE |
                    USART_CR1_UE);
    

    uart2_init();

    while(1)
    {
        uart2_send(0x12);
        USART1->DR = 0xAA;
    }

	return 0;
}
