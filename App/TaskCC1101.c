#include <includes.h>

OS_TCB   AppTaskCC1101TCB;

static CPU_STK  AppTaskCC1101Stk[APP_TASK_CC1101_STK_SIZE];

static  void  AppTaskCC1101 (void *p_arg);

void TaskCreate_CC1101(void)
{
    OS_ERR      err;
    
    OSTaskCreate((OS_TCB     *)&AppTaskCC1101TCB,
                 (CPU_CHAR   *)"App Task CC1101",
                 (OS_TASK_PTR )AppTaskCC1101, 
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_CC1101_PRIO,
                 (CPU_STK    *)&AppTaskCC1101Stk[0],
                 (CPU_STK_SIZE)APP_TASK_CC1101_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_CC1101_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
}

static  void  AppTaskCC1101 (void *p_arg)
{
    OS_ERR  err;
    p_arg = p_arg;
    
    CC1101_Init();
    
    
    
    /*
    while(1)
    {
            OSTimeDlyHMSM( 0, 0, 0, 50,
                  OS_OPT_TIME_HMSM_STRICT, 
                  &err);
    }
    */
}