#include "drv_common.h"
#include "drv_spi_hardware.h"
#include "drv_spi_software.h"
#include "drv_rc522.h"

#define	DRV_RC522_CS PAout(2)
void drv_rc522_init(void)
{
	/*PORTA时钟使能*/
	RCC->APB2ENR |= 1<<2;
	/*PA2通用推挽50MHz输出高*/
	GPIOA->CRL &= 0xFFFFF0FF;
	GPIOA->CRL |= 0x00000300;
	GPIOA->ODR |= 7<<2;
	/*SPI1初始化并设置速率18MHz*/
	//drv_spi1_hardware_init();
	/*IO模拟SPI1初始化*/
	drv_spi1_software_init();
}

void drv_rc522_connect(void)
{
	DRV_RC522_CS = 0;
}

void drv_rc522_disconnect(void)
{
	DRV_RC522_CS = 1;
}

uint8_t drv_rc522_write(uint8_t ch)
{
	//drv_spi1_hardware_write(ch);
	drv_spi1_software_write(ch);
	return 0;
}

uint8_t drv_rc522_read(void)
{
	//return drv_spi1_hardware_read();
	return drv_spi1_software_read();
}
