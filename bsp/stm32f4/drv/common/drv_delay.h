#ifndef DRV_SYSTICK_H
#define DRV_SYSTICK_H

#include <stdint.h>
extern void drv_delay_init(uint8_t sysclk);
extern void drv_delay_us(uint16_t n);
extern void drv_delay_ms(uint16_t n);
extern void drv_delay_s(uint16_t n);

#endif
