#include "sys.h"
#include "app_hcsr04.h"


void hcsr04_init(void)
{
	printf("[OK]hcsr04_init\n");
}

void task_hcsr04(void)
{
	double len = 0;
	len = (double)hcsr04_read() / 10;
	printf("len:%.1f cm\n", len);
}

