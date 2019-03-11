#ifndef DRV_W25QXX_H
#define DRV_W25QXX_H

#include <stdint.h>

uint8_t drv_w25qxx_write(uint8_t ch);
uint8_t drv_w25qxx_read(void);
void 	drv_w25qxx_connect(void);
void 	drv_w25qxx_disconnect(void);
void 	drv_w25qxx_init(void);


#endif
