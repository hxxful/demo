#ifndef DRV_CONSOLE_H
#define DRV_CONSOLE_H

#include "drv_common.h"

#define CONSOLE_USART                             USART1
#define CONSOLE_USART_CLK                         RCC_APB2Periph_USART1
#define CONSOLE_USART_BAUDRATE                    115200

#define CONSOLE_USART_RX_GPIO_PORT                GPIOA
#define CONSOLE_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define CONSOLE_USART_RX_PIN                      GPIO_Pin_10
#define CONSOLE_USART_RX_AF                       GPIO_AF_USART1
#define CONSOLE_USART_RX_SOURCE                   GPIO_PinSource10

#define CONSOLE_USART_TX_GPIO_PORT                GPIOA
#define CONSOLE_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define CONSOLE_USART_TX_PIN                      GPIO_Pin_9
#define CONSOLE_USART_TX_AF                       GPIO_AF_USART1
#define CONSOLE_USART_TX_SOURCE                   GPIO_PinSource9


//#define CONSOLE_USART                             USART1
//#define CONSOLE_USART_CLK                         RCC_APB2Periph_USART1
//#define CONSOLE_USART_BAUDRATE                    115200

//#define CONSOLE_USART_RX_GPIO_PORT                GPIOB
//#define CONSOLE_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
//#define CONSOLE_USART_RX_PIN                      GPIO_Pin_3
//#define CONSOLE_USART_RX_AF                       GPIO_AF_USART1
//#define CONSOLE_USART_RX_SOURCE                   GPIO_PinSource3

//#define CONSOLE_USART_TX_GPIO_PORT                GPIOA
//#define CONSOLE_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
//#define CONSOLE_USART_TX_PIN                      GPIO_Pin_15
//#define CONSOLE_USART_TX_AF                       GPIO_AF_USART1
//#define CONSOLE_USART_TX_SOURCE                   GPIO_PinSource15

extern void drv_console_init(void);
extern void drv_console_write(uint16_t ch);
extern uint16_t drv_console_read(void);

#endif

