#ifndef APP_W25QXX_H
#define APP_W25QXX_H

#include <stdint.h>
uint16_t w25qxx_read_id(void);
uint8_t w25qxx_read_sr(void);
void w25qxx_write_enable(void);
void w25qxx_write_disable(void);
void w25qxx_read_flash(uint8_t* buf, uint32_t addr, uint16_t size);
void w25qxx_write_flash(uint8_t* buf, uint32_t addr, uint16_t size);
void w25qxx_erase_chip(void);
void w25qxx_erase_sector(uint32_t addr);
void w25qxx_wait_busy(void);

void w25qxx_write_page(uint8_t* buf,uint32_t addr,uint16_t size);
void w25qxx_write_nocheck(uint8_t* buf,uint32_t addr,uint16_t size);

void w25qxx_init(void);
void task_w25qxx(void);

#endif
