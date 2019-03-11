#include "drv_common.h"
#include "drv_spi_software.h"

/*SPI1引脚输入输出控制*/
#define SPI1_CLK          PAout(5)
#define SPI1_MISO         PAin(6)
#define SPI1_MOSI         PAout(7)

void drv_spi1_software_delay(void);

void drv_spi1_software_init(void)
{
	/*PORTA时钟使能*/
	RCC->APB2ENR |= 1<<2;

	/*PA5.7通用推挽50MHz输出高*/
	/*PA6输入*/
	GPIOA->CRL &= 0x000FFFFF;
	GPIOA->CRL |= 0x38300000;
	GPIOA->ODR |= 7<<5;
}

void drv_spi1_software_write(uint8_t ch)
{
    uint8_t i;

    for(i = 0; i < 8; i++) {
        SPI1_CLK = 0;
        drv_spi1_software_delay();

        if(ch & 0x80) {
            SPI1_MOSI = 1;
        } else {
            SPI1_MOSI = 0;
        }
        ch <<= 1;

        drv_spi1_software_delay();
        SPI1_CLK = 1;
        drv_spi1_software_delay();
    }

}

uint8_t drv_spi1_software_read(void)
{
    uint8_t i;
    uint8_t ch = 0;
  
    for(i = 0; i < 8; i++) {
        SPI1_CLK = 0;
        drv_spi1_software_delay();

        ch <<= 1;
        if(SPI1_MISO) {
            ch |= 0x01;
        }

        SPI1_CLK = 1;
        drv_spi1_software_delay();
    }
    return ch;
}

void drv_spi1_software_delay(void)
{
    uint16_t i = 5;
    while(i--);
}

