#include "sys.h"
#include "sys_api.h"
#include "drv_emmc.h"
#include "sys_console.h"
#include "app_emmc.h"

/*系统时基 2ms*/
volatile uint32_t sys_tick = 0;


void sys_init(void)
{   
    //sys_console_init();
    //sys_delay_init();
    //emmc_init(&emmc1);
}

void sys_start(void)
{
    
   // while(1) {
   //     printf("hello world\r\n");
   //     sys_delay_ms(1000);
   //}
    ucos_start();
}






