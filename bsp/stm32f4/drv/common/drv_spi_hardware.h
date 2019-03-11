#ifndef DRV_SPI_HARDWARE_H
#define DRV_SPI_HARDWARE_H

#include <stdint.h>
void drv_spi1_hardware_init(void);
void drv_spi1_hardware_write(uint8_t ch);	
uint8_t drv_spi1_hardware_read(void);


#endif
