#include "drv_common.h"
#include "drv_dma.h"
#include "drv_max7456.h"

#define drv_irq_dma1_ch1_func DMA1_Channel2_3_IRQHandler


DMA_InitTypeDef DMA_InitStructure;   
NVIC_InitTypeDef NVIC_InitStructure;
void drv_dma1_init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
 
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
    
    DMA_DeInit(DMA1_Channel3);    
    //DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buf;
    //DMA_InitStructure.DMA_BufferSize = size;   
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;       
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;    
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, ENABLE);
}

void drv_dma1_enable(void)
{
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

void drv_dma1_disable(void)
{
    DMA_Cmd(DMA1_Channel3, DISABLE);
}

int8_t drv_dma1_write(uint8_t* buf, uint32_t size)
{   
    drv_dma1_disable();
    //drv_max7456_disconnect();
    
    if(DMA_InitStructure.DMA_MemoryBaseAddr != (uint32_t)buf || DMA_InitStructure.DMA_BufferSize != size) {
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buf;
        DMA_InitStructure.DMA_BufferSize = size;
        DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    }
    
    //drv_max7456_connect();
    drv_dma1_enable();
    return 0;
}

void drv_irq_dma1_ch1_func(void)
{
    if(RESET != DMA_GetITStatus(DMA1_IT_TC3)) {
        DMA_ClearITPendingBit(DMA1_IT_TC3);
        //drv_dma1_disable();
        //drv_max7456_disconnect();
    }
}
