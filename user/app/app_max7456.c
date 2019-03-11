#include <stdio.h>
#include <string.h>
#include <math.h>
#include "sys.h"
#include "app_protocol.h"
#include "app_max7456.h"
#include "app_max7456_fonts.h"
#include "app_led.h"
#include "lib_math.h"


uint8_t debug_data = 0x00;
/*MAX7456相关配置*/
//#define USE_DMA_SPI_OSD
#define USE_PAL

/*MAX7456相关参数*/
#define VIDEO_BUFFER_CHARS_NTSC   390
#define VIDEO_BUFFER_CHARS_PAL    480
#define VIDEO_LINES_NTSC          13
#define VIDEO_LINES_PAL           16

#define MAX_CHAR2UPDATE 50
uint8_t screen_buf[480 + 20] = {0};
uint8_t shadow_buf[480] = {0};
uint8_t dmaspi_buf[MAX_CHAR2UPDATE * 6 + 20] = {0};
uint32_t radarchar[8] = {199, 200, 202, 203, 204, 205, 206, 207};

/*MAX7456 寄存器组*/
#define VM0           0X00 		/*视频模式0寄存器*/
#define VM1           0X01 		/*视频模式1寄存器*/
#define HOS           0X02 		/*水平位置寄存器*/
#define VOS           0X03 		/*垂直位置寄存器*/
#define DMM           0X04 		/*显示存储器模式寄存器*/
#define DMAH          0X05 		/*显示存储器地址高位寄存器*/
#define DMAL          0X06 		/*显示存储器地址低位寄存器*/
#define DMDI          0X07 		/*显示存储器数据输入寄存器*/
#define CMM           0X08 		/*字符存储器模式寄存器*/
#define CMAH          0X09 		/*字符存储器地址高位寄存器*/
#define CMAL          0X0A 		/*字符存储器地址低位寄存器*/
#define CMDI          0X0B 		/*字符存储器数据输入寄存器*/
#define OSDM          0X0C 		/*OSD插入复用寄存器*/
#define RB            0X10 		/* 第N行亮度寄存器，0-13  0-15 根据不同制式 0X10+RB0-15*/
#define OSDBL_W_ADD   0X6C 		/*OSD黑电平寄存器*/
#define OSDBL_R_ADD   0XEC 		/*读寄存器地址*/
#define STAT          0XA0 		/*状态寄存器(只读) 实际上可以读任何一个0XA0-AF*/
#define DMDO          0XB0 		/*显示存储器输出寄存器(只读)*/
#define CMDO          0XC0 		/*字符存储器输出寄存器(只读)*/

/*STAT 状态寄存器*/
#define STAT_PAL      0x01
#define STAT_NTSC     0x02
#define STAT_LOS      0x04
#define STAT_NVR_BUSY 0x20

/*CMM NVM访问模式寄存器*/
#define NVM_W 0XA0			  	/*NVM 处于可写模式*/
#define NVM_R 0X05				/*NVM 处于可读模式*/


//uint8_t ascii_remap[256] = {
//    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
//    0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
//    0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
//    0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
//    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31,
//    0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
//    0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,
//    0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
//    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
//    0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63,
//    0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
//    0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
//    0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81,
//    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B,
//    0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95,
//    0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
//    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9,
//    0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3,
//    0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD,
//    0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
//    0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1,
//    0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB,
//    0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5,
//    0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
//    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
//    0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
//};

void max7456_reset(void)
{

}

/*获取要显示的参数*/
void max7456_get_data(void)
{	
	if(1 != frame_valid_flag)
		return;
	switch(frame_type) {
		case FRAME_DRONE_STATUS:
			protocol_frame_read((uint8_t*)&drone_status, frame_type, sizeof(DRONE_STATUS_T));
			byte_reverse((uint8_t*)&drone_status.roll, (uint8_t*)&drone_status.roll, 2, 1);
			byte_reverse((uint8_t*)&drone_status.pitch, (uint8_t*)&drone_status.pitch, 2, 1);
			byte_reverse((uint8_t*)&drone_status.yaw, (uint8_t*)&drone_status.yaw, 2, 1);
			byte_reverse((uint8_t*)&drone_status.alt, (uint8_t*)&drone_status.alt, 4, 1);
			//printf("status: %.2f	%.2f	%.2f	%.2f\n", (double)drone_status.roll/100, (double)drone_status.pitch/100, (double)drone_status.yaw/100, (double)drone_status.alt);
			break;
		case FRAME_DRONE_GPSDATA:
			protocol_frame_read((uint8_t*)&drone_gpsdata, frame_type, sizeof(DRONE_GPSDATA_T));
			byte_reverse((uint8_t*)&drone_gpsdata.lng, (uint8_t*)&drone_gpsdata.lng, 4, 1);
			byte_reverse((uint8_t*)&drone_gpsdata.lat, (uint8_t*)&drone_gpsdata.lat, 4, 1);
			byte_reverse((uint8_t*)&drone_gpsdata.rth_degree, (uint8_t*)&drone_gpsdata.rth_degree, 2, 1);
			byte_reverse((uint8_t*)&drone_gpsdata.rth_distance, (uint8_t*)&drone_gpsdata.rth_distance, 2, 1);
			//printf("gpsdata: %f	%f	%f	%f\n", (double)drone_gpsdata.lng, (double)drone_gpsdata.lng, (double)drone_gpsdata.rth_degree, (double)drone_gpsdata.rth_distance);
			break;
		case FRAME_DRONE_POWER:
			protocol_frame_read((uint8_t*)&drone_power, frame_type, sizeof(DRONE_POWER_T));
			byte_reverse((uint8_t*)&drone_power.voltage, (uint8_t*)&drone_power.voltage, 2, 1);
			byte_reverse((uint8_t*)&drone_power.current, (uint8_t*)&drone_power.current, 2, 1);
			//printf("power: %f	%f	%d	%d\n", (double)drone_power.voltage/100, (double)drone_power.current/100, drone_power.battery, drone_power.battery_alarm);
			break;
		case FRAME_DRONE_SENSOR2:
			protocol_frame_read((uint8_t*)&drone_sensor2, frame_type, sizeof(DRONE_SENSOR2_T));
			byte_reverse((uint8_t*)&drone_sensor2.alt_bar, (uint8_t*)&drone_sensor2.alt_bar, 4, 1);
			byte_reverse((uint8_t*)&drone_sensor2.alt_addition, (uint8_t*)&drone_sensor2.alt_addition, 4, 1);
			//printf("sensor2: %f	%f\n", (double)drone_sensor2.alt_bar/100, (double)drone_sensor2.alt_addition/100);
			break;
		case FRAME_DRONE_SPEED:
			protocol_frame_read((uint8_t*)&drone_speed, frame_type, sizeof(DRONE_SPEED_T));
			byte_reverse((uint8_t*)&drone_speed.x, (uint8_t*)&drone_speed.x, 2, 1);
			byte_reverse((uint8_t*)&drone_speed.y, (uint8_t*)&drone_speed.y, 2, 1);
			byte_reverse((uint8_t*)&drone_speed.z, (uint8_t*)&drone_speed.z, 2, 1);
			//printf("%.2f	%.2f	%.2f\n", (double)drone_speed.x/100, (double)drone_speed.y/100, (double)drone_speed.z/100);
			break;
		default:
			break;	
	}	
}

/*写寄存器*/
void max7456_write_reg(uint8_t addr, uint8_t data)
{
	max7456_connect();
	max7456_write(addr);
	max7456_write(data);
	max7456_disconnect();
}

/*读寄存器*/
uint8_t max7456_read_reg(uint8_t addr)
{
	uint8_t data = 0;
	max7456_connect();
	delay_us(20);
	if(addr >= STAT) {
		max7456_write(addr);
	} else {
		max7456_write(addr | 0x80);
	}
	data = max7456_read();
	max7456_disconnect();
	return data;
}



/*写字符到MAX7456*/
void max7456_learn_char(uint8_t nvm_addr, uint8_t* data)
{
    uint8_t i = 0;
    max7456_write_reg(CMAH, nvm_addr);
    delay_ms(10);
    for(i = 0; i < 54; i++) {
        max7456_write_reg(CMAL, i);
        max7456_write_reg(CMDI, data[i]);
    }
    max7456_write_reg(CMM, NVM_W);
    delay_ms(100);
//    while ((max7456_read_reg(STAT) & 0x20) != 0x00);
//    if(USE_PAL) {
//        max7456_write_reg(VM0,0X48);
//    } else {
//        max7456_write_reg(VM0,0X08);
//    }
}

/*写字库到MAX7456*/
void max7456_learn_fonts(void)
{
	uint16_t i = 0;
	/*关闭MAX7456视频叠加*/
	max7456_write_reg(VM0, 0x00);
	delay_ms(10);
	for(i = 0; i <= 255; i++) {
		max7456_learn_char(i, (uint8_t *)&fontdata0[i][0]);
	}
}

void max7456_learn_logo(void)
{

}


void max7456_write_char(uint16_t addr, uint8_t ch)
{
	max7456_connect();
	max7456_write_reg(DMAH, (addr & 0XFF00) >> 8);
	max7456_write_reg(DMAL, (addr & 0XFF));
	max7456_write_reg(DMDI, ch);
	max7456_disconnect();
}

void max7456_write_char_xy(uint8_t x, uint8_t y, uint8_t ch)
{
	uint16_t addr = (uint16_t)y * 30 + x;
	max7456_write_char(addr, ch);
}

void max7456_draw_char(uint8_t x, uint8_t y, uint8_t ch)
{
	screen_buf[y * 30 + x] = ch;
}

void max7456_draw_char_xy(uint8_t x, uint8_t y,uint8_t ch)
{
	max7456_draw_char(x, y, ch);
}

void max7456_draw_string_xy(uint8_t x, uint8_t y, char* str, uint8_t len)
{
	uint8_t i = 0;
	for(i = 0; i < len; i++) {
		max7456_draw_char_xy(x, y, *str);
		str++;
		x++;
	}
}

void max7456_spi_send(uint8_t* buf, uint16_t size)
{
	uint16_t i = 0;
	max7456_connect();
	for(i = 0; i < size; i++) {
		max7456_write(buf[i]);
	} 
	max7456_disconnect();
}

void max7456_dma_send(uint8_t* buf, uint16_t size)
{
	max7456_spi_send(buf, size);
}

void max7456_send_data(void)
{
    uint16_t i = 0;
    uint16_t pos = 0;
    uint16_t len = 0;
    for(i = 0; i < 480; i++) {
        if(screen_buf[pos] != shadow_buf[pos]) {
            dmaspi_buf[len++] = DMAH;
            dmaspi_buf[len++] = pos >> 8;
            dmaspi_buf[len++] = DMAL;
            dmaspi_buf[len++] = pos & 0xFF;
            dmaspi_buf[len++] = DMDI;
            dmaspi_buf[len++] = screen_buf[pos];
            shadow_buf[pos] = screen_buf[pos];
        }
        
        if((480 <= pos) || (MAX_CHAR2UPDATE * 6 <= len)) {
            pos = 0;
            break;
        } else {
            pos++;
        }
    }
        
    if(len) {
        debug_data = len / 6;
        #ifdef USE_DMA_SPI_OSD
            max7456_dma_send(dmaspi_buf, len);
        #else
            max7456_spi_send(dmaspi_buf, len);
        #endif
    }
    memset(screen_buf, 0x00, sizeof(screen_buf));
}

void max7456_clear(void)
{

}


void max7456_offset(int8_t x, int8_t y)
{
	x += 32;
	y += 16;
	max7456_write_reg(HOS, x);
	max7456_write_reg(VOS, y);
}




uint16_t max7456_draw_assert(uint8_t x, uint8_t y, uint16_t min, uint16_t max, uint16_t value)
{
	if( min > value) {
		return min;
	}

	if( max < value) {
		return max;
	}
	return value;
}

void max7456_draw_allchar(void)
{
	uint16_t i = 0;
	for(i = 0; i < 30; i++) {
		max7456_write_char_xy(i, 1, i);
	}

	for(i = 30; i < 60; i++) {
		max7456_write_char_xy(i-30, 2, i);
	}

	for(i = 60; i < 90; i++) {
		max7456_write_char_xy(i-60, 3, i);
	}

	for(i = 90; i< 120; i++) {
		max7456_write_char_xy(i-90, 4, i);
	}

	for(i = 120; i < 150; i++) {
		max7456_write_char_xy(i-120, 5, i);
	}

	for(i = 150; i < 180; i++) {
		max7456_write_char_xy(i-150, 6, i);
	}

	for(i = 180; i < 210; i++) {
		max7456_write_char_xy(i-180, 7, i);
	}

	for(i = 180; i < 210; i++) {
		max7456_write_char_xy(i-180, 8, i);
	}

	for(i = 210; i < 240; i++) {
		max7456_write_char_xy(i-210, 9, i);
	}

	for(i = 240; i < 255; i++) {
		max7456_write_char_xy(i-240, 10, i);
	}
}

void max7456_draw_roll(uint8_t x, uint8_t y, double value)
{
    char buf[8] = {0};
    
//    if(180 > value) {
//        max7456_draw_char_xy(x, y, 0xE3);
//    } else {
//        max7456_draw_char_xy(x, y, 0xE4);
//        value -= 180;
//    }
    sprintf(buf, "R%.2f", value);
    max7456_draw_string_xy(x + 1, y, buf, sizeof(buf));
}

void max7456_draw_pitch(uint8_t x, uint8_t y, double value)
{
    char buf[8] = {0};
    
//    if(180 > value) {
//        max7456_draw_char_xy(x, y, 0x0E);
//    } else {
//        max7456_draw_char_xy(x, y, 0x0F);
//        value -= 180;
//    }
    sprintf(buf, "P%.2f", value);
    max7456_draw_string_xy(x + 1, y, buf, sizeof(buf));
    
}

void max7456_draw_yaw(uint8_t x, uint8_t y, double value)
{
    char buf[8] = {0};
    
//    if(180 > value) {
//        max7456_draw_char_xy(x, y, 0x0E);
//    } else {
//        max7456_draw_char_xy(x, y, 0x0F);
//        value -= 180;
//    }
    sprintf(buf, "Y%.2f", value);
    max7456_draw_string_xy(x + 1, y, buf, sizeof(buf));
    
}

void max7456_draw_power(uint8_t x, uint8_t y, uint16_t value)
{
	char buf[6] = {0};
	max7456_draw_char_xy(x, y, 0x1E);
	sprintf(buf, "%.2fV", (double)value/100);
	max7456_draw_string_xy(x + 1, y, buf, sizeof(buf));
}

void max7456_draw_gps(uint8_t x, uint8_t y, uint8_t value, double lng, double lat)
{
	char buf[12] = {0};
	max7456_draw_char_xy(x, y, 0x06);
	sprintf(buf, "%02d", value);
	max7456_draw_string_xy(x + 1, y++, buf, 2);
	sprintf(buf, "LNG:%.4f", lng);
	max7456_draw_string_xy(x, y++, buf, 10);
	sprintf(buf, "LAT:%.4f", lat);
	max7456_draw_string_xy(x, y++, buf, 10);
}

void max7456_draw_flytime(uint8_t x, uint8_t y, uint16_t value)
{
	char buf[5] = {0};
	max7456_draw_char_xy(x, y, 0x05);
	if(value < 6000) {
		sprintf(buf, "%02d:%02d", value/60, value%60);
		max7456_draw_string_xy(x + 1, y, buf, sizeof(buf));
	} else {
		max7456_draw_string_xy(x + 1, y, "Time Error!", 11);
	}
}

void max7456_draw_altitude(uint8_t x, uint8_t y, double value)
{
	char buf[12] = {0};
	if(value < 1000) {
		sprintf(buf, "AL:%.2fm", value);
		max7456_draw_string_xy(x, y, buf, sizeof(buf));  
	}   
}

void max7456_draw_speedv(uint8_t x, uint8_t y, double speed_z)
{
	char buf[12] = {0};
	if(speed_z < 1000) {
		sprintf(buf, "VS:%.2fm/s", speed_z);
		max7456_draw_string_xy(x, y, buf, sizeof(buf));  
	}  
}


void max7456_draw_speedh(uint8_t x, uint8_t y, double speed_x, double speed_y)
{
	char buf[12] = {0};
	if(speed_x < 1000) {
		sprintf(buf, "HS:%.2fm/s", sqrt(speed_x*speed_x + speed_y*speed_y));
		max7456_draw_string_xy(x, y, buf, sizeof(buf));  
	}  
}


void max7456_draw_compass(uint8_t x, uint8_t y, double value)
{
    char heading_all[] 	    = "W----N----E----S----W----N----E----";
    char heading_show[12]   = "W----N----E";
    uint8_t i = 0;
    uint8_t n = 0;
//    char buf[8] = {0};
    
    /*罗盘*/
    n = (value + 9) / 18;
    for(i = 0; i < 11; i++) {
        heading_show[i] = heading_all[i + n];
    }
    heading_show[i] = '\0';
    max7456_draw_string_xy(x, y, heading_show, 11);
    
//    sprintf(buf, "%.2f", value);
//    max7456_draw_string_xy(x + 3, y + 1, buf, sizeof(buf));
    
//    /*箭头*/
//    m = (uint8_t)(((double)value + 11.25) / 22.5);
//    if(16 <= m) {
//        m = 0;
//    }  
//    max7456_draw_string_xy(x + 5, y + 1, (char*)&heading_pic[m][0], 2);
    
}

#define C_RTBANGLE 0xB0
/*画回航角*/
void max7456_draw_rtbagl(uint32_t x, uint32_t y, double hderr)
{
	int32_t point;

	hderr += 11.25;
	if(0 > hderr) {
		hderr += 360;
	}
	point = hderr / 22.5;
	point =	point * 2;
	point =	clamp(point , 30.1, 0);

	max7456_draw_char_xy(x, y, C_RTBANGLE + point);
	max7456_draw_char_xy(x + 1, y, C_RTBANGLE + point + 1);	
}

void max7456_draw_radar_heading(uint32_t x, uint32_t y, double hding)
{
	uint32_t point;

	hding += 22.5;
	if(360 <= hding) {
		hding -= 360;
	}
	point = hding / 45;
	point = radarchar[point];
	max7456_draw_char_xy(x, y, point);
}

/*一机飞行时*/
void max7456_draw_radar(uint32_t x, uint32_t y, double jd, double wd, double hding, uint32_t disen)
{
    double wdhome = 0;
    double jdhome = 0;
	int32_t distx, disty, xoffset = 0, yoffset = -1;	
	static int32_t xoffsetlt = 0, yoffsetlt = -1;		//清理上次值

	disty = ( wd - wdhome ) * 1850; 	//1分=1.85km-单位米
	distx = ( jd - jdhome ) * 1850 * mycos(wd * PIdv2() / 5400);//(5400 - myfabs(wd)) / 5400; //经度1度长 * cos纬度	 90d=540000分	


		 if(distx <= -300) xoffset = -5;			// 【x】
	else if(distx <= -120) xoffset = -3;		
	else if(distx <= -50)  xoffset = -1;	
	else if(distx <= 0)    xoffset = 0;		
	else if(distx <= 50)   xoffset = 1;	
	else if(distx <= 120)  xoffset = 2;			
	else if(distx <= 300)  xoffset = 4;
	else                   xoffset = 6;

         if(disty <= -300) yoffset = 3;			// 【y】
    else if(disty <= -100) yoffset = 2;		
    else if(disty <= -30)  yoffset = 1;			
    else if(disty <= 30)   yoffset = 0;	
    else if(disty <= 100)  yoffset = -1;			
    else if(disty <= 300)  yoffset = -2;
    else                   yoffset = -3;

   if(xoffset == 0 && yoffset == 0) {
        if(distx < disty && distx < -disty) {
            xoffset = -1;
        } else if( distx > -disty ) {
            yoffset = -1;
        } else {
            yoffset =  1;
        }
   }

   if(xoffset == 1 && yoffset == 0) {
        if( distx > disty && distx > -disty ) {
            xoffset =  2;
        } else if( distx <  disty ) {
            yoffset = -1;
        } else {
            yoffset =  1;
        }
   }
   	
    if(xoffsetlt != xoffset || yoffsetlt != yoffset) {	 //位置有变化
        max7456_draw_radar_heading(x + xoffset, y + yoffset, hding);	 // 改变转向
        max7456_draw_char_xy(x + xoffsetlt, y + yoffsetlt, 0);	 //上位置清零
    } else {
        max7456_draw_radar_heading(x + xoffset, y + yoffset, hding);	 // 改变转向
    }
    xoffsetlt = xoffset;	
    yoffsetlt = yoffset;
}

/*两机编队模式*/
void max7456_draw_radar2(uint32_t x, uint32_t y, double jd, double wd, double hding, double jdw, double wdw, uint32_t disen)
{
    double wdhome = 0;
    double jdhome = 0;
	int32_t distx, disty, xoffset = 0, yoffset = -1;	
	static int32_t xoffsetlt = 0, yoffsetlt = -1;		//清理上次值

	int32_t distxw, distyw, xoffsetw = 0, yoffsetw = -1;	
	static int32_t xoffsetltw = 0,	yoffsetltw = -1;		//清理上次值

	disty = ( wd - wdhome ) * 1850; 	//1分=1.85km-单位米
	distx = ( jd - jdhome ) * 1850 * mycos(wd * PIdv2() / 5400);//(5400 - myfabs(wd)) / 5400; //经度1度长 * cos纬度	 90d=540000分	


		 if(distx <= -300) xoffset = -5;			// 【x】
	else if(distx <= -120) xoffset = -3;		
	else if(distx <= -50)  xoffset = -1;	
	else if(distx <= 0)    xoffset = 0;		
	else if(distx <= 50)   xoffset = 1;	
	else if(distx <= 120)  xoffset = 2;			
	else if(distx <= 300)  xoffset = 4;
	else                   xoffset = 6;

         if(disty <= -300) yoffset = 3;			// 【y】
    else if(disty <= -100) yoffset = 2;		
    else if(disty <= -30)  yoffset = 1;			
    else if(disty <= 30)   yoffset = 0;	
    else if(disty <= 100)  yoffset = -1;			
    else if(disty <= 300)  yoffset = -2;
    else                   yoffset = -3;

   if(xoffset == 0 && yoffset == 0) {
        if(distx < disty && distx < -disty) {
            xoffset = -1;
        } else if( distx > -disty ) {
            yoffset = -1;
        } else {
            yoffset =  1;
        }
   }

   if(xoffset == 1 && yoffset == 0) {
        if( distx > disty && distx > -disty ) {
            xoffset =  2;
        } else if( distx <  disty ) {
            yoffset = -1;
        } else {
            yoffset =  1;
        }
   }
   	
    if(xoffsetlt != xoffset || yoffsetlt != yoffset) {	 //位置有变化
        max7456_draw_radar_heading(x + xoffset, y + yoffset, hding);	 // 改变转向
        max7456_draw_char_xy(x + xoffsetlt, y + yoffsetlt, 0);	 //上位置清零
    } else {
        max7456_draw_radar_heading(x + xoffset, y + yoffset, hding);	 // 改变转向
    }
    xoffsetlt = xoffset;	
    yoffsetlt = yoffset;

    /*双机编队时*/
    distyw = ( wdw - wdhome ) * 1850; 	//1分=1.85km-单位米
    distxw = ( jdw - jdhome ) * 1850 * mycos(wdw * PIdv2() / 5400);//(5400 - myfabs(wdw)) / 5400; //经度1度长 * cos纬度	 90d=540000分	


         if(distxw <=-300) xoffsetw = -5;			// 【x】
	else if(distxw <=-120) xoffsetw = -3;
	else if(distxw <=-50)  xoffsetw = -1;
	else if(distxw <= 0)   xoffsetw = 0;
	else if(distxw <= 50)  xoffsetw = 1;
	else if(distxw <= 120) xoffsetw = 2;
	else if(distxw <= 300) xoffsetw = 4;
	else                   xoffsetw = 6;

         if(distyw <= -300) yoffsetw = 3;			// 【y】
    else if(distyw <= -100) yoffsetw = 2;
    else if(distyw <= -30)  yoffsetw = 1;
    else if(distyw <= 30)   yoffsetw = 0;
    else if(distyw <= 100)  yoffsetw = -1;
    else if(distyw <= 300)  yoffsetw = -2;
    else                    yoffsetw = -3;


    if(xoffsetw == 0 && yoffsetw == 0) {
        if( distxw < distyw && distxw < -distyw ) {
            xoffsetw = -1;
        } else if(distxw > -distyw) {
            yoffsetw = -1;
        } else {
            yoffsetw =  1;
        }
    }

    if(xoffsetw == 1 && yoffsetw == 0) {
        if( distxw > distyw && distxw > -distyw ) {
            xoffsetw =  2;
        } else if( distxw <  distyw ) {
            yoffsetw = -1;
        } else {
            yoffsetw =  1;
        }
    }
  	
	if(xoffsetltw != xoffsetw || yoffsetltw != yoffsetw ) { //位置有变化
        if(xoffsetw != xoffset || yoffsetw != yoffset) {
			max7456_draw_char_xy(x + xoffsetw, y + yoffsetw, disen == 1 ? 232 : 0);	 
			max7456_draw_char_xy(x + xoffsetltw, y + yoffsetltw, 0);	 //上位置清零
        } 
    } else if(xoffsetw != xoffset || yoffsetw != yoffset) {
            max7456_draw_char_xy (x + xoffsetw, y + yoffsetw, disen == 1 ? 232 : 0);	 //上位置	改变转向
    }
    xoffsetltw = xoffsetw;	
    yoffsetltw = yoffsetw;
}


/*画轴算法*/
void max7456_draw_skyline_axis(uint8_t x, uint8_t y, double roll, double pitch, int8_t offset_x)
{
	uint8_t mode = 1;
	int16_t offset_y = 0;
	int16_t offset_r = 0;
	double rad_roll = 3.14 * roll / 180;
	double rad_pitch = 3.14 * pitch / 180;
	
	/*模式: 0-roll反，pitch正 1-roll反，pitch反 2-roll正，pitch正 3-roll正，pitch反*/
	switch(mode) {
		case 0:
			break;
		case 1:
			pitch *= -1;
			break;
		case 2:
			offset_x *= -1;
			break;
		case 3:
			offset_x *= -1;
			pitch *= -1;
			break;
		default:
			break;
	}
	
	/*算法*/
	offset_y = (int16_t)(offset_x * tan(rad_roll)) + pitch*2; 
	offset_y = (int16_t)clamp(offset_y, 60, -60);

	if(offset_y >= 0) {	
		offset_r = (offset_y + 7) % 18;
		offset_y = (offset_y + 7) / 18;
		max7456_draw_char_xy(x, y - offset_y, 0x91 - offset_r);
	} else {
		offset_r = (offset_y - 10) % 18;
		offset_y = (offset_y - 10) / 18;
		max7456_draw_char_xy(x, y - offset_y, 0x80 - offset_r);
	}		
}

/*画地平线*/
void max7456_draw_skyline(uint8_t x, uint8_t y, double roll, double pitch)
{  
	uint8_t offset_base = 6;
	
	/*画右边轴*/
	max7456_draw_skyline_axis(x + 2, y, roll, pitch, offset_base + 12 * 0);
	max7456_draw_skyline_axis(x + 3, y, roll, pitch, offset_base + 12 * 1);
	max7456_draw_skyline_axis(x + 4, y, roll, pitch, offset_base + 12 * 2);
	max7456_draw_skyline_axis(x + 5, y, roll, pitch, offset_base + 12 * 3);
	max7456_draw_skyline_axis(x + 6, y, roll, pitch, offset_base + 12 * 4);

	/*画左边轴*/
	max7456_draw_skyline_axis(x - 1, y, roll, pitch, 0 - (offset_base + 12 * 0));
	max7456_draw_skyline_axis(x - 2, y, roll, pitch, 0 - (offset_base + 12 * 1));
	max7456_draw_skyline_axis(x - 3, y, roll, pitch, 0 - (offset_base + 12 * 2));
	max7456_draw_skyline_axis(x - 4, y, roll, pitch, 0 - (offset_base + 12 * 3));
	max7456_draw_skyline_axis(x - 5, y, roll, pitch, 0 - (offset_base + 12 * 4));
	
	/*画中心点*/
	max7456_draw_char_xy(x + 0, y, 0x01);
	max7456_draw_char_xy(x + 1, y, 0x02);    
}

void max7456_draw_data()
{
	uint8_t x = 2;
	uint8_t y = 2;
	char buf[20] = {0};
    
	max7456_draw_string_xy(x + 10, y++, "STATUS", 6);
	
	sprintf(buf, "    ROLL %.2f", (double)drone_status.roll/100);
	max7456_draw_string_xy(x, y++, buf, sizeof(buf));
	sprintf(buf, "   PITCH %.2f", (double)drone_status.pitch/100);
	max7456_draw_string_xy(x, y++, buf, sizeof(buf));
	sprintf(buf, "     YAW %.2f", (double)drone_status.yaw/100);
	max7456_draw_string_xy(x, y++, buf, sizeof(buf));
	sprintf(buf, "     ALT %.2f", (double)drone_status.alt/100);
	max7456_draw_string_xy(x, y++, buf, sizeof(buf));
	sprintf(buf, "FLY MODE %d", drone_status.fly_mode);
	max7456_draw_string_xy(x, y++, buf, sizeof(buf));
	sprintf(buf, "   ARMED %d", drone_status.armed);
	max7456_draw_string_xy(x, y++, buf, sizeof(buf));
	sprintf(buf, "FUN MODE %d", drone_status.fun_mode);
	max7456_draw_string_xy(x, y++, buf, sizeof(buf));
	sprintf(buf, "    SIIN %d", drone_status.siin);
	max7456_draw_string_xy(x, y++, buf, sizeof(buf));
	
}



uint32_t sec_cnt = 0;
void max7456_draw_osd(void)
{
//	max7456_draw_allchar();
//	max7456_draw_data();
	
	max7456_draw_flytime(21, 1, sec_cnt);
	max7456_draw_roll(19, 2, (double)drone_status.roll/100);
	max7456_draw_pitch(19, 3, (double)drone_status.pitch/100);
	max7456_draw_yaw(19, 4, (double)drone_status.yaw/100);
	
	max7456_draw_altitude(0, 10, (double)drone_status.alt/100);
	max7456_draw_speedv(0, 11, (double)drone_speed.z/100);
	max7456_draw_speedh(0, 12, (double)drone_speed.x/100, (double)drone_speed.y/100);
	max7456_draw_gps(0, 1, drone_gpsdata.gps_num, drone_gpsdata.lng, drone_gpsdata.lat);
	max7456_draw_power(21, 12, drone_power.voltage);
	max7456_draw_compass(8, 1, drone_status.yaw/100);
	//max7456_draw_radar(8, 2, 114.22, 86.32, 120, 2);
	max7456_draw_rtbagl(13, 2, drone_gpsdata.rth_degree);
	max7456_draw_skyline(13, 7, (double)drone_status.roll/100, (double)drone_status.pitch/100);
	max7456_send_data();
}

void max7456_init(void)
{
	uint8_t ch = 0;

	/*软件复位*/
	max7456_write_reg(VM0, 0x02);
	delay_ms(100);

	/*关闭视频模式*/
	max7456_write_reg(VM0, 0x00);
	delay_ms(100);

	/*清理显存*/
	max7456_write_reg(DMM, 0x04);
	delay_ms(100);

	/*设置为8bit模式*/
	max7456_write_reg(DMM, 0x40);
	delay_ms(100);

	/*设置为NTSC制式*/
	max7456_write_reg(VM0, 0x08);
	delay_ms(100);

	/*不闪*/
	max7456_write_reg(VM1, 0x00);
	delay_ms(100);

	ch = max7456_read_reg(OSDBL_R_ADD);

	max7456_write_reg(OSDBL_W_ADD, ch & 0x7F);
	max7456_write_reg(OSDM, 0x2D);
	delay_ms(100);
 
	max7456_offset(10, -10);

	//max7456_learn_fonts();
	//delay_ms(100);
}

void task_max7456_draw_osd(void)
{
	max7456_draw_osd();
}

void task_max7456_get_data(void)
{
	max7456_get_data();
}

void task_max7456_time(void)
{
	uint8_t res = 0;
	sec_cnt++;

	res = max7456_read_reg(VM0);
	printf("VM1:%02X\n", res);
}
















