#ifndef DRV_DMA_H
#define DRV_DMA_H

#include <stdint.h>
void drv_dma1_init(void);
int8_t drv_dma1_write(uint8_t* buf, uint32_t size);
#endif
