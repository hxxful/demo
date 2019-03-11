#ifndef DRV_PWM_H
#define DRV_PWM_H

#include <stdint.h>
void drv_pwm_init(void);
int8_t drv_pwm_write(uint32_t val,uint8_t cmd);
int8_t drv_pwm_read(uint8_t n);

#endif
