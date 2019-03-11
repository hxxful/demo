#include "drv_common.h"
#include "drv_max7456.h"
#include "drv_spi_hardware.h"
#include "drv_spi_software.h"
#include "drv_usart_hardware.h"

/*注意PORT_CLK调用函数也可能需要修改*/
#define PORT_CLK            RCC_AHBPeriph_GPIOA
#define PORT_MAX7456_CS     GPIOA
#define PIN_MAX7456_CS      GPIO_Pin_4
#define MAX7456_CS          PAout(4)

#define GPIOx               GPIOA
void drv_max7456_init(void)
{
    /*开GPIOA时钟*/
    RCC->AHBENR |= 0x00020000;
    
    /*模式 复用模式 引脚4*/
    GPIOx->MODER  &= 0xFFFFFCFF;
    GPIOx->MODER  |= 0x00000100; 
    /*输出 上拉输出 引脚4*/
    GPIOx->OTYPER &= 0xFFFFFFEF;
    /*速度 高速输出 引脚4*/
    GPIOA->OSPEEDR &= 0xFFFFFCFF;
    GPIOA->OSPEEDR |= 0x00000300;
    /*方式 推挽上拉 引脚4*/
    GPIOx->PUPDR &= 0xFFFFFCFF;
    GPIOx->PUPDR |= 0x00000100;
     
    drv_spi1_hardware_init();
	//drv_spi1_software_init();
}

void drv_max7456_connect(void)
{
    MAX7456_CS = 0;
}

void drv_max7456_disconnect(void)
{
    MAX7456_CS = 1;
}

int8_t drv_max7456_write(uint8_t ch)
{
    drv_irq_usart1_disable();
    //drv_spi1_software_write(ch);
    drv_spi1_hardware_write(ch);
    drv_irq_usart1_enable();
    return 0;
}

uint8_t drv_max7456_read(void)
{
    uint8_t res = 0;
    drv_irq_usart1_disable();
    //res = drv_spi1_software_read();
    res = drv_spi1_hardware_read();
    drv_irq_usart1_enable();
    return res;
}
