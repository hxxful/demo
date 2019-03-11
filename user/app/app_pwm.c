#include "sys.h"
#include "app_protocol.h"
#include "app_pwm.h"

static uint32_t cnt = 500;
void task_pwm()
{
	protocol_frame_read((uint8_t *)&motor_degree, 0xFC, sizeof(MOTOR_DEGREE_T));
	
	if(motor_degree.degree > 180)
		return;
	else
		cnt = 500 + motor_degree.degree * 11;
	
	pwm_write(cnt, 1);
	pwm_write(cnt, 2);
}
