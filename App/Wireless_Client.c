#include "includes.h"

CAR_INFO_T      g_Cmd_CarInfo;
CAR_STATUS_T    g_Cmd_CarStatus;
ATTACKED_INFO_T g_Cmd_CarAttacked;
EQUIMENT_INFO_T g_Cmd_CarEquiment;


void RF_Client_Init(void)
{
    // clear all varial memory
    memset((void*)&g_Cmd_CarInfo,     0, sizeof(CAR_INFO_T));
    memset((void*)&g_Cmd_CarStatus,   0, sizeof(CAR_STATUS_T));
    memset((void*)&g_Cmd_CarAttacked, 0, sizeof(ATTACKED_INFO_T));
    memset((void*)&g_Cmd_CarEquiment, 0, sizeof(EQUIMENT_INFO_T));
}
