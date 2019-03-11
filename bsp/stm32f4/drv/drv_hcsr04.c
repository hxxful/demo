#include "drv_common.h"
#include "drv_systick.h"
#include "drv_hcsr04.h"

#define TRIG	PCout(4)
#define ECHO	PCin(5)
#define drv_hcsr04_irq_func	EXTI9_5_IRQHandler
#define drv_timer3_irq_func	TIM3_IRQHandler
/*hcsr04超声波模块初始化, 建议执行周期50ms以上*/
/*hcsr04默认最长200ms左右-34米左右,没有回应会自动拉低ECHO*/
uint32_t cnt_ms = 0;
uint32_t cnt_us = 0;
uint32_t cnt_t3 = 0;
uint32_t len = 0;
void drv_hcsr04_init(void)
{
	/*使能PortC时钟*/
	RCC->APB2ENR |= 1<<4;
	/*PC4通用推挽50MHz输出低*/
	/*PC5浮空输入*/
	GPIOC->CRL &= 0XFF00FFFF;
	GPIOC->CRL |= 0X00430000;
	GPIOC->ODR &= ~(1<<4);

	/*开TIM3时钟*/
	RCC->APB1ENR |= 1 << 1;
	/*TIM3开始复位*/
	RCC->APB1RSTR |= 1 << 1;
	/*TIM3停止复位*/
	RCC->APB1RSTR &= ~(1 << 1);
	/*TIM3配置*/
	TIM3->CR1 = 0x0000;
	/*自动重装载值 0x3E8 = 1000*/
	TIM3->ARR = 0x000003E8;
	/*预分频 0x47 = 71*/
	TIM3->PSC = 0x0047;
	TIM3->RCR = 0x0000;
	TIM3->EGR = 0x0000;
	/*使能中断*/
	TIM3->DIER |= 1 << 0;
	/*TIM3使能*/
	TIM3->CR1 |= 1 << 0;
	
	/*上升沿触发中断*/
	drv_ex_nvic_config(GPIO_C, 5, RTIR);
	/*下降沿触发中断*/
	drv_ex_nvic_config(GPIO_C, 5, FTIR);

	drv_nvic_init(3, 2, EXTI9_5_IRQn, 2);
	drv_nvic_init(3, 2, TIM3_IRQn, 2);
}

uint16_t drv_hcsr04_read(void)
{
	TRIG = 1;
	drv_systick_delay_us(15);
	TRIG = 0;
	return len;
}

void drv_hcsr04_irq_func(void)
{
	EXTI->PR = 1<<5;
	/*上升沿*/
	if(1 == ECHO) {
		cnt_ms = 0;
		cnt_us = 0;
		cnt_t3 = 0;
		TIM3->CNT = 0;
	/*下降沿*/
	} else {
		cnt_ms = cnt_t3;
		cnt_us = TIM3->CNT;
		cnt_us = cnt_ms * 1000 + cnt_us;
		len = (0.34 * cnt_us) / 2;
	}
	

}

/*1ms计时一次*/
void drv_timer3_irq_func(void)
{
	if(0 != (TIM3->SR & (uint16_t)0x0001)) {
		TIM3->SR &= (uint16_t)~0x0001;
		cnt_t3++;
	}
}


