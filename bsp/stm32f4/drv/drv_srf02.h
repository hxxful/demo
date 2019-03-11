#ifndef DRV_SRF02_H
#define DRV_SRF02_H

#include <stdint.h>
uint8_t drv_srf02_write(uint8_t ch);
uint8_t drv_srf02_read(void);
void drv_srf02_init(void);

#endif
