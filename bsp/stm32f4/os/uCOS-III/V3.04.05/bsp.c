#include  <bsp.h>
#include  <os.h>
#include "drv_console.h"


#define  BSP_REG_DEM_CR                           (*(CPU_REG32 *)0xE000EDFC)
#define  BSP_REG_DWT_CR                           (*(CPU_REG32 *)0xE0001000)
#define  BSP_REG_DWT_CYCCNT                       (*(CPU_REG32 *)0xE0001004)
#define  BSP_REG_DBGMCU_CR                        (*(CPU_REG32 *)0xE0042004)

#define  BSP_BIT_DEM_CR_TRCENA                    DEF_BIT_24
#define  BSP_BIT_DWT_CR_CYCCNTENA                 DEF_BIT_00


void  BSP_Init (void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    BSP_Tick_Init();
    drv_console_init();
}


CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);

    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}


void  BSP_Tick_Init (void)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    
    
    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    
#if (OS_VERSION >= 30000u)
    cnts = (cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz);      /* Determine nbr SysTick increments                     */
#else
    cnts = (cpu_clk_freq / (CPU_INT32U)OS_TICKS_PER_SEC);       /* Determine nbr SysTick increments.                    */
#endif
    
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */
}



#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
    CPU_INT32U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();

    BSP_REG_DEM_CR     |= (CPU_INT32U)BSP_BIT_DEM_CR_TRCENA;    /* Enable Cortex-M4's DWT CYCCNT reg.                   */
    BSP_REG_DWT_CYCCNT  = (CPU_INT32U)0u;
    BSP_REG_DWT_CR     |= (CPU_INT32U)BSP_BIT_DWT_CR_CYCCNTENA;

    CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)fclk_freq);
}
#endif



#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    CPU_TS_TMR  ts_tmr_cnts;


    ts_tmr_cnts = (CPU_TS_TMR)BSP_REG_DWT_CYCCNT;

    return (ts_tmr_cnts);
}
#endif


#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS32_to_uSec (CPU_TS32  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif


#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS64_to_uSec (CPU_TS64  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif


void  BSP_IntDisAll (void)
{
    CPU_IntDis();
}
