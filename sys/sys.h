#ifndef SYS_H
#define SYS_H

#include <stdint.h>
#include <stdio.h>
#include "sys_api.h"
#include "sys_delay.h"



extern volatile uint32_t sys_tick;
extern volatile uint8_t sys_speed_flag[];
extern void sys_init(void);
extern void sys_start(void);

#endif
