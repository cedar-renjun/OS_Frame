/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "includes.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
    
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
#if 0
void PendSV_Handler(void)
{
    //extern void OS_CPU_PendSVHandler(void);
    
}
#endif

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
#if 0
void SysTick_Handler(void)
{
    //extern void OS_CPU_SysTickHandler(void);
    OS_CPU_SysTickHandler();
}
#endif

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


void USART1_IRQHandler(void)
{
    extern FIFO_S_t* PC_Tx;
    
    uint8_t Ch;
    uint8_t MsgLen = 0;
    static uint8_t Membuf[50];
    OS_ERR err;

    OSIntEnter();
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        Ch = USART_ReceiveData(USART1);
        MsgLen = FrameUnpack(Ch, &Membuf[0]);
        if(MsgLen != 0)
        {
            CPU_INT08U * pMsg = (CPU_INT08U *)OSMemGet((OS_MEM  *)&PC_Msg,
                                                       (OS_ERR  *)&err);
            if(err == OS_ERR_NONE)
            {
                memcpy((void*)pMsg, (void*)&Membuf[0], MsgLen);
                // Send Message to Main Process
                OSTaskQPost((OS_TCB     *)&AppTaskPcParserTCB,
                            (void       *)pMsg,
                            (OS_MSG_SIZE )MsgLen,
                            (OS_OPT      )OS_OPT_POST_FIFO,
                            &err);
            }
        }
    }

    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {        
       if(FIFO_S_IsEmpty(PC_Tx))
       {
           USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
       }
       else
       {
           Ch = FIFO_S_Get(PC_Tx);
           USART_SendData(USART1, Ch);
       }
    }
    OSIntExit();
}

void TIM4_IRQHandler()
{
    if(TIM_GetITStatus(TIM4 , TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);
        GPIOA->ODR ^= GPIO_Pin_2;
        //while(1);
        CC1101_StrobeSend(STROBE_SIDLE);
        CC1101_StrobeSend(STROBE_SFRX);
        CC1101_StrobeSend(STROBE_SRX);
    }
}

void EXTI15_10_IRQHandler(void)
{    
    extern FIFO_t RF_Rx;
    uint8_t  i       = 0;
    uint8_t  MsgLen  = 0;
    OS_ERR   err     = 0;
    
    OSIntEnter();

    if(EXTI_GetITStatus(EXTI_Line11) != RESET)
    {   
        CPU_INT08U * pMsg = (CPU_INT08U *)OSMemGet((OS_MEM  *)&RF_Msg,
                                                   (OS_ERR  *)&err);
        EXTI_ClearITPendingBit(EXTI_Line11);
        MsgLen = CC1101_PacketRecv_ISR(pMsg, RF_MSG_SIZE);
        // Send Message to CC1101 Process
        OSTaskQPost((OS_TCB     *)&AppTaskCC1101TCB,
                    (void       *)pMsg,
                    (OS_MSG_SIZE )MsgLen,
                    (OS_OPT      )OS_OPT_POST_FIFO,
                    &err);

        TIM_SetCounter(TIM4, 0x0000);
        CC1101_StrobeSend(STROBE_SRX);
    }

    OSIntExit();
}

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
