#ifndef DRV_USART_H
#define DRV_USART_H

#include <stdint.h>

#define TXBUFLIM 390
#define TXBUFCAP 400

extern uint8_t 	    txbuf[];		
extern uint32_t 	txpoint;		
extern uint32_t 	txtail;		
extern uint32_t 	arynum;


extern void drv_usart1_hardware_read(void);
extern void drv_usart1_hardware_write(uint8_t ch);
extern void drv_usart1_hardware_init(uint32_t baudrate);
extern void drv_irq_usart1_enable(void);
extern void drv_irq_usart1_disable(void);


#endif

