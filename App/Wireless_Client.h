
#ifndef __WIRELESS_CLIENT_H_
#define __WIRELESS_CLIENT_H_

#ifdef  __cpluscplus
extern "C"
{
#endif

#include "stm32f10x.h"

extern PING_T          g_Cmd_Ping;
extern CAR_INFO_T      g_Cmd_CarInfo;
extern CAR_STATUS_T    g_Cmd_CarStatus;
extern ATTACKED_INFO_T g_Cmd_CarAttacked;
extern EQUIMENT_INFO_T g_Cmd_CarEquiment;

#ifdef  __cpluscplus
}
#endif

#endif  // __WIRELESS_CLIENT_H_
