#include <stdio.h>
#include "drv_console.h"
#include "sys_console.h"

void sys_console_init()
{
    drv_console_init();
}

int fputc(int ch, FILE *f)
{
    ch &= 0xFF;
    drv_console_write((uint16_t)ch);
    return (int)ch;
}

int fgetc(FILE *f)
{
	return (int)drv_console_read();
}

#pragma import(__use_no_semihosting)
struct __FILE {
    int handle;
};
FILE __stdout;

void _sys_exit(int x)
{
    x = x;
}
