#include "stm32f103xb.h"

#include "util/delay.h"

void TIM2_IRQ_handler()
{
    static int i = 0;
    i++;
}

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

    RCC->APB1ENR |= (1 << 0);           /* Enable the Timer2 module */        
    
    RCC->APB1RSTR |= (1 << 0);          /* Reset the Timer2 module */
    __delay_us(100.0);
    RCC->APB1RSTR &= ~(1 << 0);

    TIM2->CR1 = (1 << 7);               /* Auto-reload preload enable */
    TIM2->DIER = (1 << 0);
    TIM2->CNT = 65530;
    TIM2->PSC = 35540;
    TIM2->ARR = 34412;

    TIM2->CR1 |= (1 << 0);

    NVIC_SetPriority(TIM2_IRQn, 3);
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    NVIC_EnableIRQ(TIM2_IRQn);

    while(1) {}

	return 0;
}
