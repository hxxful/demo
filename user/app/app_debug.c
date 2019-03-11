#include <stdio.h>
#include "sys.h"
#include "app_debug.h"
#include "app_max7456.h"
#include "app_protocol.h"


void max7456_debug_read_frame_status(void)
{
	static uint8_t start_cnt = 0;
	if(0 == start_cnt) {
		printf("roll	pitch	yaw	alt	mode	armed	funy	siin\n");
		printf("**************************STATUS***********************************\n");
		start_cnt++;
	}
	if(0 < start_cnt) {
		printf("%d	%d	%d	%d	%d 	%d	%d	%d\n", \
			drone_status.roll/100, \
			drone_status.pitch/100, \
			drone_status.yaw/100, \
			drone_status.alt, \
			drone_status.fly_mode, \
			drone_status.armed, \
			drone_status.fun_mode, \
			drone_status.siin
		);		
        
        start_cnt++;
        if(12 == start_cnt) {
            start_cnt = 0;
            printf("\n\n");
        }

    }
}

void max7456_debug_read_frame_gpsdata(void)
{
	static uint8_t start_cnt = 0;
	if(0 == start_cnt) {
		printf("status	gps_num	lng	alt	rth_degree	rth_distance\n");
		printf("**************************GPSDATA***********************************\n");
		start_cnt++;
	}
	if(0 < start_cnt) {
		printf("%d	%d	%d	%d	%d 		%d\n", \
			drone_gpsdata.status, \
			drone_gpsdata.gps_num, \
			drone_gpsdata.lng, \
			drone_gpsdata.lat, \
			drone_gpsdata.rth_degree, \
			drone_gpsdata.rth_distance
		);		
        
        start_cnt++;
        if(12 == start_cnt) {
            start_cnt = 0;
            printf("\n\n");
        }

    }
}

void max7456_debug_read_frame_power(void)
{
	static uint8_t start_cnt = 0;
	if(0 == start_cnt) {
		printf("voltage		current		battery		battery_alarm\n");
		printf("****SPEED********* cm/s\n");
		start_cnt++;
	}
	if(0 < start_cnt) {
		printf("%d		%d		%d		%d\n", drone_power.voltage, drone_power.current, drone_power.battery, drone_power.battery_alarm);		
        
        start_cnt++;
        if(12 == start_cnt) {
            start_cnt = 0;
            printf("\n\n");
        }
    }
}

void max7456_debug_read_frame_sensor2(void)
{
	static uint8_t start_cnt = 0;
	if(0 == start_cnt) {
		printf("alt_bar	alt_addition\n");
		printf("****SENSOR2********* cm\n");
		start_cnt++;
	}
	if(0 < start_cnt) {
		printf("%d	%d\n", drone_sensor2.alt_bar, drone_sensor2.alt_addition);		
        
        start_cnt++;
        if(12 == start_cnt) {
            start_cnt = 0;
            printf("\n\n");
        }

    }
}

void max7456_debug_read_frame_speed(void)
{
	static uint8_t start_cnt = 0;
	if(0 == start_cnt) {
		printf("x	y	z\n");
		printf("****SPEED********* cm/s\n");
		start_cnt++;
	}
	if(0 < start_cnt) {
		printf("%d	%d	%d\n", drone_speed.x, drone_speed.y, drone_speed.z);		
        
        start_cnt++;
        if(12 == start_cnt) {
            start_cnt = 0;
            printf("\n\n");
        }

    }
}



void max7456_debug_read_frame(void)
{
	uint8_t buf[2] = {0};
	buf[0] = drone_status.roll >> 8;
	buf[1] = drone_status.roll;
	printf("%02X ", buf[0]);
	printf("%02X\n", buf[1]);
	//max7456_debug_read_frame_status();
	//max7456_debug_read_frame_gpsdata();
	//max7456_debug_read_frame_power();
	//max7456_debug_read_frame_sensor2();
	//max7456_debug_read_frame_speed();	
}

void max7456_debug_write_frame(void)
{
	uint16_t i = 0;
	
	drone_status.roll = 12000;
	drone_status.pitch = 200;
	drone_status.yaw = 156;
	drone_status.alt = 50;
	drone_status.fly_mode = 50;
	drone_status.armed = 1;
	drone_status.fun_mode = 1;
	drone_status.siin = 1;
	
	protocol_frame_write((uint8_t*)&drone_status, 0x01, sizeof(drone_status));
	for(i = 0; i < sizeof(drone_status) + 5; i++) {
		printf("%02X", frame_send_buf[i]);
	}
	printf("\n");
	
}

void max7456_debug(void)
{
	//max7456_debug_write_frame();
	max7456_debug_read_frame();
	
}

void task_debug(void)
{
	max7456_debug();
    //printf("debug_data:%d\n", debug_data);
   
}
