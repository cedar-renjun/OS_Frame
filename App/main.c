#include <includes.h>

static  OS_TCB   AppTaskStartTCB; 
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];

static  OS_TCB   AppTaskLEDTCB; 
static  CPU_STK  AppTaskLEDStk[APP_TASK_START_STK_SIZE];

static  OS_TMR   RF_RetransTimer;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);
static  void  AppTaskLED    (void *p_arg);

static  void  LED_Triggle(void *p_tmr, void *p_arg)
{
    GPIOA->ODR ^= GPIO_Pin_2;
}


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

int  main (void)
{
    OS_ERR  err;

    CPU_IntDis();                                               /* Disable all interrupts.                              */
    
    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR )AppTaskStart, 
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
#if 0
    OSTaskCreate((OS_TCB     *)&AppTaskLEDTCB,
                 (CPU_CHAR   *)"App Task LED",
                 (OS_TASK_PTR )AppTaskLED, 
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_LED_PRIO,
                 (CPU_STK    *)&AppTaskLEDStk[0],
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
#endif

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
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
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;

   (void)p_arg;

    BSP_Init();                                                   /* Initialize BSP functions                         */
    CPU_Init();                                                   /* Initialize the uC/CPU services                   */

    cpu_clk_freq = BSP_CPU_ClkFreq();                             /* Determine SysTick reference freq.                */                                                                        
    cnts         = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;  /* Determine nbr SysTick increments                 */
    OS_CPU_SysTickInit(cnts);                                     /* Init uC/OS periodic time src (SysTick).          */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                                 /* Compute CPU capacity with no task running        */
#endif

#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    
    /*
    OSTmrCreate(&RF_RetransTimer, 
                "RF Retrans Timer",
                0,
                20,
                OS_OPT_TMR_PERIODIC,
                LED_Triggle,
                (void*)0,
                &err
                );
    OSTmrStart(&RF_RetransTimer, &err);
    */
    
    while (DEF_TRUE) {                                            /* Task body, always written as an infinite loop.   */
    GPIOA->ODR ^= GPIO_Pin_2;
    PC_PrintStr("Hello,world\r\n");
    OSTimeDlyHMSM( 0, 0, 0, 50,
                  OS_OPT_TIME_HMSM_STRICT, 
                  &err);
    }
}

static  void  AppTaskLED (void *p_arg)
{
    OS_ERR      err;

   (void)p_arg;

    while (DEF_TRUE) {                                            /* Task body, always written as an infinite loop.   */
    GPIOA->ODR ^= GPIO_Pin_2;

    OSTimeDlyHMSM( 0, 0, 0, 22,
                  OS_OPT_TIME_HMSM_STRICT, 
                  &err);
    }
}