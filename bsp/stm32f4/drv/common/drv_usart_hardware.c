#include "app_protocol.h"
#include "drv_common.h"
#include "drv_usart_hardware.h"

#define USART_WORDLENGTH            ((uint32_t)0x00000000)
#define USART_PARITY                ((uint32_t)0x00000000)
#define USART_MODE                  USART_CR1_RE | USART_CR1_TE
#define USART_HARDWAREFLOWCONTROL   ((uint32_t)0x00000000)

#define CR3_CLEAR_MASK   ((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE))
#define CR1_CLEAR_MASK   ((uint32_t)(USART_CR1_M | USART_CR1_PCE | \
                                     USART_CR1_PS | USART_CR1_TE | \
                                     USART_CR1_RE))
#define drv_irq_usart1_func          USART1_IRQHandler

uint8_t		txbuf[TXBUFCAP + 5];
uint32_t 	txpoint = 0;
uint32_t 	txtail = 0;
uint32_t 	arynum = 0;

void drv_usart1_hardware_init(uint32_t baudrate)
{       
	uint32_t pclk2 = 72;
    	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000) / (baudrate * 16);
	mantissa=temp;
	fraction=(temp-mantissa)*16;
	mantissa<<=4;
	mantissa+=fraction;
	
	RCC->APB2ENR |= 1<<2;
	RCC->APB2ENR |= 1<<14;
	GPIOA->CRH &= 0xFFFFF00F; 
	GPIOA->CRH |= 0x000008B0;
		  
	RCC->APB2RSTR |= 1<<14;
	RCC->APB2RSTR &= ~(1<<14);
 	USART1->BRR = mantissa;
	USART1->CR1 |= 0x200C;

	USART1->CR1 |= 1<<8;
	USART1->CR1 |= 1<<5;
	
	drv_nvic_init(3, 1, USART1_IRQn, 2);
}

void drv_usart1_hardware_read(void)
{

}

void drv_usart1_hardware_write(uint8_t ch)
{
	USART1->DR = (ch & (uint16_t)0x01FF);
}

void drv_irq_usart1_func(void)
{
	if(USART1->SR & (1 << 6)) {
		USART1->SR &= ~(1 << 6);
		USART1->CR1 |= (1 << 6);
		if(0 != arynum) {
			USART1->DR = (uint8_t)txbuf[txpoint];
			if(txpoint < TXBUFCAP - 1) {
				txpoint++;
			} else {
				txpoint = 0;
			}
			arynum--;
			if(0 == arynum) {
				txtail = txpoint;
			}
		}
	}

	if(USART1->SR & (1 << 5)) {
		USART1->SR &= ~(1 << 5);
		//protocol_frame_parse(USART1->DR);
	}
}

void drv_irq_usart1_enable(void)
{
	USART1->CR1 |= 1 << 5;
	USART1->CR1 |= 1 << 6;
}

void drv_irq_usart1_disable(void)
{
	USART1->CR1 &= ~(1 << 5);
	USART1->CR1 &= ~(1 << 6);
}

