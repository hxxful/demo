#include "drv_common.h"
#include "drv_spi_hardware.h"
#include "drv_spi_software.h"
#include "drv_w25qxx.h"


#define	DRV_W25QXX_CS PAout(2)

void drv_w25qxx_init(void)
{
	/*PORTA时钟使能*/
	RCC->APB2ENR |= 1<<2;
	/*PA2.3.4通用推挽50MHz输出高*/
	/*PA3.4为挂接到SPI1总线上的其它设备片选*/
	GPIOA->CRL &= 0xFFF000FF;
	GPIOA->CRL |= 0x00033300;
	GPIOA->ODR |= 7<<2;
	/*SPI1初始化并设置速率18MHz*/
	//drv_spi1_hardware_init();
	/*IO模拟SPI1初始化*/
	drv_spi1_software_init();
}

void drv_w25qxx_connect(void)
{
	DRV_W25QXX_CS = 0;
}

void drv_w25qxx_disconnect(void)
{
	DRV_W25QXX_CS = 1;
}

uint8_t drv_w25qxx_write(uint8_t ch)
{
	//drv_spi1_hardware_write(ch);
	drv_spi1_software_write(ch);
	return 0;
}

uint8_t drv_w25qxx_read(void)
{
	//return drv_spi1_hardware_read();
	return drv_spi1_software_read();
}
