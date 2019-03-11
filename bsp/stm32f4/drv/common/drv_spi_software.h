#ifndef DRV_SPI_SOFTWARE_H
#define DRV_SPI_SOFTWARE_H

#include <stdint.h>
void drv_spi1_software_init(void);
void drv_spi1_software_write(uint8_t ch);	
uint8_t drv_spi1_software_read(void);


#endif
