#include "sys.h"

#include "drv_common.h"
#include "drv_i2c_software.h"

#define I2C1_SCL        PCout(12)
#define I2C1_SDA        PCout(11)
#define I2C1_READ_SDA   PCin(11)

#define I2C1_SDA_IN()  {GPIOC->CRH &= 0XFFFF0FFF; GPIOC->CRH |= 0x00008000;}
#define I2C1_SDA_OUT() {GPIOC->CRH &= 0XFFFF0FFF; GPIOC->CRH |= 0x00003000;}

void drv_i2c1_software_init(void)
{
	/*PORTC时钟使能*/
	RCC->APB2ENR |= 1<<4;

	/*PC11.12通用推挽50MHz输出高*/
	GPIOC->CRH &= 0xFFF00FFF;
	GPIOC->CRH |= 0x00033000;
	GPIOC->ODR |= 3<<11;
	
	I2C1_SCL = 1;
	I2C1_SDA = 1;
}

void drv_i2c1_software_start(void)
{
	I2C1_SDA_OUT();
	I2C1_SDA = 1;
	I2C1_SCL = 1;
	delay_us(4);
	I2C1_SDA = 0;
	delay_us(4);
	I2C1_SCL = 0;
}

void drv_i2c1_software_stop(void)
{
	I2C1_SDA_OUT();
	I2C1_SCL = 0;
	I2C1_SDA = 0;
	delay_us(4);
	I2C1_SCL = 1;
	I2C1_SDA = 1;
	delay_us(4);
}

void drv_i2c1_software_ack(void)
{
	I2C1_SCL = 0;
	I2C1_SDA_OUT();
	I2C1_SDA = 0;
	delay_us(2);
	I2C1_SCL = 1;
	delay_us(2);
	I2C1_SCL = 0;
}

uint8_t drv_i2c1_software_wait_ack(void)
{
	uint8_t ucErrTime = 0;
	I2C1_SDA_IN();
	I2C1_SDA = 1;
	delay_us(1);
	I2C1_SCL = 1;
	delay_us(1);
	while(I2C1_READ_SDA) {
		ucErrTime++;
		if(ucErrTime>250) {
			drv_i2c1_software_stop();
			return 1;
		}
	}
	I2C1_SCL = 0;
	return 0;
}

void drv_i2c1_software_nack(void)
{
	I2C1_SCL = 0;
	I2C1_SDA_OUT();
	I2C1_SDA = 1;
	delay_us(2);
	I2C1_SCL = 1;
	delay_us(2);
	I2C1_SCL = 0;
}

uint8_t drv_i2c1_software_write(uint8_t txd)
{
	uint8_t t;
	I2C1_SDA_OUT();
	I2C1_SCL = 0;
	for(t=0; t<8; t++) {
		I2C1_SDA = (txd & 0x80) >> 7;
		txd <<= 1;
		delay_us(2);
		I2C1_SCL = 1;
		delay_us(2);
		I2C1_SCL = 0;
		delay_us(2);
	}
	return 0;
}


uint8_t drv_i2c1_software_read(uint8_t ack)
{
	uint8_t i,receive = 0;
	I2C1_SDA_IN();
	for(i = 0; i < 8; i++ ) {
		I2C1_SCL = 0;
		delay_us(2);
		I2C1_SCL = 1;
		receive <<= 1;
		if(I2C1_READ_SDA) {
			receive++;
		}
		delay_us(1);
	}
	if (!ack)
		drv_i2c1_software_nack();
	else
		drv_i2c1_software_ack();
	return receive;
}



