#ifndef APP_LED_H
#define APP_LED_H

#include <stdint.h>
void led_on(uint8_t n);
void led_off(uint8_t n);
void led_reverse(uint8_t n);
void led_breathing_light(uint8_t n);
int8_t led_status(uint8_t n);

void led_init(void);
void task_led(void);
    
#endif
