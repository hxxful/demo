#ifndef APP_MAX7456_H
#define APP_MAX7456_H

#include <stdint.h>



void max7456_offset(int8_t x, int8_t y);
void max7456_learn_fonts(void);
void max7456_draw_allchar(void);
void max7456_init(void);
void task_max7456_draw_osd(void);
void task_max7456_get_data(void);
void task_max7456_time(void);
extern uint8_t debug_data;

#endif
