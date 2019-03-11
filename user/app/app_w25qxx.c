#include "sys.h"
#include "app_w25qxx.h"

#define W25Q80  0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16

#define FLASH_ID  W25Q64

#define W25X_WRITEENABLE            0x06 
#define W25X_WRITEDISABLE           0x04 
#define W25X_READSTATUSREG          0x05 
#define W25X_WRITESTATUSREG         0x01 
#define W25X_READDATA               0x03 
#define W25X_FASTREADDATA           0x0B 
#define W25X_FASTREADDUAL           0x3B 
#define W25X_PAGEPROGRAM            0x02 
#define W25X_BLOCKERASE             0xD8 
#define W25X_SECTORERASE            0x20 
#define W25X_CHIPERASE              0xC7 
#define W25X_POWERDOWN              0xB9 
#define W25X_RELEASEPOWERDOWN       0xAB 
#define W25X_DEVICEID               0xAB 
#define W25X_MANUFACTDEVICEID       0x90 
#define W25X_JEDECDEVICEID          0x9F

uint8_t w25qxx_read_sr(void)   
{  
	uint8_t byte = 0;
	w25qxx_connect();
	w25qxx_write(W25X_READSTATUSREG);
	byte = w25qxx_read();
	w25qxx_disconnect();
	return byte;   
}

void w25qxx_wait_busy(void)   
{   
	while (0x01 == (w25qxx_read_sr() & 0x01));
} 

void w25qxx_write_enable(void)   
{
	w25qxx_connect();
	w25qxx_write(W25X_WRITEENABLE);
	w25qxx_disconnect();
} 

void w25qxx_write_disable(void)
{
	w25qxx_connect();
	w25qxx_write(W25X_WRITEDISABLE);
	w25qxx_disconnect();     
}

void w25qxx_read_flash(uint8_t* buf, uint32_t addr, uint16_t size)   
{ 
	uint16_t i = 0;
	w25qxx_connect();
	w25qxx_write(W25X_READDATA);
	w25qxx_write((uint8_t)(addr >> 16));
	w25qxx_write((uint8_t)(addr >>  8));
	w25qxx_write((uint8_t)addr);
	for(i = 0; i < size; i++)
		buf[i] = w25qxx_read();
	w25qxx_disconnect();
}

uint16_t w25qxx_read_id(void)
{
	uint16_t tmp = 0;
	w25qxx_connect();
	w25qxx_write(0x90);
	w25qxx_write(0x00);
	w25qxx_write(0x00);
	w25qxx_write(0x00);
	tmp |= w25qxx_read() << 8;
	tmp |= w25qxx_read();
	w25qxx_disconnect();
	return tmp;
}

void drv_w25qxx_erase_sector(uint32_t addr)   
{   
	addr *= 4096;
	w25qxx_write_enable();
	w25qxx_wait_busy();
	w25qxx_connect();
	w25qxx_write(W25X_SECTORERASE);
	w25qxx_write((uint8_t)((addr) >> 16));
	w25qxx_write((uint8_t)((addr) >>  8));
	w25qxx_write((uint8_t)addr);
	w25qxx_disconnect();
	w25qxx_wait_busy();
}

void drv_w25qxx_erase_chip(void)
{
	w25qxx_write_enable();
	w25qxx_wait_busy();
	w25qxx_connect();
	w25qxx_write(W25X_CHIPERASE);
	w25qxx_disconnect();
	w25qxx_wait_busy();
}

void w25qxx_write_page(uint8_t* buf,uint32_t addr,uint16_t size)
{
	uint16_t i;  
	w25qxx_write_enable();
	w25qxx_connect();
	w25qxx_write(W25X_PAGEPROGRAM);
	w25qxx_write((uint8_t)((addr) >> 16));
	w25qxx_write((uint8_t)((addr) >>  8));
	w25qxx_write((uint8_t) addr);
	for( i = 0; i < size; i++) {
		w25qxx_write(buf[i]);
	}
	w25qxx_disconnect();
	w25qxx_wait_busy();
} 

void w25qxx_write_nocheck(uint8_t* buf,uint32_t addr,uint16_t size)   
{ 			 		 
	uint16_t pageremain;
	pageremain = 256 - addr % 256;
	if(size <= pageremain) {
		pageremain = size;
	}
	while(1) {
		w25qxx_write_page(buf, addr, pageremain);
		if(size == pageremain) {
			break;
		} else {
			buf += pageremain;
			addr += pageremain;

			size -= pageremain;
			if(size > 256) {
				pageremain = 256;
			} else {
				pageremain = size;
			}
		}
	}	    
}

uint8_t spi_flash_buf[4096];
void w25qxx_write_flash(uint8_t* buf, uint32_t addr, uint16_t size)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;
 	uint16_t i;

	secpos = addr / 4096;
	secoff = addr % 4096;
	secremain = 4096 - secoff;

	if(size <= secremain){
		secremain = size;
	}
	while(1)
	{
		w25qxx_read_flash(spi_flash_buf, secpos * 4096, 4096);
		for(i = 0; i < secremain; i++)
		{
			if(0xff != spi_flash_buf[secoff+i]) {
				break;
			}
		}
		if(i < secremain) {
			drv_w25qxx_erase_sector(secpos);
			for(i = 0;i < secremain; i++) {
				spi_flash_buf[i + secoff] = buf[i];
			}
			w25qxx_write_nocheck(spi_flash_buf, secpos * 4096, 4096);
		}else{
			w25qxx_write_nocheck(buf, addr, secremain);
		}
		if(size == secremain){
			break;
		}else{
			secpos++;
			secoff = 0;

		   	buf  += secremain;
			addr += secremain;
		   	size -= secremain;
			if(size > 4096) {
				secremain = 4096;
			} else {
				secremain = size;
			}
		}
	}	 	 
}

void w25qxx_init(void)
{
	uint16_t res = w25qxx_read_id();
	printf("[OK]w25qxx_init		id:%d\n", res);
}

void task_w25qxx(void)
{

}

