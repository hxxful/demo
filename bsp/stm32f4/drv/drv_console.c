#include "drv_console.h"

void drv_console_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_AHB1PeriphClockCmd(CONSOLE_USART_RX_GPIO_CLK | CONSOLE_USART_TX_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(CONSOLE_USART_CLK, ENABLE);
    GPIO_PinAFConfig(CONSOLE_USART_RX_GPIO_PORT, CONSOLE_USART_RX_SOURCE, CONSOLE_USART_RX_AF);
    GPIO_PinAFConfig(CONSOLE_USART_TX_GPIO_PORT, CONSOLE_USART_TX_SOURCE, CONSOLE_USART_TX_AF);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = CONSOLE_USART_RX_PIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CONSOLE_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = CONSOLE_USART_TX_PIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CONSOLE_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = CONSOLE_USART_BAUDRATE;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(CONSOLE_USART, &USART_InitStructure);
    USART_Cmd(CONSOLE_USART, ENABLE);

    USART_ClearFlag(CONSOLE_USART, USART_FLAG_TC);
}

void drv_console_write(uint16_t ch)
{
    USART_SendData(CONSOLE_USART, ch);
    while(RESET == USART_GetFlagStatus(CONSOLE_USART, USART_FLAG_TXE));
}

uint16_t drv_console_read(void)
{
    while(RESET == USART_GetFlagStatus(CONSOLE_USART, USART_FLAG_RXNE));
    return USART_ReceiveData(CONSOLE_USART);
}
