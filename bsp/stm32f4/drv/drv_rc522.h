#ifndef DRV_RC522_H
#define DRV_RC522_H

#include <stdint.h>

uint8_t drv_rc522_write(uint8_t ch);
uint8_t drv_rc522_read(void);
void 	drv_rc522_connect(void);
void 	drv_rc522_disconnect(void);
void 	drv_rc522_init(void);
#endif
