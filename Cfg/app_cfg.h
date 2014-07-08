/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                              (c) Copyright 2009; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                         Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : JJL
*                 EHS
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

#include <includes.h>

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                              2


#if 0
#define PC_SIMULATION
#endif

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                         256

// CC1101 Task
#define  APP_TASK_CC1101_STK_SIZE                        400
#define  APP_TASK_CC1101_PRIO                            3
extern OS_TCB   AppTaskCC1101TCB;
extern void TaskCreate_CC1101(void);

// Pc Parser Task
#define  APP_TASK_PC_PARSER_STK_SIZE                      400
#define  APP_TASK_PC_PARSER_PRIO                          4
extern OS_TCB   AppTaskPcParserTCB;
extern void TaskCreate_PcParser(void);

// Simulate Task
#ifdef PC_SIMULATION
#define  APP_TASK_SIMLUATE_STK_SIZE                       256
#define  APP_TASK_SIMLUATE_PRIO                           5
extern OS_TCB   AppTaskSimluateTCB;
extern void TaskCreate_Simluate(void);
#endif

// UART Device
extern OS_MUTEX    PC_UART_DEVICE;

// Global Variable Protect
extern OS_MUTEX    GLOBAL_DATA_PROTECT;

// PC Memory Manage
#define PC_MSG_SIZE                                       32
#define PC_MSG_CNT                                        4
extern  OS_MEM PC_Msg;

// RF Memory Manage
#define RF_MSG_SIZE                                       32
#define RF_MSG_CNT                                        4
extern  OS_MEM RF_Msg;

// PC Tx FIFO
#define PC_TX_BUFF_SIZE                                   256

#endif
