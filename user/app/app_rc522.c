#include "sys.h"
#include "app_rc522.h"

/*读寄存器*/
uint8_t rc522_read_reg(uint8_t addr)
{
	uint8_t data = 0;
	rc522_connect();
	data = rc522_read();
	rc522_disconnect();
	return data;
}

/*写寄存器*/
void rc522_write_reg(uint8_t addr, uint8_t data)
{
	rc522_connect();
	rc522_write(addr);
	rc522_write(data);
	rc522_disconnect();
}



void rc522_init(void)
{
	uint16_t res = rc522_read_id();
	printf("[OK]rc522_init		id:%d\n", res);
}

void task_rc522(void)
{

}
