#include <string.h>
#include <stdio.h>
#include "app_protocol.h"

#include "app_led.h"


#define FRAME_MAX_SIZE  50
#define FRAME_HEAD_SIZE 2
uint8_t frame_head[FRAME_HEAD_SIZE] = {0xAA, 0xAA};

uint8_t frame_recv_buf[FRAME_MAX_SIZE] = {0};
uint8_t frame_send_buf[FRAME_MAX_SIZE] = {0};
uint8_t frame_pointer = 0;
uint8_t frame_valid_flag = 0;
uint8_t frame_payload_size = 0;
uint8_t frame_type = 0;
uint8_t frame_check = 0;
uint8_t frame_calcu = 0;

DRONE_STATUS_T     drone_status;
DRONE_GPSDATA_T    drone_gpsdata;
DRONE_POWER_T      drone_power;
DRONE_SENSOR2_T    drone_sensor2;
DRONE_SPEED_T      drone_speed;

MOTOR_DEGREE_T motor_degree;



/*解包*/
/*可尝试用环形缓冲*/
static void protocol_frame_parse_head(uint8_t ch)
{
	if(frame_head[frame_pointer] == ch) {
		frame_recv_buf[frame_pointer] = ch;
		frame_calcu += ch;
		frame_pointer++;		
	} else {
		frame_calcu = 0;
		frame_pointer = 0;
		
	}
}

static void protocol_frame_pasre_body(uint8_t ch)
{    		
	if(FRAME_HEAD_SIZE == frame_pointer) {
		frame_type = ch;
	}

	if(FRAME_HEAD_SIZE + 1 == frame_pointer)
		frame_payload_size = ch;
	
	if(FRAME_HEAD_SIZE + 2 + frame_payload_size == frame_pointer) {		
		frame_recv_buf[frame_pointer] = ch;
		frame_check = frame_recv_buf[frame_pointer];
		if(frame_calcu == frame_check) {
			frame_valid_flag = 1;
		}
		frame_pointer = 0;
		frame_calcu = 0;
		frame_check = 0;
		return;
	}
	
	frame_calcu += ch;
	frame_recv_buf[frame_pointer] = ch;
	frame_pointer++;
}

void protocol_frame_parse(uint8_t ch)
{  
	if(frame_valid_flag == 0) {
		if(FRAME_HEAD_SIZE > frame_pointer)
			protocol_frame_parse_head(ch);
		else
			protocol_frame_pasre_body(ch);
	}
}

int8_t protocol_frame_read(uint8_t* buf, uint8_t type, uint8_t size)
{
        memcpy((void*)buf, (const void*)&frame_recv_buf[FRAME_HEAD_SIZE + 2], size);
	frame_valid_flag = 0;
	frame_type = 0;
	return 0;
}

/*大小端转换*/
void byte_reverse(uint8_t *src, uint8_t *dst, uint8_t size, uint8_t enable)
{	
	uint32_t i = 0;
	uint8_t buf[8] = {0};
	if(enable) {
		for(i = 0; i < size; i++)
			buf[size - 1 - i] = src[i];					
		for(i = 0; i < size; i++)
			dst[i] = buf[i];
	}
}
	














/*打包*/
void protocol_frame_pack_head(void)
{
	memcpy(frame_send_buf, frame_head, FRAME_HEAD_SIZE);
}

void protocol_frame_pack_body(uint8_t* buf, uint8_t type, uint8_t size)
{
    uint8_t i = 0;
    frame_send_buf[FRAME_HEAD_SIZE] = type;
    frame_send_buf[FRAME_HEAD_SIZE + 1] = size;
    if((FRAME_MAX_SIZE - 8) >= size) {
        for(i = 0; i < size; i++) {
            frame_send_buf[FRAME_HEAD_SIZE + 2 + i] = buf[i];
            frame_check += buf[i];
        } 
        frame_send_buf[FRAME_HEAD_SIZE + 2 + i] = frame_check;
    }
    frame_check = 0;
}

void protocol_frame_pack(uint8_t* buf, uint8_t type, uint8_t size)
{
    protocol_frame_pack_head();
    protocol_frame_pack_body(buf, type, size);
}

int8_t protocol_frame_write(uint8_t* buf, uint8_t type, uint8_t size)
{
    protocol_frame_pack(buf, type, size);
    return 0;
}
