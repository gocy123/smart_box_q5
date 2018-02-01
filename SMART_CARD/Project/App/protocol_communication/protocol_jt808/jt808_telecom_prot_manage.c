#include "user_system_cfg.h"

#ifdef USE_PROTOCOL_JT808

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "os.h"
#ifdef USE_SIM800_MODEM
#include "modem_sim800_app.h"
#endif
#ifdef USE_SIM6320_MODEM
#include "modem_sim6320_app.h"
#endif

#ifdef USE_MC20_MODEM
#include "modem_MC20_app.h"
#endif


#include "jt808_telecom_prot_manage.h"
#include "user_debug_app.h"

#include "os_cfg_app.h"			//TCB,STACK,PRIORITY,任务间消息的结构体定义和宏


#include "jt808_protocol_rx.h"
#include "jt808_protocol_tx.h"
#include "jt808_pt_misc.h"


static USER_AIR_PROTOCOL_NET_MANAGE_T AirProtocol_link_manager={0};

//----终端登录状态标志位1登录，0未登录---
uint8_t get_already_login_flag(void)
{
	if(AIR_PT_STA_LOGIN_OK == AirProtocol_link_manager.link_state )
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

static void telecom_prot_manage_task_msg_handle(TASK_MSG_T *p_msg)
{
	OS_ERR os_err;
	uint8_t error_flag=0;
	uint8_t temp;
	switch(p_msg->msg_father_type)
	{
		case FATHER_MSG_T_TASK_POWER_CTRL:
			if(POWER_CTRL_PWR_ON == p_msg->msg_son_type)
			{
				InfoPrintf("report task rx ctl msg: pwr on\r\n");
			}
			else if(POWER_CTRL_SLEEP== p_msg->msg_son_type)
			{
				user_delay_ms(TIME_50ms);
				InfoPrintf("report task rx ctl msg: sleep\r\n");
				AirProtocol_link_manager.link_state = AIR_PT_STA_CONN_OFF;
				OSTmrStop(&period_report_tmr,OS_OPT_TMR_NONE,NULL,&os_err);
				OSTaskSuspend(NULL,&os_err);
			}
			else if(POWER_CTRL_WAKEUP== p_msg->msg_son_type )
			{
				InfoPrintf("report task rx ctl msg: wake up\r\n");
				change_report_period(get_report_period());
			}
			else
			{
				error_flag=2;
			}
			break;
		case FATHER_MSG_T_MODEM_SOCKET_STATE:
			InfoPrintf("report task rx msg:socket state=%u\r\n",p_msg->msg_son_type); 
			temp = p_msg->msg_son_type;
			if(temp==0)
			{
				AirProtocol_link_manager.link_state=AIR_PT_STA_CONN_OFF;
				AirProtocol_link_manager.login_step=0;
				AirProtocol_link_manager.logout_step=0;
			}
			else if(temp==1)
			{
				AirProtocol_link_manager.link_state=AIR_PT_STA_SOCKET_CREATE_OK;
				AirProtocol_link_manager.login_step=1;
			}		
			break;
		case FATHER_MSG_T_ACC_STATUS_INDICATION:
			//InfoPrintf("report task rx msg:ACC=%u\r\n",p_msg->msg_son_type);
			break;
		case FATHER_MSG_T_REPORT_DATA:
			//可用于警情上报 等等， 及时上报，不需进入缓冲池
			break;
		case FATHER_MSG_T_ACK_TO_SERVER_SET:
			//暂时屏蔽
			//InfoPrintf("report task rx msg:ack to server set,cmd=0x%04X,result=%d\r\n",p_msg->msg_son_type,p_msg->result);
			/*ack_server_set_param(p_msg->serial,p_msg->msg_son_type,p_msg->result);

			if(DN_SET_REPORT_PERIOD_MSG_ID == p_msg->msg_son_type)
			{
				change_report_period(get_report_period());
			}*/
			break;
		case FATHER_MSG_T_ACK_TO_SERVER_QRY:
			//暂时屏蔽
			//InfoPrintf("report task rx msg:ack to server qry\r\n");
			//ack_server_query_param(p_msg->serial,p_msg->msg_son_type);
		break;

		default:
			error_flag=5;
			break;
		
	}
	
	if(error_flag!=0)
	{
		InfoPrintf("report task rx err msg:---%d---!!!\r\n",error_flag);
	}
}

#define ______COMMUNICATION__TASK__________________________________________________

void telecom_protocol_manage_task(void*p_para)
{
	OS_ERR os_err;
	static uint16_t msg_size;
	uint32_t last_time=0;
	static uint32_t last_prot_time=0;
	//uint8_t i;
	//uint8_t *ptr;
	TASK_MSG_T *p_msg;

	p_para = p_para;
	//=====read the period report time
	
	#if 1
	OSTaskCreate((OS_TCB     *)&remote_receive_task_Tcb,
			     (CPU_CHAR   *)((void *)"remote receive Task"),
			     (OS_TASK_PTR )remote_receive_task,
			     (void       *)0,
			     (OS_PRIO     )REMOTE_RECEIVE_TASK_PRIO,
			     (CPU_STK    *)remote_receive_task_stk,
			     (CPU_STK_SIZE)(REMOTE_RECEIVE_TASK_STK_SIZE/10),
			     (CPU_STK_SIZE)REMOTE_RECEIVE_TASK_STK_SIZE,
			     (OS_MSG_QTY  )REMOTE_RECEIVE_TASK_MSG_QTY,
			     (OS_TICK     )REMOTE_RECEIVE_TASK_TIME_QUANTA,
			     (void       *)0,
			     (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
			     (OS_ERR     *)&os_err);
	if(os_err!=OS_ERR_NONE)
	{
		return ;
	}
	else
	{
		InfoPrintf("remote receive Task created ok\r\n");
	}
	#endif

	#if 1
	OSTaskCreate(&remote_report_task_Tcb,
				"remote_report_task Tcb",
				(OS_TASK_PTR)    remote_report_task,
				(void          *)NULL,
				REMOTE_REPORT_TASK_PRIO,
				(CPU_STK 	*)  remote_report_task_stk,
				(CPU_STK_SIZE)  REMOTE_REPORT_TASK_STK_SIZE/10,/* limit*/
				(CPU_STK_SIZE)  REMOTE_REPORT_TASK_STK_SIZE,
				(OS_MSG_QTY)    REMOTE_REPORT_TASK_MSG_QTY,
				(OS_TICK )      REMOTE_REPORT_TASK_TIME_QUANTA,
				(void  *)       NULL,
				(OS_OPT)        (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				&os_err);	

	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf("creat remote_report task failed !!!\r\n");
	}
	#endif

	
	AirProtocol_link_manager.link_state = AIR_PT_STA_CONN_OFF;

	while(1)
	{			
		do
		{
			p_msg = (TASK_MSG_T *)OSTaskQPend(T_10MS*2,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&os_err);	
	        if(NULL != p_msg)
			{
				telecom_prot_manage_task_msg_handle(p_msg);
				user_free(p_msg,__FUNCTION__);
			}
			
		}while(NULL != p_msg);


		//-----log in && out manage----------------------------------------------------------
		if(AirProtocol_link_manager.login_step == 0)
		{
			last_prot_time=user_get_os_time();
		}
		
		if(AirProtocol_link_manager.login_step > 0)
		{//login ...
			AirProtocol_link_manager.logout_step=0;	
			
			if(AirProtocol_link_manager.login_step==1)
			{
				if(get_register_ok_flag()==0)
				{
					InfoPrintf("register.......\r\n");
					//if(jt808_pack_and_rep_register_frame()==0)
					if(1)
					{
						InfoPrintf("register ok\r\n");
						AirProtocol_link_manager.login_step=2;
						last_prot_time=user_get_os_time();
					}
					else
					{
						user_delay_ms(3000);
					}

					if(OSTimeGet(&os_err)-last_prot_time >= T_1MIN)
					{
						AirProtocol_link_manager.login_step=0;
						modem_ReCreat_socket();	
					}
				}
				else
				{
					AirProtocol_link_manager.login_step=2;
				}
				//AirProtocol_link_manager.login_step=2; //zgc
			}
			else if(AirProtocol_link_manager.login_step==2)
			{
				InfoPrintf("login........\r\n");
				if(jt808_pack_and_rep_login_frame()==0)
				{
					InfoPrintf("login ok\r\n");
					AirProtocol_link_manager.link_state = AIR_PT_STA_LOGIN_OK;
					AirProtocol_link_manager.login_step=0;
				}
				else
				{
					user_delay_ms(3000);
					InfoPrintf("telecom_protocol_manage task:login failed\r\n");
				}
			}
		}
		else if(AirProtocol_link_manager.logout_step > 0)
		{//req logout ...
			//sgmw_logout();
			AirProtocol_link_manager.link_state = AIR_PT_STA_LOGOUT_OK;	
			
			AirProtocol_link_manager.logout_step=0;	
			AirProtocol_link_manager.login_step=0;
		}

		if(user_get_os_time()-last_time>T_1S)
		{
			last_time=user_get_os_time();
			//InfoPrintf("telecom_protocol_manage task loop...\r\n");
		}
	}
}


#endif

