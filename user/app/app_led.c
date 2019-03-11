#include "sys.h"
#include "app_led.h"

uint32_t led_cnt = 0;
void led_on(uint8_t n)
{
    led_write(n, 1);
}

void led_off(uint8_t n)
{
	led_write(n, 0);
}

int8_t led_status(uint8_t n)
{
	return led_read(n);
}

void led_breathing_light(uint8_t n)
{

}

void led_reverse(uint8_t n)
{
	int8_t res = led_status(n);
	
	if(res){
		led_off(n);
	}else{
		led_on(n);
	}
}

void led_init(void)
{
	printf("[OK]led_init\n");
}

void task_led(void)
{
	led_reverse(0);
	led_cnt++;
}
