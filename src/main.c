#include "stm32f103xb.h"

#include "util/delay.h"

volatile uint16_t usTurnsLeft = 0;
volatile uint16_t usTurnsRight = 0;

void EXTI0_IRQ_handler()
{
    EXTI->PR |= (EXTI_PR_PR0);          /* Acknowledge the interrupt */
    NVIC_ClearPendingIRQ(EXTI0_IRQn);

    if(GPIOB->IDR & GPIO_IDR_IDR1)      /* If PB1 is high when PB0 just had a falling edge we turn anti-clockwise */
        usTurnsLeft += 1;
    else                                /* Else if PB0 is already low when PB0 just had a falling edge we turn clockwise */
        usTurnsRight += 1;

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

    RCC->APB2ENR |= (RCC_APB2ENR_IOPBEN |
                     RCC_APB2ENR_AFIOEN);   /* Enable GPIO B module */
    
                                        /* Reset the GPIO B module */
    RCC->APB2RSTR |= (RCC_APB2RSTR_IOPBRST |
                      RCC_APB2RSTR_AFIORST);
    __delay_us(100.0);                  /* Give the module some time to reset */
    RCC->APB2RSTR &= ~(RCC_APB2RSTR_IOPBRST |
                       RCC_APB2RSTR_AFIORST);


    GPIOB->CRL &= ~(0x33);              /* PB0 and PB1 are input pins */

    AFIO->EXTICR[0] = 0x01;             /* PB is input for EXTI0 */

    EXTI->IMR = EXTI_IMR_MR0;           /* Line 1 is masked and thus enabled */
    EXTI->FTSR = EXTI_FTSR_TR0;         /* Falling edge is the trigger for an interrupt */


    NVIC_SetPriority(EXTI0_IRQn, 0x03);
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI0_IRQn);

    while(1) {}

	return 0;
}
