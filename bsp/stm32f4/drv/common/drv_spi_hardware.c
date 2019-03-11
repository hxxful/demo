#include "drv_common.h"
#include "drv_spi_hardware.h"

void drv_spi1_hardware_init(void)
{
	/*PORTA时钟使能*/
	RCC->APB2ENR |= 1<<2;
	/*SPI1时钟使能*/
	RCC->APB2ENR |= 1<<12;

	/*PA5.6.7复用推挽50MHz输出高*/
	GPIOA->CRL &= 0x000FFFFF;
	GPIOA->CRL |= 0xBBB00000;
	GPIOA->ODR |= 7<<5;

	/*全双工使能*/
	SPI1->CR1 &= ~(1<<10);
	/*软件NSS管理*/
	SPI1->CR1 |= 1<<9;
	SPI1->CR1 |= 1<<8;

	/*配置为主设备*/
	SPI1->CR1 |= 1<<2;
	/*8bit数据格式*/
	SPI1->CR1 &= ~(1<<11);
	/*时钟极性 高电平*/
	SPI1->CR1 |= 1<<1;
	/*时钟相位 第二个时钟边沿开始*/
	SPI1->CR1 |= 1<<0;
	/*先发送MSB*/
	SPI1->CR1 &= ~(1<<7);
	/*设置速度18MHz*/
	SPI1->CR1 &= ~(7<<3);
	SPI1->CR1 |= 1<<3;
	/*开启SPI设备*/
	SPI1->CR1 |= 1<<6;
	
}

static void spi_senddata8(SPI_TypeDef* SPIx, uint8_t data)
{
	SPIx->DR = data;
}

static uint8_t spi_receivedata8(SPI_TypeDef* SPIx)
{
	return SPIx->DR;
}

//static void spi_i2s_senddata16(SPI_TypeDef* SPIx, uint16_t data)
//{
//  SPIx->DR = (uint16_t)data;
//}

//uint16_t spi_i2s_receivedata16(SPI_TypeDef* SPIx)
//{
//  return SPIx->DR;
//}

uint8_t drv_spi1_hardware_read(void)
{
	uint8_t retry = 0;
	while (RESET == (SPI1->SR & SPI_SR_TXE)) {
		retry++;
		if(retry > 200) {
		    return 0;
		}
	}
	spi_senddata8(SPI1, 0x00);

	retry = 0;
	while (RESET == (SPI1->SR & SPI_SR_RXNE)) {
		retry++;
		if(retry > 200) {
		    return 0;
		}
	}
	return spi_receivedata8(SPI1);
}

void drv_spi1_hardware_write(uint8_t ch)
{
	uint8_t retry = 0;
	while (RESET == (SPI1->SR & SPI_SR_TXE)) {
		retry++;
		if(retry > 200) {
			return;
		}
	}
	spi_senddata8(SPI1, ch);

	retry = 0;
	while (RESET == (SPI1->SR & SPI_SR_RXNE)) {
		retry++;
		if(retry > 200) {
		    return;
		}
	}
	spi_receivedata8(SPI1);
}

