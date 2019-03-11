#ifndef DRV_MAX7456_H
#define DRV_MAX7456_H

#include <stdint.h>
int8_t drv_max7456_write(uint8_t ch);
uint8_t drv_max7456_read(void);
void 	drv_max7456_connect(void);
void 	drv_max7456_disconnect(void);
void 	drv_max7456_init(void);

#endif
