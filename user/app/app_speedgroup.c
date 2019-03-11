#include <stdint.h>
#include "app_speedgroup.h"

#define SPEED_NUM	5
#define TASK_NUM	3

volatile uint8_t sys_speed_flag[SPEED_NUM] = {0};

typedef struct {
	void (*task_name)(void);
	uint8_t enable;
	uint8_t var;
}TASK;

void task_null(void)
{

}

TASK task_list[SPEED_NUM][TASK_NUM] = {
{
/*2ms速率组*/
{task_null, 0, 0},
{task_null, 0, 0},
{task_null, 0, 0},
},

{
/*10ms速率组*/
{task_null, 0, 0},
{task_null,  0, 0},
{task_null, 0, 0},
},

{
/*50ms速率组*/
{task_null, 0, 0},
{task_null, 0, 0},
{task_null, 0, 0},
},

{
/*100ms速率组*/
{task_null, 0, 0},
{task_null, 0, 0},
{task_null, 0, 0},
},

{
/*1s速率组*/
{task_null, 0, 0},
{task_null, 0, 0},
{task_null, 0, 0},
}
};

void start_task(void)
{
	uint8_t i = 0;
	uint8_t j = 0;
	
	for(i = 0; i < SPEED_NUM; i++) {		
		if(1 == sys_speed_flag[i]){
			sys_speed_flag[i] = 0;
			for(j = 0; j < TASK_NUM; j++){
				if(1 == task_list[i][j].enable)
					task_list[i][j].task_name();
			}
		}		
	}	
}

