#include "drv_common.h"


/*STM32F10x*/
void drv_nvic_setvectortable(uint32_t NVIC_VectTab, uint32_t Offset)
{
    SCB->VTOR = NVIC_VectTab|(Offset & (uint32_t)0x1FFFFF80);
}

void drv_nvic_prioritygroupconfig(uint8_t nvic_group)
{
    uint32_t tmp, tmp1;
    tmp1 = (~nvic_group) & 0x07;
    tmp1 <<= 8;
    tmp = SCB->AIRCR;
    tmp &= 0X0000F8FF;
    tmp |= 0X05FA0000;
    tmp |= tmp1;
    SCB->AIRCR = tmp;
}

void drv_nvic_init(uint8_t nvic_preemptionpriority, uint8_t nvic_subpriority, uint8_t nvic_channel, uint8_t nvic_group)
{
	uint32_t tmp;
	drv_nvic_prioritygroupconfig(nvic_group);
	tmp = nvic_preemptionpriority << (4 - nvic_group);
	tmp |= nvic_subpriority&(0x0f >> nvic_group);
	tmp &= 0xf;
	NVIC->ISER[nvic_channel / 32] |= (1 << nvic_channel % 32);
	NVIC->IP[nvic_channel] |= tmp << 4;
}

void drv_ex_nvic_config(uint8_t gpiox, uint8_t bitx, uint8_t trim)
{
    uint8_t extaddr;
    uint8_t extoffset;
    extaddr = bitx / 4;
    extoffset = (bitx % 4) * 4;
    RCC->APB2ENR |= 0x01;
    AFIO->EXTICR[extaddr] &= ~(0x000F << extoffset);
    AFIO->EXTICR[extaddr] |= gpiox << extoffset;
    EXTI->IMR |= 1 << bitx;
    //EXTI->EMR |= 1 << bitx;
    if(trim & 0x01)EXTI->FTSR |= 1 << bitx;
    if(trim & 0x02)EXTI->RTSR |= 1 << bitx;
}

void drv_rcc_deinit(void)
{
    RCC->APB1RSTR = 0x00000000;
    RCC->APB2RSTR = 0x00000000;

    RCC->AHBENR = 0x00000014;
    RCC->APB2ENR = 0x00000000;
    RCC->APB1ENR = 0x00000000;
    RCC->CR |= 0x00000001;
    RCC->CFGR &= 0xF8FF0000;
    RCC->CR &= 0xFEF6FFFF;
    RCC->CR &= 0xFFFBFFFF;
    RCC->CFGR &= 0xFF80FFFF;
    RCC->CIR = 0x00000000;
#ifdef VECT_TAB_RAM
    drv_nvic_setvectortable(0x20000000, 0x0);
#else
    drv_nvic_setvectortable(0x08000000, 0x0);
#endif
}
