#include "stm32f103xb.h"

void uart2_init()
{
    USART2->CR1 = (USART_CR1_RE |       /* Enable Receiver, Transmitter and the USART module */
                   USART_CR1_TE |
                   USART_CR1_UE);         
    USART2->BRR = 0x138;
}

void uart2_send(uint8_t ucByte)
{
    while(!(USART2->SR & USART_SR_TXE));

    USART2->DR = ucByte;

    while(!(USART2->SR & USART_SR_TC));
}
