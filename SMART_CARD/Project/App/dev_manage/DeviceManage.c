#include <stdlib.h>
#include <string.h>



//user app head file
#include "os.h"
#include "heads.h"
#include "os_cfg_app.h"

#include "stm32f10x.h"
#include "stm32f10x_it.h"

#include "user_rf_app.h"

void *user_malloc(unsigned int size,char const*caller)
{
	void *ptr;

	uint32_t check_size;
	uint32_t temp;

	if(size%8==0)
	{
		check_size=size;
	}
	else
	{
		temp=size%8;
		
		check_size=(8-temp)+size;
	}

	check_size+=8;

    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();

	ptr=malloc(check_size);
	if(ptr!=NULL)
	{
		memset(ptr,0,check_size);
		 CPU_CRITICAL_EXIT();	
        return ptr;
	} 
    else
	{
		 CPU_CRITICAL_EXIT();
		return NULL;
    }
}

void user_free(void *rmem,char const*caller)
{
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();


	if(rmem!=NULL)
	{
		free(rmem);
	}
	else
	{
		while(1);
	}
	
    CPU_CRITICAL_EXIT();
	//return ;
}

void user_delay_ms(uint32_t ms)
{
	OS_ERR os_err;
	OSTimeDly(ms, OS_OPT_TIME_DLY, &os_err);
}
/*return os running time*/
uint32_t user_get_os_time(void)
{
	OS_ERR os_err;
	return OSTimeGet(&os_err);
}

void RCC_Configuration(void)
{  
	RCC_DeInit();											//RCC系统复位
	RCC_HSICmd(ENABLE);										//使能内部高速振荡
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	//使能FLASH BUFFER
	FLASH_SetLatency(FLASH_Latency_2);						//Flash 2个等待周期
	RCC_HCLKConfig(RCC_SYSCLK_Div1);						//HCLK = SYSCLK
	RCC_PCLK2Config(RCC_HCLK_Div1);							//PCLK2 = HCLK
	RCC_PCLK1Config(RCC_HCLK_Div1);							//PCLK1 = HCLK
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)		//等待 HSI 就绪
	{

	}
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);					//选择 HSI 为系统时钟
	while(RCC_GetSYSCLKSource() != 0x00)					//等待系统时钟就绪
	{

	}
	
	RCC_APB1PeriphClockCmd(	  RCC_APB1Periph_TIM2
							| RCC_APB1Periph_TIM4
							| RCC_APB1Periph_PWR 
							| RCC_APB1Periph_BKP
							//| RCC_APB1Periph_USART2
							#ifdef ENABLE_PRINT
							| RCC_APB1Periph_USART3
							#endif
							,ENABLE
						  );

	RCC_APB2PeriphClockCmd(	  RCC_APB2Periph_GPIOA
							| RCC_APB2Periph_GPIOB
							| RCC_APB2Periph_USART1
							#ifdef EN_ADC1_CHN1
							| RCC_APB2Periph_ADC1
							#endif
							| RCC_APB2Periph_AFIO
							, ENABLE
						  );
	#ifdef EN_ADC1_CHN1
	RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
	#endif
	

}


/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure={0};

	///NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);	//设置向量表基址：0x08000000   

	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USARTz Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
  	/* Enable the USARTz Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	

	/* Configure and enable ADC interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	//定时器时间中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
}

void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef 		TIM_OCInitStructure;

	//TIM_TimeBaseStructure.TIM_Period = 32;				//重载值 8us
	TIM_TimeBaseStructure.TIM_Period = 32;				//重载值 8us
	TIM_TimeBaseStructure.TIM_Prescaler = 0;			//分频值PCLK/(X+1)
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;	//中央对齐模式1
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(LF125K_TIMx, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode 		= TIM_OCMode_PWM1;			//PWM输出模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_OutputNState= TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse 		= 16;						//占空比 50%
	TIM_OCInitStructure.TIM_OCPolarity 	= TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState= TIM_OCNIdleState_Reset;

	TIM_OC1Init(LF125K_TIMx, &TIM_OCInitStructure);	//LF信号发生器1
	TIM_OC1PreloadConfig(LF125K_TIMx, TIM_OCPreload_Enable);

	TIM_OC2Init(LF125K_TIMx, &TIM_OCInitStructure);	//LF信号发生器2
	TIM_OC2PreloadConfig(LF125K_TIMx, TIM_OCPreload_Enable);  

	TIM_ARRPreloadConfig(LF125K_TIMx, ENABLE);
	TIM_CtrlPWMOutputs(LF125K_TIMx, ENABLE);
	TIM_Cmd(LF125K_TIMx, DISABLE);   

	TIM_TimeBaseStructure.TIM_Period 		= U16_MAX;
	TIM_TimeBaseStructure.TIM_Prescaler 	= 15;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	//周期2us定时计数器
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		//清中断，以免一启用中断后立即产生中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//TIM_Cmd(TIM2, ENABLE);
}



void EXTI0_Config(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	/* Enable GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* Configure PA.00 pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* Connect EXTI0 Line to PA.00 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);

	/* Configure EXTI0 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void EXTI1_Config(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	/* Enable GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* Configure PA.00 pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* Connect EXTI0 Line to PA.00 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

	/* Configure EXTI0 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0E;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Can0ManageTask(void *p_arg)
{
	p_arg=p_arg;
	while(1)
		{
			user_delay_ms(3000);
			InfoPrintf("Can0ManageTask ok\r\n");
		}
}


void ISO15765Task(void *p_arg)
{
	p_arg=p_arg;
	while(1)
			{
				user_delay_ms(3000);
				InfoPrintf("ISO15765Task ok\r\n");
			}

}

void StorageTask(void *p_arg)
{
	p_arg=p_arg;
	while(1)
			{
				user_delay_ms(3000);
				InfoPrintf("StorageTask ok\r\n");
			}

}

void creat_app_task(void)
{
	OS_ERR os_err;

	#if 0
	OSTaskCreate((OS_TCB     *)&Can0ManageTask_Tcb,
			     (CPU_CHAR   *)((void *)"can0 manage Task"),
			     (OS_TASK_PTR )Can0ManageTask,
			     (void       *)0,
			     (OS_PRIO     )CAN0_MANAGE_TASK_PRIO,
			     (CPU_STK    *)Can0ManageTask_Stk,
			     (CPU_STK_SIZE)(CAN0_MANAGE_TASK_STK_SIZE/10),
			     (CPU_STK_SIZE)CAN0_MANAGE_TASK_STK_SIZE,
			     (OS_MSG_QTY  )CAN0_MANAGE_TASK_MSG_QTY,
			     (OS_TICK     )CAN0_MANAGE_TASK_TIME_QUANTA,
			     (void       *)0,
			     (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			     (OS_ERR     *)&os_err);
	if(os_err!=OS_ERR_NONE)
	{
		//return ;
	}
	else
	{
		//InfoPrintf("can0 manage Task created ok\r\n");
	}
	#endif
	
	#if 0
	OSTaskCreate((OS_TCB     *)&ModemManageTask_Tcb,
			     (CPU_CHAR   *)((void *)"modem manage Task"),
			     (OS_TASK_PTR )ModemManageTask,
			     (void       *)0,
			     (OS_PRIO     )MODEM_MANAGE_TASK_PRIO,
			     (CPU_STK    *)ModemManageTask_Stk,
			     (CPU_STK_SIZE)(MODEM_MANAGE_TASK_STK_SIZE/10),
			     (CPU_STK_SIZE)MODEM_MANAGE_TASK_STK_SIZE,
			     (OS_MSG_QTY  )MODEM_MANAGE_TASK_MSG_QTY,
			     (OS_TICK     )MODEM_MANAGE_TASK_TIME_QUANTA,
			     (void       *)0,
			     (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			     (OS_ERR     *)&os_err);
	if(os_err!=OS_ERR_NONE)
	{
		//return ;
	}
	else
	{
		//InfoPrintf("modem manage Task created ok\r\n");
	}
	#endif

	
	#if 0
	OSTaskCreate((OS_TCB     *)&telecom_prot_manage_task_Tcb,
			     (CPU_CHAR   *)((void *)"telecom_prot_manage task"),
			     (OS_TASK_PTR )telecom_protocol_manage_task,
			     (void       *)0,
			     (OS_PRIO     )TELECOM_PROTOCOL_MANAGE_TASK_PRIO,
			     (CPU_STK    *)telecom_prot_manage_task_stk,
			     (CPU_STK_SIZE)(TELECOM_PROT_MANAGE_TASK_STK_SIZE/10),
			     (CPU_STK_SIZE)TELECOM_PROT_MANAGE_TASK_STK_SIZE,
			     (OS_MSG_QTY  )TELECOM_PROT_MANAGE_TASK_MSG_QTY,
			     (OS_TICK     )TELECOM_PROT_MANAGE_TASK_TIME_QUANTA,
			     (void       *)0,
			     (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			     (OS_ERR     *)&os_err);
	if(os_err!=OS_ERR_NONE)
	{
		//return ;
	}
	else
	{
		//InfoPrintf("report manage task created ok\r\n");
	}
	#endif

	
	#if 0
	OSTaskCreate((OS_TCB     *)&GpsTask_Tcb,
			     (CPU_CHAR   *)((void *)"gps Task"),
			     (OS_TASK_PTR )GpsTask,
			     (void       *)0,
			     (OS_PRIO     )GPS_TASK_PRIO,
			     (CPU_STK    *)GpsTask_Stk,
			     (CPU_STK_SIZE)(GPS_TASK_STK_SIZE/10),
			     (CPU_STK_SIZE)GPS_TASK_STK_SIZE,
			     (OS_MSG_QTY  )GPS_TASK_MSG_QTY,
			     (OS_TICK     )GPS_TASK_TIME_QUANTA,
			     (void       *)0,
			     (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			     (OS_ERR     *)&os_err);
	if(os_err!=OS_ERR_NONE)
	{
		//return ;
	}
	else
	{
		//InfoPrintf("gps Task created ok\r\n");
	}
	#endif
	
	#if 1
	OSTaskCreate((OS_TCB     *)&rf_task_Tcb,
			     (CPU_CHAR   *)((void *)"rf Task"),
			     (OS_TASK_PTR )RfTask,
			     (void       *)0,
			     (OS_PRIO     )RF_TASK_PRIO,
			     (CPU_STK    *)rf_task_stk,
			     (CPU_STK_SIZE)(RF_TASK_STK_SIZE/10),
			     (CPU_STK_SIZE)RF_TASK_STK_SIZE,
			     (OS_MSG_QTY  )RF_TASK_MSG_QTY,
			     (OS_TICK     )RF_TASK_TIME_QUANTA,
			     (void       *)0,
			     (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			     (OS_ERR     *)&os_err);
	if(os_err!=OS_ERR_NONE)
	{
		//return ;
	}
	else
	{
		//InfoPrintf("rf Task created ok\r\n");
	}
	#endif

	
	#if 0
	OSTaskCreate((OS_TCB     *)&ISO15765Task_Tcb,
			     (CPU_CHAR   *)((void *)"ISO15765 Task"),
			     (OS_TASK_PTR )ISO15765Task,
			     (void       *)0,
			     (OS_PRIO     )ISO15765_TASK_PRIO,
			     (CPU_STK    *)ISO15765Task_Stk,
			     (CPU_STK_SIZE)(ISO15765_TASK_STK_SIZE/10),
			     (CPU_STK_SIZE)ISO15765_TASK_STK_SIZE,
			     (OS_MSG_QTY  )ISO15765_TASK_MSG_QTY,
			     (OS_TICK     )ISO15765_TASK_TIME_QUANTA,
			     (void       *)0,
			     (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			     (OS_ERR     *)&os_err);
	if(os_err!=OS_ERR_NONE)
	{
		//return ;
	}
	else
	{
		//InfoPrintf("ISO15765 Task created ok\r\n");
	}
	#endif

	
	#if 0
	OSTaskCreate((OS_TCB     *)&storage_task_Tcb,
			     (CPU_CHAR   *)((void *)"storage Task"),
			     (OS_TASK_PTR )StorageTask,
			     (void       *)0,
			     (OS_PRIO     )STORAGE_TASK_PRIO,
			     (CPU_STK    *)storage_task_stk,
			     (CPU_STK_SIZE)(STORAGE_TASK_STK_SIZE/10),
			     (CPU_STK_SIZE)STORAGE_TASK_STK_SIZE,
			     (OS_MSG_QTY  )STORAGE_TASK_MSG_QTY,
			     (OS_TICK     )STORAGE_TASK_TIME_QUANTA,
			     (void       *)0,
			     (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			     (OS_ERR     *)&os_err);
	if(os_err!=OS_ERR_NONE)
	{
		//return ;
	}
	else
	{
		//InfoPrintf("storage Task created ok\r\n");
	}
	#endif

}


void DeviceManageTaskMsg_handle(void)
{
	OS_ERR os_err;
	uint8_t *p_msg;
	OS_MSG_SIZE msg_size;
	p_msg=OSTaskQPend(TIME_0ms,OS_OPT_PEND_NON_BLOCKING,&msg_size,NULL,&os_err);
	if(p_msg != NULL)
	{
		user_free(p_msg,__FUNCTION__);	
	}
}



int DM_post_one_task_msg(OS_TCB *tcb,TASK_MSG_T task_msg)
{
    OS_ERR os_err;
	TASK_MSG_T *task_manage_msg=NULL;
	TASK_MSG_T temp_msg;
	
	temp_msg=task_msg;	
	
	task_manage_msg=(TASK_MSG_T *)user_malloc(sizeof(TASK_MSG_T),__FUNCTION__);
	if(task_manage_msg!=NULL)
	{
		memcpy((uint8_t *)task_manage_msg,(uint8_t *)(&temp_msg),sizeof(TASK_MSG_T));
		
		OSTaskQPost(tcb,task_manage_msg,sizeof(TASK_MSG_T),OS_OPT_POST_FIFO,&os_err);	
		if(OS_ERR_NONE == os_err)
		{
			return 0;
		}
		else
		{
			InfoPrintf("post task q fail -%d-,free...\r\n",os_err);
			user_free(task_manage_msg,__FUNCTION__);
			return -1;
		}
	}
	else
	{
		InfoPrintf("malloc task q fail\r\n");
		return -2;
	}
}

static void DM_post_pwr_on_to_tasks(void)
{
	//OS_ERR os_err;
	TASK_MSG_T task_manage_msg={0};

	task_manage_msg.msg_src_id		= DEV_MANAGE_TASK_ID;
	task_manage_msg.msg_father_type	= FATHER_MSG_T_TASK_POWER_CTRL;
	task_manage_msg.msg_son_type	= POWER_CTRL_PWR_ON;
	DM_post_one_task_msg(&GpsTask_Tcb,task_manage_msg);
	//DM_post_one_task_msg(&Can0ManageTask_Tcb,task_manage_msg);
	//DM_post_one_task_msg(&ModemManageTask_Tcb,task_manage_msg);
}

void DeviceManageTask(void *p_arg)
{
	OS_ERR os_err;

	p_arg=p_arg;

	RCC_Configuration();
	IWDG_Configuration();
	NVIC_Configuration();
	GPIO_Configuration();
	TIM_Configuration();
	//EXTI0_Config();
	//EXTI1_Config();
	debug_port_init();
	user_open_rtc();
	creat_app_task();
	DM_post_pwr_on_to_tasks();
	BEEP_ON;
	OSTimeDly(500, OS_OPT_TIME_DLY, &os_err);
	BEEP_OFF;
	while(1)
	{
		OSTimeDly(5000, OS_OPT_TIME_DLY, &os_err);
		//InfoPrintf("DeviceManageTask loop...\r\n");

		printf_time_now();


		/*do
		{
			ret=USART_GetFlagStatus(USART3,USART_FLAG_TXE);
		}while(ret==0);
		USART_SendData(USART3, 85);*/
	}
	

}



