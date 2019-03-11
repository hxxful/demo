#include "sys.h"

#include "drv_common.h"
#include "drv_at24cxx.h"
#include "drv_i2c_software.h"

#define AT24C01    127
#define AT24C02    255
#define AT24C04    511
#define AT24C08    1023
#define AT24C16    2047
#define AT24C32    4095
#define AT24C64    8191
#define AT24C128   16383
#define AT24C256   32767

#define EE_TYPE AT24C02

void drv_at24cxx_init(void)
{
	drv_i2c1_software_init();
}

/*读一个字节*/
int8_t drv_at24cxx_read(uint16_t addr)
{
	uint8_t tmp = 0;
	drv_i2c1_software_start();
	if(EE_TYPE > AT24C16) {
		drv_i2c1_software_write(0XA0);
		drv_i2c1_software_wait_ack();
		drv_i2c1_software_write(addr >> 8);
		drv_i2c1_software_wait_ack();
	} else {
		drv_i2c1_software_write(0XA0 + ((addr / 256) << 1));
	}

	drv_i2c1_software_wait_ack();
	drv_i2c1_software_write(addr % 256);
	drv_i2c1_software_wait_ack();
	drv_i2c1_software_start();
	drv_i2c1_software_write(0XA1);
	drv_i2c1_software_wait_ack();
	tmp = drv_i2c1_software_read(0);
	drv_i2c1_software_stop();
	return tmp;
}

/*写一个字节*/
int8_t drv_at24cxx_write(uint16_t addr,uint8_t data)
{
	drv_i2c1_software_start();
	if(EE_TYPE > AT24C16) {
		drv_i2c1_software_write(0XA0);
		drv_i2c1_software_wait_ack();
		drv_i2c1_software_write(addr >> 8);
	} else {
		drv_i2c1_software_write(0XA0+((addr/256)<<1));
	}
	drv_i2c1_software_wait_ack();
	drv_i2c1_software_write(addr % 256);
	drv_i2c1_software_wait_ack();
	drv_i2c1_software_write(data);
	drv_i2c1_software_wait_ack();
	drv_i2c1_software_stop();
	delay_ms(10);
	return 0;
}


