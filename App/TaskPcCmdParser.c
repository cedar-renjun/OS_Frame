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
            LED_Triggle(0);
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
#if 0
    CAR_INFO_T     Cmd_CarInfo;    
    Cmd_CarInfo.seq_number.value = *(uint16_t*)pData;
    Cmd_CarInfo.cmd_id           = CMD_PROPERTY;
    Cmd_CarInfo.data.car_id      = CAR_ID_A_0;
    Cmd_CarInfo.data.weapon      = WEAPON_GUN_BB;
    Cmd_CarInfo.data.blood       = 10000;
    memcpy((void*)&(Cmd_CarInfo.data.name[0]), CAR_NAME, sizeof(CAR_NAME));
#else
    g_Cmd_CarInfo.seq_number.value = *(uint16_t*)pData;
    g_Cmd_CarInfo.cmd_id           = CMD_PROPERTY;
#endif
    datalen = FramePack((uint8_t *)&(g_Cmd_CarInfo.seq_number.value),
                        sizeof(SEQ_NUM) + sizeof(uint16_t) + sizeof(CAR_INFO), (uint8_t *)&g_Cmd_CarInfo);

    OSMemPut((OS_MEM  *)&PC_Msg,
             (void    *)pData,
             (OS_ERR  *)&err);
    
    if(datalen != 0)
    {
        PC_PrintBlock((uint8_t *)&(g_Cmd_CarInfo), sizeof(CAR_INFO_T));
    }
}

static void Parser_status (void* pData)
{
    uint8_t        datalen;
    OS_ERR         err;
#if 0
    CAR_STATUS_T   Cmd_CarStatus;
#ifdef PC_DEMO
    static uint16_t index = 0;
    index++;
    Cmd_CarStatus.seq_number.value  = *(uint16_t*)pData;
    Cmd_CarStatus.cmd_id            = CMD_STATUS;
    Cmd_CarStatus.data.power        = index%100;
    Cmd_CarStatus.data.bullet       = index%100;
    Cmd_CarStatus.data.blood_cur    = (index*100)%10000;
    Cmd_CarStatus.data.position.x   = index%100;
    Cmd_CarStatus.data.position.y   = index%100;
    Cmd_CarStatus.data.speed.x      = index%100;
    Cmd_CarStatus.data.speed.y      = index%100;
    Cmd_CarStatus.data.speed.r      = index%100;
    Cmd_CarStatus.data.linkquality  = index%100;
#else    
    Cmd_CarStatus.seq_number.value  = *(uint16_t*)pData;
    Cmd_CarStatus.cmd_id            = CMD_STATUS;
    Cmd_CarStatus.data.power        = 100;
    Cmd_CarStatus.data.bullet       = 100;
    Cmd_CarStatus.data.blood_cur    = 5000;
    Cmd_CarStatus.data.position.x   = 10;
    Cmd_CarStatus.data.position.y   = 20;
    Cmd_CarStatus.data.speed.x      = 30;
    Cmd_CarStatus.data.speed.y      = 40;
    Cmd_CarStatus.data.speed.r      = 50;
    Cmd_CarStatus.data.linkquality  = 60;
#endif
    
#else
    g_Cmd_CarStatus.seq_number.value  = *(uint16_t*)pData;
    g_Cmd_CarStatus.cmd_id            = CMD_STATUS;
#endif

    datalen = FramePack((uint8_t *)&(g_Cmd_CarStatus.seq_number.value),
                        sizeof(SEQ_NUM) + sizeof(uint16_t) + sizeof(CAR_STATUS), (uint8_t *)&g_Cmd_CarStatus);

    OSMemPut((OS_MEM  *)&PC_Msg,
             (void    *)pData,
             (OS_ERR  *)&err);
        
    if(datalen != 0)
    {
        PC_PrintBlock((uint8_t *)&(g_Cmd_CarStatus), sizeof(CAR_STATUS_T));
    }
}

static void Parser_control (void* pData)
{
    uint8_t i = 0;
    OS_ERR         err;
    CONTROL_INFO_T Cmd_Control;
    uint8_t * PacketBuff = (uint8_t*)pData;
    
    Cmd_Control.header = (uint16_t)0xBB55;
    memcpy((void*)&(Cmd_Control.data), (void*)&PacketBuff[4], sizeof(CONTROL_INFO));
    Cmd_Control.checksum = PacketBuff[4];
    
    // Calulate XOR checksum
    for(i = 1; i < sizeof(CONTROL_INFO); i++)
    {
        Cmd_Control.checksum ^= PacketBuff[4+i];
    }

    OSMemPut((OS_MEM  *)&PC_Msg,
             (void    *)pData,
             (OS_ERR  *)&err);

    //RC_PrintBlock((uint8_t *)&(Cmd_Control), sizeof(CONTROL_INFO_T));
    //PC_PrintBlock((uint8_t *)&(Cmd_Control), sizeof(CONTROL_INFO_T));
}

static void Parser_ping (void* pData)
{
    OS_ERR         err;
    uint8_t        datalen;
    PING_T         Cmd_Ping;
    
#ifdef PC_DEMO
    static uint16_t index = 0;
#endif

    Cmd_Ping.seq_number.value = *(uint16_t*)pData;
    Cmd_Ping.cmd_id           = CMD_PING;
#ifndef PC_DEMO
    Cmd_Ping.data.status      = ERR_NONE;
#else    
    Cmd_Ping.data.status      = index++%3;
#endif
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