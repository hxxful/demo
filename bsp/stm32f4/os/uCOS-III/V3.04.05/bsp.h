/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                     BOARD SUPPORT PACKAGE (BSP)
*
*                                       IAR Development Kits
*                                              on the
*
*                                    STM32F429II-SK KICKSTART KIT
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : FF
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*
*           (2) This file and its dependencies requires IAR v6.20 or later to be compiled.
*
*********************************************************************************************************
*/

#ifndef  BSP_PRESENT
#define  BSP_PRESENT


/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_MODULE
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include  <stdio.h>
#include  <stdarg.h>

#include  <cpu.h>
#include  <cpu_core.h>

#include  <lib_def.h>
#include  <lib_ascii.h>


#include  <stm32f4xx_conf.h>

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             PERIPH DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void        BSP_Init                          (void);

void        BSP_IntDisAll                     (void);

CPU_INT32U  BSP_CPU_ClkFreq                   (void);

void        BSP_Tick_Init                     (void);

/*
*********************************************************************************************************
*                                           INTERRUPT SERVICES
*********************************************************************************************************
*/

void        BSP_IntInit                       (void);

void        BSP_IntEn                         (CPU_DATA       int_id);

void        BSP_IntDis                        (CPU_DATA       int_id);

void        BSP_IntClr                        (CPU_DATA       int_id);

void        BSP_IntVectSet                    (CPU_DATA       int_id,
                                               CPU_FNCT_VOID  isr);

void        BSP_IntPrioSet                    (CPU_DATA       int_id,
                                               CPU_INT08U     prio);


/*
*********************************************************************************************************
*                                     PERIPHERAL POWER/CLOCK SERVICES
*********************************************************************************************************
*/

CPU_INT32U   BSP_PeriphClkFreqGet        (CPU_DATA       pwr_clk_id);

void         BSP_PeriphEn                (CPU_DATA       pwr_clk_id);

void         BSP_PeriphDis               (CPU_DATA       pwr_clk_id);


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/


#endif                                                          /* End of module include.                               */

