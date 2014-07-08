#include <includes.h>

static  OS_TCB   AppTaskStartTCB; 
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];

static  OS_TMR   RF_RetransTimer;

static  void  AppTaskStart  (void *p_arg);

OS_MUTEX    PC_UART_DEVICE;

// Global Variable Protect
OS_MUTEX    GLOBAL_DATA_PROTECT;

OS_MEM     PC_Msg;
CPU_INT08U PC_MsgBuf[PC_MSG_CNT][PC_MSG_SIZE];

OS_MEM     RF_Msg;
CPU_INT08U RF_MsgBuf[RF_MSG_CNT][RF_MSG_SIZE];

FIFO_S_t* PC_Tx;

/*
typedef struct
{
    int16_t  mouse_x;                               // 鼠标坐标X
    int16_t  mouse_y;                               // 鼠标坐标Y
    int16_t  mouse_z;                               // 鼠标坐标Z
    uint8_t  left_key_press_flag;                   // 鼠标左键是否按下
    uint8_t  right_key_press_flag;                  // 鼠标右键是否按下
    uint16_t keyboard_value;                        // 当前键盘按键
    uint16_t reserved;
}CONTROL_INFO;
*/

int  main (void)
{
    OS_ERR  err;
    
    /*
    CONTROL_INFO_T Cmd_Control;

    Cmd_Control.seq_number.value          = (uint16_t) 0x1234;
    Cmd_Control.cmd_id                    = (uint16_t) CMD_CONTROL;
    Cmd_Control.data.mouse_x              = (int16_t ) 0x1234;
    Cmd_Control.data.mouse_y              = (int16_t ) 0x5678;
    Cmd_Control.data.left_key_press_flag  = (int8_t  ) 0x01;
    Cmd_Control.data.right_key_press_flag = (int8_t  ) 0x00;
    Cmd_Control.data.keyboard_value       = (uint16_t) 0x07;
        
    FramePack((uint8_t *)&(Cmd_Control.seq_number.value), sizeof(SEQ_NUM) + sizeof(uint16_t) + sizeof(CONTROL_INFO),
              (uint8_t *)&Cmd_Control);
    */
    
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
                 (OS_MSG_QTY  )5,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

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
    
    PC_Tx = FIFO_S_Create(PC_TX_BUFF_SIZE);
    
    // Create Memory Manage
    OSMemCreate((OS_MEM      *) &PC_Msg,
                (CPU_CHAR    *) "PC Message Queue",
                (void        *) &PC_MsgBuf[0][0],
                (OS_MEM_QTY   ) PC_MSG_CNT,
                (OS_MEM_SIZE  ) PC_MSG_SIZE,
                (OS_ERR      *) &err);
    
    OSMemCreate((OS_MEM      *) &RF_Msg,
                (CPU_CHAR    *) "RF Message Queue",
                (void        *) &RF_MsgBuf[0][0],
                (OS_MEM_QTY   ) RF_MSG_CNT,
                (OS_MEM_SIZE  ) RF_MSG_SIZE,
                (OS_ERR      *) &err);
    
    // Create UART Device Lock
    OSMutexCreate((OS_MUTEX  *) &PC_UART_DEVICE,
                  (CPU_CHAR  *) "UART Device Mutex Lock",
                  &err);
    
    // Create Global Data Protect Lock    
    OSMutexCreate((OS_MUTEX  *) &GLOBAL_DATA_PROTECT,
                  (CPU_CHAR  *) "Global Data Mutex Lock",
                  &err);
    
    // Create CC1101 Task
    TaskCreate_CC1101();
    
    // Create PC Parse
    TaskCreate_PcParser();
    
#ifdef PC_SIMULATION
    TaskCreate_Simluate();
#endif
    
    while (DEF_TRUE) {

    //RC_PrintStr("Hello,world");
    LED_Triggle(0);

    OSTimeDlyHMSM( 0, 0, 0, 200,
                  OS_OPT_TIME_HMSM_STRICT, 
                  &err);
    }
}
