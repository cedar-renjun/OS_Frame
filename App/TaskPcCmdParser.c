#include <includes.h>

OS_TCB   AppTaskPcParserTCB;

static CPU_STK  AppTaskPcParserStk[APP_TASK_PC_PARSER_STK_SIZE];

static  void  AppTaskPcParser (void *p_arg);

// Auxiliary Functions
static void Parser_property (void* pData);
static void Parser_status   (void* pData);
static void Parser_control  (void* pData);
static void Parser_ping     (void* pData);

// Global Data Struct

void TaskCreate_PcParser(void)
{
    OS_ERR      err;
    
    OSTaskCreate((OS_TCB     *)&AppTaskPcParserTCB,
                 (CPU_CHAR   *)"App Task Pc Parser",
                 (OS_TASK_PTR )AppTaskPcParser, 
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_PC_PARSER_PRIO,
                 (CPU_STK    *)&AppTaskPcParserStk[0],
                 (CPU_STK_SIZE)APP_TASK_PC_PARSER_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_PC_PARSER_STK_SIZE,
                 (OS_MSG_QTY  )5,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
}

static  void  AppTaskPcParser (void *p_arg)
{
    OS_ERR  err;
    void    *p_msg;
    CPU_TS  ts;
    OS_MSG_SIZE msg_size;
    uint16_t cmd_id;
        
    p_arg = p_arg;
    
    while(1)
    {
        p_msg = OSTaskQPend(0,
                            OS_OPT_PEND_BLOCKING,
                            &msg_size,
                            &ts,
                            &err);
        if(OS_ERR_NONE == err)
        {
#ifdef DEBUG
            //LED_Triggle(0);
            PC_PrintBlock((uint8_t *)p_msg, msg_size);
            OSMemPut((OS_MEM  *)&PC_Msg,
                     (void    *)p_msg,
                     (OS_ERR  *)&err);
#else
            cmd_id = *(uint16_t*)((uint8_t*)p_msg+2);
            switch(cmd_id)
            {
                case CMD_PROPERTY:
                    {
                        Parser_property(p_msg);
                        break;
                    }
                    
                case CMD_STATUS:
                    {
                        Parser_status(p_msg);
                        break;
                    }
    
                case CMD_CONTROL:
                    {
                        Parser_control(p_msg);
                        break;
                    }
                case CMD_PING:
                    {
                        Parser_ping(p_msg);
                        break;
                    }
                default:
                    {
                        // Unrecognized Cmd
                        OSMemPut((OS_MEM  *)&PC_Msg,
                                 (void    *)p_msg,
                                 (OS_ERR  *)&err);
#ifdef DEBUG
                        PC_PrintStr("Unrecognized cmd!\r\n");
#endif                        
                        break;
                    }
            }
#endif            
        }
    }
}

static void Parser_property (void* pData)
{
    uint8_t        datalen;
    OS_ERR         err;
    CPU_TS         ts;
    CAR_INFO_T     Cmd_CarInfo;

    OSMutexPend((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                (OS_TICK     )0,
                (OS_OPT      )OS_OPT_PEND_BLOCKING,
                (CPU_TS     *)&ts,
                (OS_ERR     *)&err);
    memcpy((void*)&Cmd_CarInfo, (void*)&g_Cmd_CarInfo, sizeof(CAR_INFO_T));
    OSMutexPost((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                (OS_OPT      )OS_OPT_POST_NONE,
                (OS_ERR     *)&err);
        
    Cmd_CarInfo.seq_number.value = *(uint16_t*)pData;
    Cmd_CarInfo.cmd_id           =  (uint16_t )CMD_PROPERTY;

    datalen = FramePack((uint8_t *)&(Cmd_CarInfo.seq_number.value),
                        sizeof(SEQ_NUM) + sizeof(uint16_t) + sizeof(CAR_INFO), (uint8_t *)&Cmd_CarInfo);

    OSMemPut((OS_MEM  *)&PC_Msg,
             (void    *)pData,
             (OS_ERR  *)&err);

    if(datalen != 0)
    {
        PC_PrintBlock((uint8_t *)&(Cmd_CarInfo), sizeof(CAR_INFO_T));
    }
}

static void Parser_status (void* pData)
{
    uint8_t        datalen;
    OS_ERR         err;
    CPU_TS         ts;
    CAR_STATUS_T   Cmd_CarStatus;

    OSMutexPend((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                (OS_TICK     )0,
                (OS_OPT      )OS_OPT_PEND_BLOCKING,
                (CPU_TS     *)&ts,
                (OS_ERR     *)&err);
    memcpy((void*)&Cmd_CarStatus, (void*)&g_Cmd_CarStatus, sizeof(CAR_STATUS_T));
    OSMutexPost((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                (OS_OPT      )OS_OPT_POST_NONE,
                (OS_ERR     *)&err);

    Cmd_CarStatus.seq_number.value  = *(uint16_t*)pData;
    Cmd_CarStatus.cmd_id            =  (uint16_t )CMD_STATUS;

    datalen = FramePack((uint8_t *)&(Cmd_CarStatus.seq_number.value),
                        sizeof(SEQ_NUM) + sizeof(uint16_t) + sizeof(CAR_STATUS), (uint8_t *)&Cmd_CarStatus);

    OSMemPut((OS_MEM  *)&PC_Msg,
             (void    *)pData,
             (OS_ERR  *)&err);

    if(datalen != 0)
    {
        PC_PrintBlock((uint8_t *)&(Cmd_CarStatus), sizeof(CAR_STATUS_T));
    }
}

static void Parser_control (void* pData)
{
    OS_ERR         err;
    CONTROL_INFO_T Cmd_Control;
    uint8_t        datalen;
    
    FramePack((uint8_t *)pData, sizeof(SEQ_NUM) + sizeof(uint16_t) + sizeof(CONTROL_INFO),
              (uint8_t *)&Cmd_Control);

    OSMemPut((OS_MEM  *)&PC_Msg,
             (void    *)pData,
             (OS_ERR  *)&err);

    RC_PrintBlock((uint8_t *)&(Cmd_Control), sizeof(CONTROL_INFO_T));
    //PC_PrintBlock((uint8_t *)&(Cmd_Control), sizeof(CONTROL_INFO_T));
}

static void Parser_ping (void* pData)
{
    OS_ERR         err;
    uint8_t        datalen;    
    PING_T         Cmd_Ping;
    CPU_TS         ts;
    
    OSMutexPend((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                (OS_TICK     )0,
                (OS_OPT      )OS_OPT_PEND_BLOCKING,
                (CPU_TS     *)&ts,
                (OS_ERR     *)&err);
    memcpy((void*)&Cmd_Ping, (void*)&g_Cmd_Ping, sizeof(PING_T));
    OSMutexPost((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                (OS_OPT      )OS_OPT_POST_NONE,
                (OS_ERR     *)&err);
    
    Cmd_Ping.seq_number.value = *(uint16_t*)pData;
    Cmd_Ping.cmd_id           =  (uint16_t)CMD_PING;
    
    datalen = FramePack((uint8_t *)&(Cmd_Ping.seq_number.value),
                        sizeof(SEQ_NUM) + sizeof(uint16_t) + sizeof(PING), (uint8_t *)&Cmd_Ping);

    OSMemPut((OS_MEM  *)&PC_Msg,
             (void    *)pData,
             (OS_ERR  *)&err);

    if(datalen != 0)
    { 
        PC_PrintBlock((uint8_t *)&(Cmd_Ping), sizeof(PING_T));
    }
}