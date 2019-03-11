#include "sys_api.h"

void (*delay_us)(uint32_t);
void (*delay_ms)(uint16_t);

int8_t (*led_write)(uint8_t n, uint8_t cmd);
int8_t (*led_read)(uint8_t);

void (*dev_max7456_init)(void);
void (*max7456_connect)(void);
void (*max7456_disconnect)(void);
int8_t (*max7456_write)(uint8_t);
uint8_t (*max7456_read)(void);


void (*w25qxx_connect)(void);
void (*w25qxx_disconnect)(void);
uint8_t (*w25qxx_read)(void);
uint8_t (*w25qxx_write)(uint8_t);


int8_t (*frame_read)(uint8_t*, uint8_t, uint8_t);
int8_t (*frame_write)(uint8_t*, uint8_t, uint8_t);



void sys_api_init(void)
{
	
}
