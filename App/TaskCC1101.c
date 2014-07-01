#include <includes.h>

OS_TCB   AppTaskCC1101TCB;

static CPU_STK  AppTaskCC1101Stk[APP_TASK_CC1101_STK_SIZE];

static  void  AppTaskCC1101 (void *p_arg);

static void TIMER4_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    TIM_DeInit(TIM4);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseStructure.TIM_Period      = 20000;
    TIM_TimeBaseStructure.TIM_Prescaler   = 7200-1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM4, ENABLE);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel            = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

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
                 (OS_MSG_QTY  )4,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
}

static  void  AppTaskCC1101 (void *p_arg)
{
    OS_ERR      err;
    uint8_t     retval;
    void        *p_msg;
    uint8_t     *p_data;
    uint8_t     datalen;
    CPU_TS      ts;
    OS_MSG_SIZE msg_size;
    uint16_t    cmd_id;

    p_arg = p_arg;
    
    retval = CC1101_Init();
    if(0 != retval)
    {
        while(1)
        {
            OSTimeDlyHMSM( 0, 0, 0, 50,
                           OS_OPT_TIME_HMSM_STRICT, 
                           &err);
            LED_Triggle(0);
        }
    }
    
    TIMER4_Init();
    
    while(1)
    {
        p_msg = OSTaskQPend(0,
                            OS_OPT_PEND_BLOCKING,
                            &msg_size,
                            &ts,
                            &err);

        // Msg Parser
        cmd_id = *(uint16_t*)((uint8_t*)p_msg+2);
        switch(cmd_id)
        {
            case CMD_PROPERTY:
                {
                    OSMutexPend((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                                (OS_TICK     )0,
                                (OS_OPT      )OS_OPT_PEND_BLOCKING,
                                (CPU_TS     *)&ts,
                                (OS_ERR     *)&err);
                    memcpy((void*)&(g_Cmd_CarInfo.data), (void*)((uint8_t*)p_msg+4), sizeof(CAR_INFO));
                    OSMutexPost((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                                (OS_OPT      )OS_OPT_POST_NONE,
                                (OS_ERR     *)&err);
                    break;
                }
                
            case CMD_STATUS:
                {
                    OSMutexPend((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                                (OS_TICK     )0,
                                (OS_OPT      )OS_OPT_PEND_BLOCKING,
                                (CPU_TS     *)&ts,
                                (OS_ERR     *)&err);
                    memcpy((void*)&(g_Cmd_CarStatus.data), (void*)((uint8_t*)p_msg+4), sizeof(CAR_STATUS));
                    OSMutexPost((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                                (OS_OPT      )OS_OPT_POST_NONE,
                                (OS_ERR     *)&err);
                    break;
                }

            case CMD_ATTACKED:
                {
                    g_Cmd_CarAttacked.seq_number.value = (uint16_t) 0x0000;
                    g_Cmd_CarAttacked.cmd_id           = (uint16_t) CMD_ATTACKED;
                    memcpy((void*)&(g_Cmd_CarAttacked.data), (void*)((uint8_t*)p_msg+4), sizeof(ATTACKED_INFO));
                    datalen = FramePack((uint8_t *)&(g_Cmd_CarAttacked.seq_number.value),
                              sizeof(SEQ_NUM) + sizeof(uint16_t) + sizeof(ATTACKED_INFO), (uint8_t *)&g_Cmd_CarAttacked);

                    if(datalen != 0)
                    { 
                        PC_PrintBlock((uint8_t *)&(g_Cmd_CarAttacked), sizeof(ATTACKED_INFO_T));
                    }
    
                    break;
                }
            case CMD_EQUIMENT:
                {
                    g_Cmd_CarEquiment.seq_number.value = (uint16_t) 0x0000;
                    g_Cmd_CarEquiment.cmd_id           = (uint16_t) CMD_EQUIMENT;
                    memcpy((void*)&(g_Cmd_CarEquiment.data), (void*)((uint8_t*)p_msg+4), sizeof(EQUIMENT_INFO));
                    datalen = FramePack((uint8_t *)&(g_Cmd_CarEquiment.seq_number.value),
                              sizeof(SEQ_NUM) + sizeof(uint16_t) + sizeof(EQUIMENT_INFO), (uint8_t *)&g_Cmd_CarEquiment);

                    if(datalen != 0)
                    { 
                        PC_PrintBlock((uint8_t *)&(g_Cmd_CarEquiment), sizeof(EQUIMENT_INFO_T));
                    }
                    
                    break;
                }

            default:
                {                    
                    break;
                }
        }

        // Release Msg Memory
        OSMemPut((OS_MEM  *)&RF_Msg,
                 (void    *)p_msg,
                 (OS_ERR  *)&err);
    }
}