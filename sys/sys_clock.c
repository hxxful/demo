#include "sys.h"
#include "sys_clock.h"
#include "drv_clock.h"

void sys_clock_init(void)
{
    drv_clock_inside();
}
