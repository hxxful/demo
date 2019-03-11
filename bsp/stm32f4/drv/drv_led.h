#ifndef DRV_LED_H
#define DRV_LED_H

#include <stdint.h>

void drv_led_init(void);
int8_t drv_led_write(uint8_t n,uint8_t val);
int8_t drv_led_read(uint8_t n);

#endif
