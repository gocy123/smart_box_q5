
/**
  ******************************************************************************
  * @file    USART/Printf/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
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
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include <stdio.h>

#include "platform_config.h"
#include "heads.h"

#include "os.h"
#include "os_cfg_app.h"



int main(void)
{
    OS_ERR os_err;
	
	OSInit(&os_err);
	#if 1
	OSTaskCreate((OS_TCB     *)&DeviceManageTask_Tcb,
			     (CPU_CHAR   *)((void *)"device manage Task"),
			     (OS_TASK_PTR )DeviceManageTask,
			     (void       *)0,
			     (OS_PRIO     )DEVICE_MANAGE_TASK_PRIO,
			     (CPU_STK    *)DeviceManageTask_Stk,
			     (CPU_STK_SIZE)(DEVICE_MANAGE_TASK_STK_SIZE/10),
			     (CPU_STK_SIZE)DEVICE_MANAGE_TASK_STK_SIZE,
			     (OS_MSG_QTY  )DEVICE_MANAGE_TASK_MSG_QTY,
			     (OS_TICK     )20,
			     (void       *)0,
			     (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			     (OS_ERR     *)&os_err);
	if(os_err!=OS_ERR_NONE)
	{
		//return 0;
		os_err=os_err;
	}
	else
	{
		//BlockPrintf("device manage Task created ok\r\n");
	}
	#endif

	
    OS_CPU_SysTickInit((SystemCoreClock / OS_CFG_TICK_RATE_HZ)); 
    OSStart(&os_err);

    while(1);
	//return 0;
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
