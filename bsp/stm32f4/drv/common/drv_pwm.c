#include "drv_common.h"
#include "drv_pwm.h"

#define drv_pwm_irq_func TIM3_IRQHandler

/*pwm初始化 20ms一个周期-对应0-20000*/
void drv_pwm_init()
{

	/*1.PWM引脚配置*/
	
	/*AFIO时钟使能*/
	RCC->APB2ENR |= 1<<0;
	
	/*使能PA时钟*/
	RCC->APB2ENR |= 1<<2;
	/*PA6推挽输出高*/	
	GPIOA->CRL &= 0xF0FFFFFF;
	GPIOA->CRL |= 0x0B000000;
	GPIOA->ODR |= 1<<6;
	
	/*PA7推挽输出高*/	
	GPIOA->CRL &= 0x0FFFFFFF;
	GPIOA->CRL |= 0xB0000000;
	GPIOA->ODR |= 1<<7;
	
	
//	/*使能PA时钟*/
//	RCC->APB2ENR |= 1<<2;
//	/*PA8推挽输出高*/	
//	GPIOA->CRH &= 0xFFFFFFF0;
//	GPIOA->CRH |= 0x0000000B;
//	GPIOA->ODR |= 1<<8;
//	
//	/*使能PC时钟*/
//	RCC->APB2ENR |= 1<<4;
//	/*PC13推挽输出高*/	
//	GPIOC->CRH &= 0xFF0FFFFF;
//	GPIOC->CRH |= 0x00B00000;
//	GPIOC->ODR |= 1<<13;
	
	/*2.PWM定时器配置*/
	
	RCC->APB1ENR |= 1<<1;
	TIM3->CR1 = 0x0000;
	TIM3->ARR = 0x00004E1F;
	TIM3->PSC = 0x0047;
	TIM3->RCR = 0x0000;
	TIM3->EGR = 0x0000;
	TIM3->DIER |= 1 << 0; 
	TIM3->DIER |= 1 << 6;
	
	/*CH1-PWM模式1-使能*/
	TIM3->CCMR1 |= 6<<4;
	/*CH1预装载使能*/
	TIM3->CCMR1 |= 1<<3;
	/*OC1输出使能*/
	TIM3->CCER |= 1<<0;
	
	/*CH2-PWM模式1-使能*/
	TIM3->CCMR1 |= 6<<12;
	/*CH2预装载使能*/
	TIM3->CCMR1 |= 1<<11;
	/*OC2输出使能*/
	TIM3->CCER |= 1<<4;
	
	/*TIM自动重载使能*/
	//TIM3->CR1 |= 1<<7;
	/*TIM3使能*/
	TIM3->CR1 |= 1<<0;

	//drv_nvic_init(3, 2, TIM3_IRQn, 2);	
}

void drv_pwm_irq_func(void)
{
	if(0 != (TIM2->SR & (uint16_t)0x0001)) {
		TIM2->SR &= (uint16_t)~0x0001;
	}
}

int8_t drv_pwm_write(uint32_t val, uint8_t cmd)
{
	switch(cmd) {
		case 1:
			TIM3->CCR1 = val;
			break;
		case 2:
			TIM3->CCR2 = val;
			break;
		case 3:
			TIM3->CCR3 = val;
			break;
		case 4:
			TIM3->CCR4 = val;
			break;
		default:
			break;
	}
	return 0;
}

int8_t drv_pwm_read(uint8_t n)
{
	return 0;
}



