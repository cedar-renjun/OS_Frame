
#include <includes.h>

#ifdef PC_SIMULATION

OS_TCB   AppTaskSimluateTCB;

static CPU_STK  AppTaskSimluateStk[APP_TASK_SIMLUATE_STK_SIZE];
static  void  AppTaskSimluate (void *p_arg);


void TaskCreate_Simluate(void)
{
    OS_ERR      err;

    OSTaskCreate((OS_TCB     *)&AppTaskSimluateTCB,
                 (CPU_CHAR   *)"App Task Simluate",
                 (OS_TASK_PTR )AppTaskSimluate, 
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_SIMLUATE_PRIO,
                 (CPU_STK    *)&AppTaskSimluateStk[0],
                 (CPU_STK_SIZE)APP_TASK_SIMLUATE_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_SIMLUATE_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
}

static void AppTaskSimluate(void *p_arg)
{
    OS_ERR  err;
    CPU_TS  ts;
    static uint16_t index = 0;
    p_arg = p_arg;
    
    memcpy((void*)&(g_Cmd_CarInfo.data.name), "Team A", sizeof("Team A"));
    g_Cmd_CarInfo.data.car_id  = CAR_ID_A_0;
    g_Cmd_CarInfo.data.weapon  = WEAPON_GUN_GAS;
    g_Cmd_CarInfo.data.blood   = 10000;
    
    while(1)
    {
        if(index++ > 101)
        {
            index = 0;
        }

        OSMutexPend((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                    (OS_TICK     )0,
                    (OS_OPT      )OS_OPT_PEND_BLOCKING,
                    (CPU_TS     *)&ts,
                    (OS_ERR     *)&err);

        // Ping Simulate
        g_Cmd_Ping.data.status          = index%3;
        
        // Status Simulate
        g_Cmd_CarStatus.data.power      = (uint8_t )(index%100);
        g_Cmd_CarStatus.data.bullet     = (uint8_t )(index%100);
        g_Cmd_CarStatus.data.blood_cur  = (uint16_t)((index*100)%10000);
        g_Cmd_CarStatus.data.position.x = (int8_t  )(index);
        g_Cmd_CarStatus.data.position.y = (int8_t  )(index); 
        g_Cmd_CarStatus.data.speed.x    = (int8_t  )(index);
        g_Cmd_CarStatus.data.speed.y    = (int8_t  )(index);
        g_Cmd_CarStatus.data.speed.r    = (int8_t  )(index);
        
        // Attacked Simulate
        g_Cmd_CarAttacked.data.weapon   = (uint8_t )(index%5);
        g_Cmd_CarAttacked.data.point    = (uint8_t )(index%5);
        g_Cmd_CarAttacked.data.blood    = (uint16_t)((index*200)%5000);

        // Equiment Simulate
        g_Cmd_CarEquiment.data.type     = (uint8_t )(index%4);
        g_Cmd_CarEquiment.data.blood    = (int16_t )(index*100);

        OSMutexPost((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                    (OS_OPT      )OS_OPT_POST_NONE,
                    (OS_ERR     *)&err);

        OSTimeDlyHMSM( 0, 0, 0, 100,
                       OS_OPT_TIME_HMSM_STRICT,
                       &err);
    }
}

#endif