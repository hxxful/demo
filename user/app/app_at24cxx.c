#include "sys.h"
#include "app_at24cxx.h"



void at24cxx_write_bytes(uint16_t addr, uint8_t *buf, uint8_t size)
{
	while(size--) {
		at24cxx_write(addr, *buf);
		addr++;
		buf++;
	}
}


void at24cxx_read_bytes(uint16_t addr, uint8_t *buf, uint8_t size)
{
	while(size--) {
		*buf = at24cxx_read(addr);
		addr++;
		buf++;
	}
}

void at24cxx_init(void)
{
	uint8_t buf[10] = {"1234567809"};
	at24cxx_write_bytes(0, buf, 10);
	printf("[OK]at24cxx_init	123456709\n");
}


void task_at24cxx(void)
{
	uint8_t buf[15] = {0};
	at24cxx_read_bytes(0, buf, 10);
	//printf("%s\n", buf);
}
