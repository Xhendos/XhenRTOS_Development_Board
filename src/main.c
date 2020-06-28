#include "stm32f103xb.h"

#include "util/delay.h"

volatile uint8_t ucBitsSampled = 0;
volatile uint8_t ucInfraredBusy = 0;
volatile uint16_t usInfraredMessage;
uint16_t usInfraredMessage2;

void TIM2_IRQ_handler()
{
    uint8_t ucBit; 
    TIM2->SR &= ~(TIM_SR_UIF);          /* Acknowledge the interrupt */
    if(ucInfraredBusy == 0)
    {
        usInfraredMessage = 0;
        ucInfraredBusy = 1;
        TIM3->CR1 |= TIM_CR1_CEN;
    } else if(ucInfraredBusy != 0)
    {
        ucBit = ((GPIOA->IDR & GPIO_IDR_IDR4) >> 4);
        usInfraredMessage |= (ucBit << ucBitsSampled);
        ucBitsSampled += 1; 
    }
    TIM2->CR1 |= (1 << 0);
}

void TIM3_IRQ_handler()
{
    TIM3->SR &= ~(TIM_SR_UIF);

    GPIOA->ODR ^= (GPIO_ODR_ODR8);
    ucBitsSampled = 0; 

    TIM2->CR1 &= ~(TIM_CR1_CEN);
    
    if(usInfraredMessage != 65535)
        usInfraredMessage2 = usInfraredMessage;
    
    ucInfraredBusy = 0;
}

void EXTI4_IRQ_handler()
{
    EXTI->PR |= (EXTI_PR_PR12);
    TIM2->CR1 |= TIM_CR1_CEN;
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

    RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN |
                     RCC_APB1ENR_TIM3EN);
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN |
                     RCC_APB2ENR_AFIOEN);

    RCC->APB1RSTR |= (RCC_APB1RSTR_TIM2RST |
                      RCC_APB1RSTR_TIM3RST);
    RCC->APB2RSTR |= (RCC_APB2RSTR_IOPARST |
                      RCC_APB2RSTR_AFIORST);
    __delay_us(100.0);                  /* Give the modules some time to reset */
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST |
                       RCC_APB1RSTR_TIM3RST);
    RCC->APB2RSTR &= ~(RCC_APB2RSTR_IOPARST |
                       RCC_APB2RSTR_AFIORST);

    GPIOA->CRH |= GPIO_CRH_MODE8;       
    GPIOA->CRH &= ~(GPIO_CRH_CNF8);

    GPIOA->CRH |= GPIO_CRH_CNF12_1;
    GPIOA->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12_0);

    GPIOA->ODR |= GPIO_ODR_ODR8;
       
    TIM2->CR1 = (1 << 7);               /* Auto-reload preload enable */
    TIM2->CR1 |= (1 << 4);
    TIM2->DIER = (1 << 0);
    TIM2->CNT = 103;
    TIM2->PSC = 71;
    TIM2->ARR = 103;
    TIM2->CR1 |= (1 << 3);

    TIM3->CR1 = (1 << 7);               /* Auto-reload preload enable */
    TIM3->CR1 |= (1 << 4);
    TIM3->DIER = (1 << 0);
    TIM3->CNT = 3003;
    TIM3->PSC = 71;
    TIM3->ARR = 3003;
    TIM3->CR1 |= (1 << 3);

    AFIO->EXTICR[3] &= ~(AFIO_EXTICR4_EXTI12_PA);
    
    
    AFIO->EXTICR[0] |= 0x10;
    EXTI->IMR = EXTI_IMR_MR4;
    EXTI->FTSR = EXTI_FTSR_TR4;


    NVIC_SetPriority(TIM2_IRQn, 3);
    NVIC_EnableIRQ(TIM2_IRQn);
    
    NVIC_SetPriority(TIM3_IRQn, 2);
    NVIC_EnableIRQ(TIM3_IRQn);

    NVIC_SetPriority(EXTI4_IRQn, 1);
    NVIC_ClearPendingIRQ(EXTI4_IRQn);
    NVIC_EnableIRQ(EXTI4_IRQn);

    while(1) {}
    
	return 0;
}
