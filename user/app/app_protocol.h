#ifndef app_PROTOCOL_H
#define app_PROTOCOL_H

#include <stdint.h>
extern uint8_t frame_payload_size;
extern uint8_t frame_type;

/*无人机*/
#define FRAME_DRONE_STATUS 	0x01
#define FRAME_DRONE_GPSDATA	0x04
#define FRAME_DRONE_POWER	0x05
#define FRAME_DRONE_SENSOR2	0x07
#define FRAME_DRONE_SPEED	0x0B

#pragma pack(1)
typedef struct {
	int16_t roll;
	int16_t pitch;
	int16_t yaw;
	int32_t alt;
	uint8_t fly_mode;
	uint8_t armed; 
	uint8_t fun_mode;
	/*signal intensity 信号强度*/
	uint8_t siin;
} DRONE_STATUS_T;
#pragma pack()

#pragma pack(1)
typedef struct {
	uint8_t status;
	uint8_t gps_num;
	int32_t lng;
	int32_t lat;
	int16_t rth_degree;
	uint16_t rth_distance; 
} DRONE_GPSDATA_T;
#pragma pack()

#pragma pack(1)
typedef struct {
	uint16_t voltage;
	uint16_t current;
	uint8_t battery;
	uint8_t battery_alarm;
} DRONE_POWER_T;
#pragma pack()

#pragma pack(1)
typedef struct {
	int32_t alt_bar;
	int32_t alt_addition;
} DRONE_SENSOR2_T;
#pragma pack()

#pragma pack(1)
typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} DRONE_SPEED_T;
#pragma pack()

/*舵机*/
#pragma pack(1)
typedef struct {
	uint8_t degree;
}MOTOR_DEGREE_T;
#pragma pack()

extern DRONE_STATUS_T     drone_status;
extern DRONE_GPSDATA_T    drone_gpsdata;
extern DRONE_POWER_T      drone_power;
extern DRONE_SENSOR2_T    drone_sensor2;
extern DRONE_SPEED_T      drone_speed;

extern MOTOR_DEGREE_T motor_degree;

extern uint8_t frame_recv_buf[];
extern uint8_t frame_send_buf[];
extern uint8_t frame_valid_flag;

void protocol_frame_parse(uint8_t ch);
void protocol_frame_pack(uint8_t* buf, uint8_t type, uint8_t size);
int8_t protocol_frame_read(uint8_t* buf, uint8_t type, uint8_t size);
int8_t protocol_frame_write(uint8_t* buf, uint8_t type, uint8_t size);
void byte_reverse(uint8_t *src, uint8_t *dst, uint8_t size, uint8_t enable);

#endif
