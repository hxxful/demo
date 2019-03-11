#ifndef DRV_I2C_H
#define DRV_I2C_H

#include <stdint.h>
void drv_i2c1_software_init(void);
void drv_i2c1_software_start(void);
void drv_i2c1_software_stop(void);
void drv_i2c1_software_ack(void);
uint8_t drv_i2c1_software_wait_ack(void);
void drv_i2c1_software_nack(void);
uint8_t drv_i2c1_software_write(uint8_t txd);
uint8_t drv_i2c1_software_read(uint8_t ack);



#endif

