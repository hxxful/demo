#ifndef SYS_DELAY_H
#define SYS_DELAY_H

#include <stdint.h>
extern void sys_delay_init(void);

extern void sys_delay_us(uint32_t n);
extern void sys_delay_ms(uint32_t n);
extern void sys_delay_s(uint32_t n);
extern uint64_t sys_time_get_us(void);
extern uint64_t sys_time_get_ms(void);

#endif

