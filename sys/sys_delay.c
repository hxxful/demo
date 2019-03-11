#include "sys.h"
#include "sys_delay.h"
#include "drv_delay.h"

void sys_delay_init(void)
{
    //drv_delay_init(180);
}

void sys_delay_us(uint32_t n)
{
    drv_delay_us(n);
}


void sys_delay_ms(uint32_t n)
{
    drv_delay_ms(n);
}

void sys_delay_s(uint32_t n)
{
    drv_delay_s(n);
}



uint64_t sys_time_get_us(void)
{
    CPU_TS32 ts = 0;
    CPU_INT64U us = 0;
    ts = CPU_TS_Get32();
    us = CPU_TS32_to_uSec(ts);
    return us;
}

uint64_t sys_time_get_ms(void)
{
    OS_ERR err;
    uint64_t res =0;
    res = OSTimeGet(&err);
    return res;
}
