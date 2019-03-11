/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                       IAR Development Kits
*                                              on the
*
*                                    STM32F429II-SK KICKSTART KIT
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : YS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>
#include "drv_console.h"
#include "app_emmc.h"
#include "sys_api.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  APP_TASK_EQ_0_ITERATION_NBR              16u


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* ----------------- APPLICATION GLOBALS -------------- */
static  OS_TCB      AppTaskStartTCB;
static  CPU_STK     AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
                                                                /* ------------ FLOATING POINT TEST TASK -------------- */
static  OS_TCB      App_Task1TCB;
static  CPU_STK     App_Task1Stk[APP_CFG_TASK_1_STK_SIZE];
static  OS_TCB      App_Task2TCB;
static  CPU_STK     App_Task2Stk[APP_CFG_TASK_2_STK_SIZE];
static  OS_TCB      App_Task3TCB;
static  CPU_STK     App_Task3Stk[APP_CFG_TASK_3_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart          (void  *p_arg);
static  void  App_Task1             (void  *p_arg);
static  void  App_Task2             (void  *p_arg);
static  void  App_Task3             (void  *p_arg);
static  void  AppTaskCreate         (void);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
uint16_t buf1[2048] = {0};
uint16_t buf2[2048] = {0};


int ucos_start(void)
{
    OS_ERR  err;


    BSP_IntDisAll();                                            /* Disable all interrupts.                              */
    
    CPU_Init();                                                 /* Initialize the uC/CPU Services                       */
    Mem_Init();                                                 /* Initialize Memory Management Module                  */
    Math_Init();                                                /* Initialize Mathematical Module                       */
    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"App Task Start",
                 (OS_TASK_PTR   )AppTaskStart,
                 (void         *)0u,
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&AppTaskStartStk[0u],
                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    (void)&err;

    return (0u);
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    OS_ERR  err;
   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */
    //BSP_Tick_Init();                                            /* Initialize Tick Services.                            */
    emmc_init(&emmc1);
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
    
    APP_TRACE_DBG(("Creating Application Tasks\r\n"));
    AppTaskCreate();                                            /* Create Application tasks                             */


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        //APP_TRACE_DBG(("task running...\r\n"));
        //APP_TRACE_DBG(("Task Running...%d\r\n", OSTimeGet(&err)));
        OSTimeDlyHMSM(0u, 0u, 3u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}


/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static void AppTaskCreate (void)
{
    OS_ERR  os_err;
    
                                                                /* ------------- CREATE FLOATING POINT TASK ----------- */
    OSTaskCreate((OS_TCB      *)&App_Task1TCB,
                 (CPU_CHAR    *)"App Task1",
                 (OS_TASK_PTR  ) App_Task1, 
                 (void        *) 0,
                 (OS_PRIO      ) APP_CFG_TASK_1_PRIO,
                 (CPU_STK     *)&App_Task1Stk[0],
                 (CPU_STK_SIZE ) App_Task1Stk[APP_CFG_TASK_1_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) APP_CFG_TASK_1_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
                 
    OSTaskCreate((OS_TCB      *)&App_Task2TCB,
                 (CPU_CHAR    *)"App Task2",
                 (OS_TASK_PTR  ) App_Task2, 
                 (void        *) 0,
                 (OS_PRIO      ) APP_CFG_TASK_2_PRIO,
                 (CPU_STK     *)&App_Task2Stk[0],
                 (CPU_STK_SIZE ) App_Task2Stk[APP_CFG_TASK_2_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) APP_CFG_TASK_2_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
    
    OSTaskCreate((OS_TCB      *)&App_Task3TCB,
                 (CPU_CHAR    *)"App Task3",
                 (OS_TASK_PTR  ) App_Task3, 
                 (void        *) 0,
                 (OS_PRIO      ) APP_CFG_TASK_3_PRIO,
                 (CPU_STK     *)&App_Task3Stk[0],
                 (CPU_STK_SIZE ) App_Task3Stk[APP_CFG_TASK_3_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) APP_CFG_TASK_3_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
}

static void App_Task1(void *p_arg)
{
    OS_ERR err;
        
    while(DEF_TRUE) {
        //emmc_speed_test_write(&emmc1, 4, 10000, EMMC_SPEED_TEST_FILE1);
        OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}


static void App_Task2(void *p_arg)
{
    OS_ERR err;
    while(DEF_TRUE) {
        emmc_speed_test_write(&emmc1, 4, 100000, EMMC_SPEED_TEST_FILE1);
        OSTimeDlyHMSM(0u, 0u, 0u, 40u, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

static void App_Task3(void *p_arg)
{
    OS_ERR err;
    while(DEF_TRUE) {
        //emmc_speed_test_write(&emmc1, 4, 400, EMMC_SPEED_TEST_FILE2);
        //emmc_speed_test_read(&emmc1, EMMC_SPEED_TEST_FILE2);
        OSTimeDlyHMSM(0u, 0u, 0u, 40u, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}
