#include "drv_common.h"
#include "drv_delay.h"

//#define OS_SUPPORT_NONE
#define OS_SUPPORT_UCOSIII




#ifdef OS_SUPPORT_NONE

#define drv_systick_irq_func SysTick_Handler
static uint8_t  fac_us = 0;
static uint16_t fac_ms = 0;

//初始化延迟函数
//SYSTICK的时钟固定为AHB时钟的1/8
//SYSCLK:系统时钟频率
void drv_delay_init(u8 SYSCLK)
{
 	SysTick->CTRL&=~(1<<2);					//SYSTICK使用外部时钟源	 
	fac_us=SYSCLK/8;						//不论是否使用OS,fac_us都需要使用
	fac_ms=((u32)SYSCLK*1000)/8;			//非OS下,代表每个ms需要的systick时钟数   
}		

void drv_systick_irq_func(void)
{

}

//延时nus
//nus为要延时的us数.	
//注意:nus的值,不要大于798915us(最大值即2^24/fac_us@fac_us=21)
void drv_delay_us(uint16_t nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 				//时间加载	  		 
	SysTick->VAL=0x00;        				//清空计数器
	SysTick->CTRL=0x01 ;      				//开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL=0x00;      	 			//关闭计数器
	SysTick->VAL =0X00;       				//清空计数器 
}
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对180M条件下,nms<=745ms 
void drv_delay_xms(uint16_t nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;			//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           			//清空计数器
	SysTick->CTRL=0x01 ;          			//开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL=0x00;       				//关闭计数器
	SysTick->VAL =0X00;     		  		//清空计数器	  	    
} 
//延时nms 
//nms:0~65535
void drv_delay_ms(uint16_t nms)
{	 	 
	u8 repeat=nms/540;						//这里用540,是考虑到某些客户可能超频使用,
											//比如超频到248M的时候,delay_xms最大只能延时541ms左右了
	u16 remain=nms%540;
	while(repeat)
	{
		drv_delay_xms(540);
		repeat--;
	}
	if(remain)drv_delay_xms(remain);
} 

void drv_delay_s(uint16_t n)
{
    drv_delay_ms(n*1000);
}
#endif

#ifdef OS_SUPPORT_UCOSIII

#include "os.h"

void drv_delay_us(uint16_t n)
{
    
}

void drv_delay_ms(uint16_t n)
{
    OS_ERR err = OS_ERR_NONE;
    OSTimeDlyHMSM(0u, 0u, 0u, n, OS_OPT_TIME_HMSM_STRICT, &err);
}

void drv_delay_s(uint16_t n)
{
    OS_ERR err = OS_ERR_NONE;
    OSTimeDlyHMSM(0u, 0u, n, 0u, OS_OPT_TIME_HMSM_STRICT, &err);

}
#endif



