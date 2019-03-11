#include "drv_common.h"
#include "drv_led.h"

#define MAX_NUM_LED	3


#define LED0 PAout(8)
#define LED1 PDout(2)
#define LED2 PCout(13)

void drv_led_init(void)
{
	/*使能PortA时钟*/
	RCC->APB2ENR |= 1<<2;
	
	/*使能PortB时钟*/
	//RCC->APB2ENR |= 1<<3;
	
	/*使能PortC时钟*/
	//RCC->APB2ENR |= 1<<4;
	
	/*使能PortD时钟*/
	RCC->APB2ENR |= 1<<5;
	
	/*使能PortE时钟*/
	//RCC->APB2ENR |= 1<<6;

	/*PC13通用推挽50MHz输出高*/
	//GPIOC->CRH &= 0XFF0FFFFF;
	//GPIOC->CRH |= 0X00300000;	
	//GPIOC->ODR |= 1<<13;
	
	/*PA8通用推挽50MHz输出高*/
	GPIOA->CRH &= 0XFFFFFFF0;
	GPIOA->CRH |= 0X00000003;
	GPIOA->ODR |= 1<<8;

	/*PD2通用推挽50MHz输出高*/
	GPIOD->CRL &= 0XFFFFF0FF;
	GPIOD->CRL |= 0X00000300;
	GPIOD->ODR |= 1<<2;
}

int8_t drv_led_write(uint8_t n, uint8_t cmd)
{
    if(1 < cmd) {
        return -1;
    }

    if(MAX_NUM_LED < n) {
        return -2;
    }

    switch(n) {
	case 0:
		LED0 = cmd;
		break;
	case 1:
		LED1 = cmd;
		break;
	case 2:
		LED2 = cmd;
		break;
	default:
		break;
    }

    return 0;
}

int8_t drv_led_read(uint8_t n)
{
	uint8_t res = 0x00;

	if(MAX_NUM_LED < n) {
		return -1;
	}

	switch(n) {
		case 0:
			res = LED0;
			break;
		case 1:
			res = LED1;
			break;
		case 2:
			res = LED2;
			break;
		default:
			break;
	}
	
	return (int8_t)res;
}


