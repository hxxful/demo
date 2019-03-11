#include "sys.h"
#include "drv_common.h"
#include "drv_timers.h"


#define drv_timer2_irq_func	TIM2_IRQHandler

void drv_timer2_init(void)
{
	/*开TIM2时钟*/
	RCC->APB1ENR |= 1 << 0;
	/*TIM2开始复位*/
	RCC->APB1RSTR |= 1 << 0;
	/*TIM2停止复位*/
	RCC->APB1RSTR &= ~(1 << 0);

	/*TIM2配置*/
	TIM2->CR1 = 0x0000;
	TIM2->ARR = 0x000007D0;
	TIM2->PSC = 0x0047;
	TIM2->RCR = 0x0000;
	TIM2->EGR = 0x0000;
	TIM2->DIER |= 1 << 0; 
	/*TIM2使能*/
	TIM2->CR1 |= 1 << 0;
	drv_nvic_init(2, 1, TIM2_IRQn, 2);
}


/*  2ms		* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  10ms	|		  |         |         |         |         |		    |         |         |         |         |         |
 *  50ms      |                                               |                                                   |
 *  100ms       |                                                                                                   |
 *  1s            |                                                                                                   |
 *
 *
 *
 */
void drv_timer2_irq_func(void)
{
	if(0 != (TIM2->SR & (uint16_t)0x0001)) {
		TIM2->SR &= (uint16_t)~0x0001;
	}

	sys_tick++;

	/*2ms*/
	sys_speed_flag[0] = 1;

	/*10ms*/
	if(1 == sys_tick % 5) {
		sys_speed_flag[1] = 1;
		return;
	}

	/*50ms*/
	if(2 == sys_tick % 25) {
		sys_speed_flag[2] = 1;
		return;
	} 	

	/*100ms*/
	if(3 == sys_tick % 50) {
		sys_speed_flag[3] = 1;
		return;
	}

	/*1s*/
	if(4 == sys_tick % 500) {
		sys_speed_flag[4] = 1;
		return;
	}
}
