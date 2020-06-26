#include "stm32f103xb.h"

#include "util/delay.h"

void ADC1_2_IRQ_handler()
{
    volatile uint16_t usMeasurement = ADC1->DR;
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

    RCC->APB2ENR = (RCC_APB2ENR_ADC1EN |
                    RCC_APB2ENR_AFIOEN |
                    RCC_APB2ENR_IOPAEN);
    RCC->APB2RSTR = (RCC_APB2RSTR_ADC1RST |
                     RCC_APB2RSTR_AFIORST |
                     RCC_APB2RSTR_IOPARST);
    __delay_us(100.0);
    RCC->APB2RSTR &= ~(RCC_APB2RSTR_ADC1RST |
                       RCC_APB2RSTR_AFIORST |
                       RCC_APB2RSTR_IOPARST);
   
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV4;  /* Divide APB2 clock (36MHz) by 4 to get 9 MHz */
     
    GPIOA->CRL &= ~(GPIO_CRL_MODE1 |    /* PA1 is an analog input pin */
                    GPIO_CRL_CNF1); 
    
    ADC1->CR1 |= ADC_CR1_EOCIE;
    ADC1->SQR1 = 0;
    ADC1->SQR3 = ADC_SQR3_SQ1_0;        /* The first and only channel we want to read is channel 1 */
    ADC1->CR2 |= ADC_CR2_EXTSEL;        /* Trigger to start a conversion is software */
    ADC1->CR2 |= ADC_CR2_EXTTRIG;
    ADC1->SMPR2 = ADC_SMPR2_SMP1_1;     /* Add 13.5 cycles to the sample time */
    ADC1->CR2 |= ADC_CR2_CONT;          /* Continues conversion mode */

    ADC1->CR2 |= ADC_CR2_RSTCAL |
                 ADC_CR2_ADON;          /* Reset old calibration of the ADC2 module */
    while(ADC1->CR2 & ADC_CR2_RSTCAL);  
    ADC1->CR2 |= ADC_CR2_CAL;           /* Start a new calibration */
    while(ADC1->CR2 & ADC_CR2_CAL);

    NVIC_SetPriority(ADC1_2_IRQn, 3);
    NVIC_ClearPendingIRQ(ADC1_2_IRQn);
    NVIC_EnableIRQ(ADC1_2_IRQn);

    ADC1->CR2 |= ADC_CR2_SWSTART;       /* Start a conversion */
    while(1)
    {
        
    }

    return 0;
}
