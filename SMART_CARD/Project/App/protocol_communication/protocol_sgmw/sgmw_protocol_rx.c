#include "user_system_cfg.h"

#ifdef USE_PROTOCOL_SGMW
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stdlib.h"

#include "os.h"
#include "os_cfg_app.h"			//TCB,STACK,PRIORITY,�������Ϣ�Ľṹ�嶨��ͺ�
#include "rsa_main.h"
#include "aes.h"
#include "timer_define.h"

#include "modem_sim800_AT_pro.h"
#include "modem_sim800_app.h"
#include "sgmw_protocol_rx.h"
#include "sgmw_protocol_tx.h"
#include "sgmw_telecom_prot_manage.h"
#include "user_debug_app.h"
#include "user_gps_app.h"
#include "user_rtc_app.h"


/******************************************
����	:������յ�У��ɹ���Э���
����:*buf У����ʼλ��Len У����ĳ���
����:weicheng 2015.11.19
*******************************************/
typedef union {
	uint16_t msg_type;
	info_data_t st_msg_attr;
}MSG_INFO_DATA_T;


#define SGMW_PT_APP_LAYER_BUF_MAX  1100  
static uint8_t 	sgmw_pt_app_layer_buf[SGMW_PT_APP_LAYER_BUF_MAX];//δת���������
static uint8_t 	sgmw_DeEscaped_frame_buf[1024]; //�������Э���

OS_Q 	rx_server_ack_q;


TASK_MSG_T 	qry_ack_msg[ACK_MSG_MAX_NUM];
TASK_MSG_T 	upgrade_req_msg;

extern uint32_t SD_save_time 	; //SD������ʱ��0�Զ�
extern uint8_t 	SD_save_status 	; //�Ƿ񱣴�SD�� 0������ 1����
extern uint8_t 	report_TBC_type ; //tbc�ش���ʽ 1 DBC 2 TBC

/*
 *****************************************************************************************
 * Description: 
 * Arguments  : serial:tbox's serial,cmd_id:tbox's cmd id,result:server ack 
 *
 * Returns	  : 
 * create by:liaoxc 2015-11-24			
 *******************************************************************************************
 */
static int post_report_result(uint16_t serial,uint16_t cmd_id,uint8_t result)
{
	OS_ERR os_err;
   // static u8 idx=0;
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
			//InfoPrintf("post rx_server_ack q, ok\r\n");
		}
		else
		{
			InfoPrintf("post rx_server_ack q, fail--%d--\r\n",os_err);
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
 * Arguments  : ack_type:FATHER_MSG_T_ACK_TO_SERVER_SET,//Ӧ��������·������ý��
 *                       FATHER_MSG_T_ACK_TO_SERVER_QRY//Ӧ��������Ĳ�ѯ���
 *              serial:server's serial,cmd_id:server's cmd id,result:tbox's result 
 * Returns	  : 
 * create by:liaoxc 2015-11-24			
 *******************************************************************************************
 */
int post_ack_to_server_q(uint8_t ack_type,uint16_t serial,uint16_t cmd_id,uint8_t result)
{
	/*OS_ERR os_err;
	static uint8_t msg_index=0;
	
	qry_ack_msg[msg_index].src_id = REMOTE_RECV_TASK_ID; 
	qry_ack_msg[msg_index].msg_type = ack_type;
	qry_ack_msg[msg_index].cmd_id = cmd_id;
	qry_ack_msg[msg_index].serial = serial;
	qry_ack_msg[msg_index].result = result;

	OSTaskQPost(&tcb_report_manage_task,(void*)&qry_ack_msg[msg_index],sizeof(TASK_MSG_T), OS_OPT_POST_FIFO,&os_err);
	if(os_err!=0)
	{
		InfoPrintf("post_ack_to_server_q to report task faild -%d-\r\n",os_err);
	}

	msg_index++;
	if(msg_index >= ACK_MSG_MAX_NUM)
	{
		msg_index = 0;
	}
	return os_err;*/
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
	/*OS_ERR os_err;
	static uint8_t msg_indx=0;

	param_msg_tbl[msg_indx].src_id = src_id; 
	param_msg_tbl[msg_indx].msg_type = msg_type;//FATHER_MSG_T_REMOTE_UPDATE_PARAM;
	param_msg_tbl[msg_indx].cmd_id = cmd_id;
	
	OSTaskQPost(pTCB,(void*)&param_msg_tbl[msg_indx],sizeof(TASK_MSG_T), OS_OPT_POST_FIFO,&os_err);

	msg_indx++;
	if(msg_indx >= MAX_UPDATE_PARAM_MSG_NUM)
	{
		msg_indx = 0;
	}
	
	return os_err;*/
  return 0;

}


#define ___pro_rx_gprs_data________________________________________________________________________

/************************************
����	:��Э��ʱ��ת����RTCʱ��
����	:weicheng 2015.12.02 
*************************************/
static void sgmw_pt_time_transform_beijing_time(BEIJING_TIME_T *rtc_t,sgwm_protocol_time_format_t *time)
{
    time->re_report = 0;

	rtc_t->year 	= time->year_shi * 10 	+ time->year_ge +2000;
	rtc_t->month	= time->month_shi *10 	+ time->month_ge;
	rtc_t->day 		= time->day_shi * 10 	+ time->day_ge;		
	rtc_t->hour 	= time->hour_shi * 10 	+ time->hour_ge;				
	rtc_t->minute 	= time->minute_shi * 10 + time->minute_ge; 
	rtc_t->second 	= time->second_shi * 10 + time->second_ge;
}

/*****************************************
����:���У��
����:*buf У����ʼλ��Len У����ĳ���
����:1->У��ʧ��0->У��ɹ�
����:weicheng 2015.11.19
*******************************************/
uint8_t  sgmw_xor_checkout_func(uint8_t *buf, uint16_t Len)
{
   uint16_t i;
   uint8_t checkNumber=0;
   for(i=1;i<Len-2;i++) //i=1 ������ͷ��Len-2��ȥ��β��У��λ
   {
      checkNumber ^= buf[i];
   }
   if(checkNumber == buf[Len-2])
   {
       return 0;
   }
   else //ʧ��
   {
        return 1;
   }
}

static void sgmw_parse_one_rx_protocol_frame(uint8_t *buf, uint16_t Len)
{
	uint16_t rec_msg_id;
	uint16_t rec_serial;
	uint16_t rec_msg_len;
	sgwm_frame_t *report_t;
	uint16_t ack_msg_id;
	uint16_t ack_serial;
	uint8_t  ack_result=0;
	uint16_t i,j,k;
	//uint32_t postback_f=0; //�ش�Ƶ��
	//uint32_t heartbeat_f=0;//����Ƶ��
	BEIJING_TIME_T server_time; //RTCʱ��
	sgwm_protocol_time_format_t *protocol_format_time; //������ʱ��
	//FRESULT ret;
	//uint16_t port;
	//static  uint8_t  package_total=0;//���������ܰ���
	//static  uint8_t  send_package_serial=0; //���Ͱ����
	//unsigned char file_md5[16];//�ļ�MD5ֵ

	uint8_t mult_pack_f = 0; 
	uint8_t aes_encrypt_flag=0;
	
	MSG_INFO_DATA_T msg_attribution={0};

	
	report_t =(sgwm_frame_t *)buf;

	//������Ϣ��ID 
	rec_msg_id = report_t->st_head.msg_id;				

	
	//������Ϣ����Ϣ������
	msg_attribution.msg_type=htons(report_t->st_head.msg_type);	

	mult_pack_f=msg_attribution.st_msg_attr.multi_pack_flag;
	#if PROTOCOL_V1_0_2_3
	aes_encrypt_flag=msg_attribution.st_msg_attr.aes_flag;
	#endif
	rec_msg_len = msg_attribution.st_msg_attr.pdu_len;

	//������Ϣ����ˮ��
	rec_serial = report_t->st_head.serial;				


	//InfoPrintf("У��ɹ�...\r\n");
	
	//InfoPrintf("ƽ̨�·�(encryp):\r\n");
	//for(i=0;i<Len;i++)InfoPrintf("%02X,",buf[i]);
	//InfoPrintf("\r\n");


	if(aes_encrypt_flag==1)
	{
		//InfoPrintf("AES����...\r\n");
		i=rec_msg_len>>4;
		j=rec_msg_len%16;

		if(j!=0)
		{
			k=(i<<4)+16;
		}
		else
		{
			k=i<<4;
		}
		
		if(mult_pack_f!=0)
		{
			AesDecryptData(report_t->pdu+4,k);	
		}
		else
		{
			AesDecryptData(report_t->pdu,k);	
		}
	}

	
	//InfoPrintf("ƽ̨Ӧ��(remote):");
	//for(i=0;i<Len;i++)InfoPrintf("%02X,",buf[i]);
	//InfoPrintf("\r\n");
	//InfoPrintf("ƽ̨�·�(remote)ID------------------------- :%04X\r\n",rec_msg_id);
	
   	//DebugPrintf(REMOTE_DBG,"rx remote:rx len:%u, s:%04X,id:%04X\r\n",rec_msg_len,rec_serial,rec_msg_id);
   	switch(rec_msg_id)
   	{
		case GENERAL_ACK_MSG_ID: //ͨ��Ӧ����ϢID
			memcpy(&ack_serial,report_t->pdu,2);                          
			memcpy(&ack_msg_id,report_t->pdu+2,2);
			ack_result = *(report_t->pdu+4);
			post_report_result(ack_serial,ack_msg_id,ack_result);
			break;
		
		case DN_ACK_RSA_KEY_ID:
			{
            //OS_ERR os_err; 
			uint16_t N_len_hex;
			unsigned char *N_modulus_hex;
			uint8_t  E_len_hex;
			unsigned char *E_exponent_hex;
			
			//InfoPrintf("����ԿӦ��(remote)\r\n");
			
			//BlockPrintf("RX RSA KEY:\r\n");
			//for(i=0;i<Len;i++)BlockPrintf("%02X,",buf[i]);
			//BlockPrintf("\r\n");

			//ACK serials num
			//memcpy(&ack_serial,report_t->pdu,2);  
			ack_serial=report_t->pdu[0]+report_t->pdu[1]*256;
			//BlockPrintf("Ӧ����ˮ��%04X\r\n",ack_serial);
			
			//ACK result
			ack_result = *(report_t->pdu+2);
			//BlockPrintf("Ӧ����%02X\r\n",ack_result);
			
			//N KEY len (2 bytes)
			memcpy(&N_len_hex,report_t->pdu+3,2);
			N_len_hex=report_t->pdu[3]+report_t->pdu[4]*256;
			//BlockPrintf("N ����%04X\r\n",N_len_hex);

			//N KEY
			N_modulus_hex=report_t->pdu+5;

			//E KEY len (1 byte)
			E_len_hex= *(report_t->pdu+5+N_len_hex);
			//BlockPrintf("E ����%04X\r\n",E_len_hex);

			//E KEY
			E_exponent_hex=report_t->pdu+5+N_len_hex+1;

			if(N_len_hex>128 || E_len_hex>64)
			{
				ack_result=0x55;
			}
			else
			{
                memset(RSA_N_modulus_str,0,256+64);
				
				hex_to_string(N_modulus_hex,N_len_hex,RSA_N_modulus_str,&RSA_N_len_str);
				hex_to_string(E_exponent_hex,E_len_hex,RSA_E_exponent_str,(uint16_t *)&RSA_E_len_str);
				
				/*BlockPrintf(	"Nstr(%d):%s\r\nEstr(%d):%s\r\n\r\n",
								RSA_N_len_str,
								RSA_N_modulus_str,
								RSA_E_len_str,
								RSA_E_exponent_str);*/
			}
			}
		
			post_report_result(ack_serial,UP_REQ_RSA_KEY_ID,ack_result);
			break;
			
		case DN_ACK_AES_KEY_ID:
			//InfoPrintf("AES��Կ�ϱ�Ӧ��(remote):\r\n");
			//ACK serials num
			//memcpy(&ack_serial,report_t->pdu,2);  
			ack_serial=report_t->pdu[0]+report_t->pdu[1]*256;
			//InfoPrintf("Ӧ����ˮ��%04X\r\n",ack_serial);
			
			//ACK result
			ack_result = *(report_t->pdu+2);
			//InfoPrintf("Ӧ����%02X\r\n",ack_result);
			post_report_result(ack_serial,UP_REP_AES_KEY_ID,ack_result);
			break;
			
		case DN_LOGIN_ACK_MSG_ID: //ƽ̨Ӧ���ն˵�½
			//������Ϣ
			InfoPrintf("��ȨӦ��(remote)...\r\n");
			memcpy(&ack_serial,report_t->pdu,2);                          
			ack_result = *(report_t->pdu+2);
			//��ȡ������ʱ��
			protocol_format_time = (sgwm_protocol_time_format_t *)(report_t->pdu+3);
			if(gps_check_lacation_state()==0 || (get_rtc_init_flag()==0) )
			{
				BEIJING_TIME_T local_current_time;
				sgmw_pt_time_transform_beijing_time(&server_time,protocol_format_time);
				get_current_time(&local_current_time);
				
				if(  (local_current_time.year	!= server_time.year)    
				   ||(local_current_time.month	!= server_time.month)
				   ||(local_current_time.day	!= server_time.day)      
				   ||(local_current_time.hour	!= server_time.hour)
				   ||(local_current_time.minute	!= server_time.minute)
				   ||(local_current_time.second	!= server_time.second)
				  )
				{
					InfoPrintf("��½Уʱ,ƽ̨ʱ��:");
					InfoPrintf("%u-%02d-%02d %02d:%02d:%02d\r\n",server_time.year,server_time.month,
					server_time.day,server_time.hour,server_time.minute,server_time.second);
					InfoPrintf("����RTCʱ��:");
					printf_time_now();
					write_rtc(server_time);//����RTCʱ��
					InfoPrintf("Уʱ�����RTC:");
					printf_time_now();
				}
			}
			post_report_result(ack_serial,UP_LOGIN_MSG_ID,ack_result);
			break;
		
	  	case DN_DOWN_DBC_FILE_MSG_ID:// ƽ̨�·�DBC�ļ�sgmw_rec_dbc_file
			//����DBC�ļ�
			ack_result = sgmw_rec_dbc_tbc_file(mult_pack_f,report_t->pdu,rec_msg_len);
			//������Ϣ
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_SET,rec_serial,rec_msg_id,ack_result);
			break;

	  	case DN_GET_DEFAULT_DBC_MSG_ID: //�鿴Ĭ��DBC�ļ�
            //������Ϣ
            post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_QRY,rec_serial,rec_msg_id,0);
			break;
							 
	  	case DN_SET_DEFAULT_DBC_MSG_ID: //ƽ̨�޸�Ĭ�ϵ�DBC�ļ�
            /*memcpy(file_md5,report_t->pdu,16);//��ȡDBC�ļ�MD5[16]  
			//����Ĭ��TBC�ļ�
			if(!set_default_tbc_file(file_md5))
			{
				ack_result = 0;
				post_update_param_msg(	&tcb_can_manage_task,
										REMOTE_RECV_TASK_ID,
										FATHER_MSG_T_REMOTE_UPDATE_PARAM,
										RMT_UPD_TBC_DFT
									 );
			}
			else
			{
				InfoPrintf("set default md5 failed\r\n");
				ack_result = 1;
			}
			                           
            //������Ϣ
            post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_SET,rec_serial,rec_msg_id,ack_result);*/
			break;		
			
      	case DN_GET_DBC_LIST_MSG_ID: //�鿴DBC�ļ��б�
			//������Ϣ
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_QRY,rec_serial,rec_msg_id,0);
			break;	
		
	  	case DN_DEL_DBC_FILE_MSG_ID: //ɾ��DBC�ļ� 
			/*if(!read_default_tbc_file_md5(file_md5))//��ȡĬ�ϵ�MD5ֵ
			{
				if(memcmp((const void *)file_md5,(const void *)report_t->pdu,16)==0)
				{
					//ɾ��9����,�󹤷����TBC����
					feed_wdt();
					__disable_irq();
					Chip_IAP_PreSectorForReadWrite(9,9);
					Chip_IAP_EraseSector(9,9);
					__enable_irq();
					feed_wdt();
					InfoPrintf("delete default TBCfile OK\r\n");
				}
			}
			
	        memcpy(file_md5,report_t->pdu,16);//��ȡDBC�ļ�MD5[16]  
			if(!delete_tbc_file(file_md5))
	        {
				ack_result = 0;									
			}
	        else
	        {
	            InfoPrintf("TBC file not exist\r\n");
				ack_result = 1;
	        }
	        post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_SET,rec_serial,rec_msg_id,ack_result);*/
       		break;
		
	  	case DN_GET_VER_MSG_ID: //�鿴��ϵͳ�汾��
            post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_QRY,rec_serial,rec_msg_id,0);
     		break;

	  	case DN_GET_REPORT_PERIOD_MSG_ID: //�鿴�ش�Ƶ��
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_QRY,rec_serial,rec_msg_id,0);
			break;
		
	  	case DN_SET_REPORT_PERIOD_MSG_ID: //�޸Ļش�Ƶ��
            //memcpy(&postback_f,report_t->pdu,4);
            //post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_SET,rec_serial,rec_msg_id,set_report_period(postback_f));
  			break;
		
	  	case DN_GET_HEARTBEAT_MSG_ID: //�鿴����Ƶ��
            post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_QRY,rec_serial,rec_msg_id,0);
        	break;

	  	case DN_SET_HEARTBEAT_MSG_ID: //�޸�����Ƶ��
			//memcpy(&heartbeat_f,report_t->pdu,4);
			//post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_SET,rec_serial,rec_msg_id,set_heartbeat_period(heartbeat_f));
			break;
		
	  	case DN_GET_USE_FILE_TYPE_MSG_ID: //�鿴�ش���ʽ
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_QRY,rec_serial,rec_msg_id,0);
			break;
		
	  	case DN_SET_USE_FILE_TYPE_MSG_ID: //�޸Ļش���ʽ
			report_TBC_type = *report_t->pdu;
			set_rep_tbc_type(report_TBC_type);
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_SET,rec_serial,rec_msg_id,0);
			break;
		
	  	case DN_GET_SAVE_DATA_EN_MSG_ID: //�鿴�Ƿ񱣴�SD��
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_QRY,rec_serial,rec_msg_id,0);
			break;					
				
      	case DN_SET_SAVE_DATA_EN_MSG_ID: //�޸��Ƿ񱣴�SD��
			SD_save_status = 1;
			set_sd_save_status(SD_save_status);
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_SET,rec_serial,rec_msg_id,0);
			break;
		
	 	case DN_GET_SAVE_DATA_TIME_MSG_ID: //�鿴SD������ʱ��
            post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_QRY,rec_serial,rec_msg_id,0);
 			break;
		
	 	case DN_SET_SAVE_DATA_TIME_MSG_ID: //�޸�D������ʱ��
			SD_save_time = 0;
			set_sd_save_time(SD_save_time );
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_SET,rec_serial,rec_msg_id,0);
			break;
		
	 	case DN_GET_SD_STATE_MSG_ID: //�鿴D��״̬
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_QRY,rec_serial,rec_msg_id,0);
			break;
	 	case DN_GET_SD_VOL_MSG_ID: //�鿴SD���ܴ�С
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_QRY,rec_serial,rec_msg_id,0);
			break;
	 	case DN_GET_SD_USED_MSG_ID: //�鿴D�����ô�С
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_QRY,rec_serial,rec_msg_id,0);
			break;
	  	case DN_GET_REMOTE_NET_PARAM_MSG_ID: //�鿴��ǰIP�˿ں�
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_QRY,rec_serial,rec_msg_id,0);
			break;
	 	case DN_SET_REMOTE_NET_PARAM_MSG_ID: //�޸ĵ�ǰIP/�����˿ں�
            /*memcpy(&port,report_t->pdu,2);
            set_remote_port(port);	       
            set_remote_dn(report_t->pdu+2,rec_msg_len-2);
			post_update_param_msg(	&tcb_modem_manage_task,
									REMOTE_RECV_TASK_ID,
									FATHER_MSG_T_REMOTE_UPDATE_PARAM,
									RMT_UPD_IP);
			post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_SET,rec_serial,rec_msg_id,0);*/
            break;	
      	case NEW_UPDATE_PACKEGE_ID: //�������ָ��
            /*ack_result =parse_upgrade_param(report_t->pdu);                      
            //������Ϣ
            post_ack_to_server_q(FATHER_MSG_T_ACK_TO_SERVER_SET,rec_serial,rec_msg_id,ack_result);*/                          
            break;
      	case REQUEST_UPDATE_PACKEGE_ACK_ID://����������Ӧ��
	        //upgrade_data_handle(rec_msg_len,report_t->pdu);                          
	        break;
			
	  	default:break; 
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
static int sgmw_extract_one_rx_prtocol_frame(uint8_t *app_buf,
  											 int buf_valid_len,
  											 int *extracted_frame_len)
{
	int i;

	//-------------------------------
	for(i=0;i<buf_valid_len;i++)
	{
		if(SGMW_PROTOCOL_FIX==app_buf[i])
		{
			break;
		}
	}

	if(0 != i)
	{
		//�ҵ�HEAD,����HEAD֮ǰ����������,�����Ƶ�
		*extracted_frame_len=0;
		return i;
	}

	//------------------------------
	for(i=1;i<buf_valid_len;i++)
	{
		if(SGMW_PROTOCOL_FIX==app_buf[i])
		{
			break;
		}
	}

	if(buf_valid_len==i)
	{
		*extracted_frame_len=0;
		return 0;
	}

	if(i >= SGMW_PT_FRAME_MININUM_LEN)
	{//ok
		*extracted_frame_len=(i+1);
		return (i+1);
	}
	else
	{//mininum len error
		*extracted_frame_len=0;
		return (i+1)-1;//retain the second SGMW_PROTOCOL_FIX,it may be the HEAD_BYTE of next frame
	}
}

uint8_t sgmw_analysis_modem_gprs_q(uint8_t * ptr,uint16_t len)
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
		max_allow_copy_len= SGMW_PT_APP_LAYER_BUF_MAX-remain_len;
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

		memcpy(sgmw_pt_app_layer_buf+remain_len,ptr,one_time_copy_len);

		remain_len += one_time_copy_len;
		
		while(1)	
		{
			if(remain_len>=9)//9:mininum of one sgmw frame 
			{
				shift_len=sgmw_extract_one_rx_prtocol_frame(sgmw_pt_app_layer_buf,remain_len,&extractOut_buf_len);
				
				if(shift_len>0)
				{
					if(extractOut_buf_len>0)
					{	
						DeEscaped_len = sgmw_DeEscape_package(sgmw_DeEscaped_frame_buf,
															  sgmw_pt_app_layer_buf,
															  extractOut_buf_len);//��ת��
						
						ret = sgmw_xor_checkout_func(sgmw_DeEscaped_frame_buf,DeEscaped_len);
						if(0==ret)
						{
							sgmw_parse_one_rx_protocol_frame(sgmw_DeEscaped_frame_buf,DeEscaped_len);
						}
						else
						{
							//У�����
							InfoPrintf("GPRS֡У�����\r\n");
							shift_len -= 1;//retain the last HEAD_BYTE, it may be the head of next frame.
						}
					}
					else
					{

					}
					
					remain_len-=shift_len;
					
					memmove(sgmw_pt_app_layer_buf,sgmw_pt_app_layer_buf+shift_len,remain_len);
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
			sgmw_analysis_modem_gprs_q(p_buf,msg_size);
			user_free(p_buf,__FUNCTION__);
			OSSchedRoundRobinYield(&os_err);
		}
		else if(os_err != OS_ERR_TIMEOUT)
		{
			OSTimeDly(100,OS_OPT_TIME_DLY,&os_err);
		}
	}
}

#endif

