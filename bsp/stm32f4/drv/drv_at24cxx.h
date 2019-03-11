#ifndef DRV_AT24CXX_H
#define DRV_AT24CXX_H

#include <stdint.h>
void 	drv_at24cxx_init(void);
int8_t drv_at24cxx_read(uint16_t addr);
int8_t drv_at24cxx_write(uint16_t addr, uint8_t data);

#endif
