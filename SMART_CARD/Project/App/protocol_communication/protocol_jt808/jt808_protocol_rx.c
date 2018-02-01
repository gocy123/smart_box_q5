#include "user_system_cfg.h"

#ifdef USE_PROTOCOL_JT808

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stdlib.h"


#include "os.h"
#include "os_cfg_app.h"			//TCB,STACK,PRIORITY,任务间消息的结构体定义和宏
//#include "rsa_main.h"
//#include "aes.h"

#ifdef USE_SIM800_MODEM
#include "modem_sim800_app.h"
#include "modem_sim800_AT_pro.h"
#endif

#ifdef USE_SIM6320_MODEM
#include "modem_sim6320_app.h"
#include "modem_sim6320_AT_pro.h"
#endif

#ifdef USE_MC20_MODEM
#include "modem_MC20_app.h"
#include "modem_MC20_AT_pro.h"
#endif


//#include "sgmw_telecom_prot_manage.h"
#include "user_debug_app.h"
#include "user_gps_app.h"
#include "user_rtc_app.h"


#include "jt808_protocol_rx.h"
#include "jt808_protocol_tx.h"
#include "jt808_pt_misc.h"

#define JT808_PT_APP_LAYER_BUF_MAX  300  
static uint8_t 	jt808_pt_app_layer_buf[JT808_PT_APP_LAYER_BUF_MAX];//未转义的完整包
static uint8_t 	jt808_DeEscaped_frame_buf[300]; //解析后的协议包
OS_Q 	rx_server_ack_q;
TASK_MSG_T 	upgrade_req_msg;

extern uint32_t SD_save_time 	; //SD卡保存时间0自动
extern uint8_t 	SD_save_status 	; //是否保存SD卡 0不保存 1保存
extern uint8_t 	report_TBC_type ; //tbc回传方式 1 DBC 2 TBC


int post_report_result(uint16_t serial,uint16_t cmd_id,uint8_t result)
{
	OS_ERR os_err;
	remote_result_q_t *ptr;

	ptr=(remote_result_q_t *)user_malloc(sizeof(remote_result_q_t),__FUNCTION__);
	if(ptr!=NULL)
	{
		ptr->cmd_id=REMOTE_RECV_TASK_ID;
		ptr->serial=serial;
		ptr->cmd_id=cmd_id;
		ptr->result=result;
		
		OSQPost(&rx_server_ack_q,(uint8_t *)ptr,sizeof(remote_result_q_t),OS_OPT_POST_FIFO,&os_err);
		
	    if(os_err == OS_ERR_NONE)
		{
			InfoPrintf("post rx_server_ack q, ok\r\n");
		}
		else
		{
			InfoPrintf("post rx_server_ack q, fail--%d--\r\n",os_err);
			user_free(ptr,__FUNCTION__);
		}
		
		return os_err;
	}
	else
	{
		InfoPrintf("post rx_server_ack q, malloc fail\r\n");
	}

	return -1;	
}


/*
 *****************************************************************************************
 *function name:int post_ack_to_server_q(uint16_t serial,uint16_t cmd_id,uint8_t result)
 * Description: 
 * Arguments  : ack_type:FATHER_MSG_T_ACK_TO_SERVER_SET,//应答服务器下发的设置结果
 *                       FATHER_MSG_T_ACK_TO_SERVER_QRY//应答服务器的查询结果
 *              serial:server's serial,cmd_id:server's cmd id,result:tbox's result 
 * Returns	  : 
 * create by:liaoxc 2015-11-24			
 *******************************************************************************************
 */
int post_ack_to_server_q(uint8_t ack_type,uint16_t serial,uint16_t cmd_id,uint8_t result)
{
	return 0;
}

/*
*********************************************************************************************************
*function name: post_update_param_msg
* Description: give a message 
* Arguments  : 
*
* Returns	 : 0	 If the call was successful
*		   -1	 If not
*Created by:	   newone 2015-12-31
*********************************************************************************************************
*/
int  post_update_param_msg(OS_TCB* pTCB,enum task_msg_src_id_t src_id,uint8_t msg_type,uint8_t cmd_id)
{
  return 0;

}


#define ___pro_rx_gprs_data________________________________________________________________________


/*****************************************
功能:异或校验
参数:*buf 校验起始位，Len 校验包的长度
返回:1->校验失败0->校验成功
作者:weicheng 2015.11.19
*******************************************/
uint8_t  jt808_xor_checkout_func(uint8_t *buf, uint16_t Len)
{
   uint16_t i;
   uint8_t checkNumber=0;
   for(i=1;i<Len-2;i++) //i=1 跳过包头，Len-2减去包尾和校验位
   {
      checkNumber ^= buf[i];
   }
   if(checkNumber == buf[Len-2])
   {
       return 0;
   }
   else //失败
   {
        return 1;
   }
}

uint32_t high_low_switch_32(uint32_t a)
{
	uint32_t b;
	b= ((a<<24)|((a&0x0000FF00)<<8)|((a&0x00FF0000)>>8)|(a>>24));
	return b;
}

uint16_t high_low_switch_16(uint16_t a)
{
	uint16_t b;
	b=((a<<8)|(a>>8));
	return b;
}


static void jt808_parse_one_rx_protocol_frame(uint8_t *buf, uint16_t len)
{
	//protocol format
	uint16_t msg_id;
	uint16_t msg_attribute;
	uint8_t  tm_phone_num[6]={0};
	uint16_t msg_sn;
	uint16_t multi_package_amount;
	uint16_t multi_package_sn;
	
	//user define
	JT808_PROTCOL_T *jt808_prot;
	uint8_t  multi_package_flag;
	uint8_t  *pdu_data;
	uint16_t pdu_data_len;
	int i;

	InfoPrintf("接收空中协议帧:");
	for(i=0;i<len;i++)
	{
		InfoPrintf("%02X,",buf[i]);
	}	
	InfoPrintf("\r\n");

	jt808_prot=(JT808_PROTCOL_T *)buf;
	
	msg_id=jt808_prot->msg_id;
	msg_id=high_low_switch_16(msg_id);

	msg_attribute=jt808_prot->msg_attribute;
	msg_attribute=high_low_switch_16(msg_attribute);

	memcpy(tm_phone_num,jt808_prot->tm_phone_num,6);
	
	msg_sn=jt808_prot->msg_sn;
	msg_sn=high_low_switch_16(msg_sn);

	if(((msg_attribute>>13)&0x0001)!=0)
	{
		multi_package_flag=1;
	}
	else
	{
		multi_package_flag=0;
	}

	pdu_data_len= msg_attribute&0x01FF;

	if(multi_package_flag==1)
	{
		multi_package_amount=*((uint16_t *)(jt808_prot->pdu));
		multi_package_amount=high_low_switch_16(multi_package_amount);
		
		multi_package_sn=*((uint16_t *)(jt808_prot->pdu+2));
		multi_package_sn=high_low_switch_16(multi_package_sn);

		pdu_data=(jt808_prot->pdu+4);
	}
	else
	{
		pdu_data=jt808_prot->pdu;
	}

	InfoPrintf("RX:msg id=%04X\r\n",msg_id);

	switch (msg_id)
	{
		case SV_COM_ACK_ID:
			jt808_com_ack_pro(pdu_data,pdu_data_len);
			break;
		case SV_ACK_TM_REG_ID:	
          	InfoPrintf("注册应答\r\n");
			jt808_ack_reg_pro(pdu_data,pdu_data_len);
			break;
		case SV_SET_TM_PARA_ID:					
			break;
		case SV_GET_TM_PARA_ID:				
			break;
		case SV_CTL_TM_ID:						
			break;
		case SV_GET_TM_GPS_INFO_ID:				
			break;
		case SV_TEMP_CTL_TRACK_GPS_ID:			
			break;
		case SV_EVENT_SET_ID:						
			break;
		case SV_ASK_QUESTION_ID:					
			break;
		case SV_INOF_PLAY_MENU_SET_ID:			
			break;
		case SV_INFO_SERVICE_ID:					
			break;
		case SV_PHONE_CALL_BACK_ID:				
			break;
		case SV_SET_TELEPHONE_BOOT_ID:			
			break;
		case SV_VEHICLE_CTL_ID:					
			break;
		case SV_SET_CIRCLE_AREA_ID:				
			break;
		case SV_DELETE_CIRCULAR_AREA_ID:			
			break;
		case SV_SET_RECTANGLE_AREA_ID:			
			break;
		case SV_DELETE_RECTANGLE_AREA_ID:			
			break;
		case SV_SET_POLYGON_AREA_ID:				
			break;
		case SV_DELETE_POLYGON_AREA_ID:			
			break;
		case SV_SET_ROUTE_ID:						
			break;
		case SV_DELETE_ROUTE_ID:					
			break;
		case SV_SAMPLE_TRAVEL_INFO_ID:			
			break;
		case SV_SET_TRAVEL_INFO_REC_PARA_ID:		
			break;
		case SV_ACK_MULTI_MEDIA_DATA_REP_ID:		
			break;
		case SV_CTL_TAKING_ONE_PHOTO_ID:			
			break;
		case SV_MULTI_MEDIA_DATA_INDEXING_ID:		
			break;
		case SV_STORED_MEDIA_DATA_REP_ID:			
			break;
		case SV_START_RECORDING_ID:				
			break;
		case SV_DATA_DOWN_DIRECTLY_ID:			
			break;
		case SV_RSA_PUBLIC_KEY_ID:	
			break;
		default:
			break;
	}
	
}
/*	
brief: extract one frame from the app layer cache buffer

	input :
			app_buf      : wait for extract buf
			buf_valid_len: wait buf`s len
	output:
			extracted_frame_len: extracted frame`s length; 
			if >0 ,means extracted one frame successfully (important !!!)
			app caller should take care of this output parameter.
	return:
			-1		: error calling
			others	: need_shift_out_amount
*/
static int jt808_extract_one_rx_prtocol_frame(uint8_t *app_buf,
  											  int buf_valid_len,
  											  int *extracted_frame_len)
{
	int i;

	//-------------------------------
	for(i=0;i<buf_valid_len;i++)
	{
		if(JT808_PROTOCOL_FIX==app_buf[i])
		{
			break;
		}
	}

	if(0 != i)
	{
		//找到HEAD,但在HEAD之前有乱码数据,先左移掉
		*extracted_frame_len=0;
		return i;
	}

	//------------------------------
	for(i=1;i<buf_valid_len;i++)
	{
		if(JT808_PROTOCOL_FIX==app_buf[i])
		{
			break;
		}
	}

	if(buf_valid_len==i)
	{
		*extracted_frame_len=0;
		return 0;
	}

	if(i >= JT808_PT_FRAME_MININUM_LEN)
	{//ok
		*extracted_frame_len=(i+1);
		return (i+1);
	}
	else
	{//mininum len error
		*extracted_frame_len=0;
		return (i+1)-1;//retain the second JT808_PROTOCOL_FIX,it may be the HEAD_BYTE of next frame
	}
}

uint8_t jt808_analysis_modem_gprs_q(uint8_t * ptr,uint16_t len)
{
	//int i;
	uint16_t ret;
	
	uint16_t DeEscaped_len;
	
	static int remain_len=0;	
	int extractOut_buf_len=0;
	int shift_len=0; 
	
	uint16_t input_len;
	uint16_t max_allow_copy_len;
	uint16_t one_time_copy_len;

	input_len=len;

	while(1)
	{
		max_allow_copy_len= JT808_PT_APP_LAYER_BUF_MAX-remain_len;
		if(input_len <= max_allow_copy_len)
		{
			one_time_copy_len=input_len;
			input_len-=input_len;
		}
		else
		{
			one_time_copy_len=max_allow_copy_len;
			input_len-=max_allow_copy_len;
		}

		if(0==one_time_copy_len)
		{
			break;
		}

		memcpy(jt808_pt_app_layer_buf+remain_len,ptr,one_time_copy_len);

		remain_len += one_time_copy_len;
		
		while(1)	
		{
			if(remain_len>=JT808_PT_FRAME_MININUM_LEN)//15:mininum of one jt808 frame 
			{
				shift_len=jt808_extract_one_rx_prtocol_frame(jt808_pt_app_layer_buf,remain_len,&extractOut_buf_len);
				
				if(shift_len>0)
				{
					if(extractOut_buf_len>0)
					{	
						DeEscaped_len = jt808_DeEscape_package(jt808_DeEscaped_frame_buf,
															  jt808_pt_app_layer_buf,
															  extractOut_buf_len);//反转义
						
						ret = jt808_xor_checkout_func(jt808_DeEscaped_frame_buf,DeEscaped_len);
						if(0==ret)
						{
							jt808_parse_one_rx_protocol_frame(jt808_DeEscaped_frame_buf,DeEscaped_len);
						}
						else
						{
							//校验错误
							InfoPrintf("GPRS帧校验错误\r\n");
							shift_len -= 1;//retain the last HEAD_BYTE, it may be the head of next frame.
						}
					}
					else
					{

					}
					
					remain_len-=shift_len;
					
					memmove(jt808_pt_app_layer_buf,jt808_pt_app_layer_buf+shift_len,remain_len);
				}
				else
				{
					break;
				}
			}
			else
			{
				//rx bytes amount is not equal or more than the protocol frame mininum 
				break;
			}
		}
	}

	return 0;
}

#define ___TASK________________________________A_______________________

void *pend_recv_gprs_data_q(uint16_t wait_time,uint16_t *msg_size)
{
	uint8_t *ptr;
	OS_ERR os_err;
	ptr=(uint8_t *)OSQPend(&modem_receive_gprs_data_q,wait_time,OS_OPT_PEND_BLOCKING,msg_size,NULL,&os_err);
	if(os_err==OS_ERR_NONE)
	{
		return ptr;
	}

	if(ptr!=NULL)
	{
		user_free(ptr,__FUNCTION__);
	}
	return NULL;
}

void remote_receive_task(void *p_para)
{
	uint8_t * p_buf = NULL;
	OS_ERR os_err; 
	uint16_t msg_size;
	p_para = p_para;
	
	OSQCreate(&rx_server_ack_q,"rx_server_ack q;",4,&os_err);
	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf("create rx_server_ack q fail\r\n");
	}

	while(1)
	{
		p_buf = pend_recv_gprs_data_q(1000,&msg_size);
		if(p_buf != NULL)
		{
			jt808_analysis_modem_gprs_q(p_buf,msg_size);
			user_free(p_buf,__FUNCTION__);
			OSSchedRoundRobinYield(&os_err);
		}
		else if(os_err != OS_ERR_TIMEOUT)
		{
			user_delay_ms(TIME_100ms);
		}
	}
}

#endif

