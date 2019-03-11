#ifndef APP_RC522_H
#define APP_RC522_H

#include <stdint.h>
uint16_t rc522_read_id(void);
uint8_t rc522_read_sr(void);
void rc522_write_enable(void);
void rc522_write_disable(void);
void rc522_read_flash(uint8_t* buf, uint32_t addr, uint16_t size);
void rc522_write_flash(uint8_t* buf, uint32_t addr, uint16_t size);
void rc522_erase_chip(void);
void rc522_erase_sector(uint32_t addr);
void rc522_wait_busy(void);

void rc522_write_page(uint8_t* buf,uint32_t addr,uint16_t size);
void rc522_write_nocheck(uint8_t* buf,uint32_t addr,uint16_t size);

void rc522_init(void);
void task_rc522(void);

#endif
