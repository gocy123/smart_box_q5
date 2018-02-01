#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ISO15765.h"
#include "can0_network_layer.h"
#include "user_debug_app.h"

#include "os.h"
#include "os_cfg_app.h"


 



#define ______port_area____________________________________________________________________________
//----修改这个函数，可提高传输速度--------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static int ISO15765_ck_tx_data_time_interval(uint32_t last_time, uint32_t current_time,uint32_t STmin);

N_USData_confirm_t  ISO15765_confirm=confirm_ok;   //用于APP和网络层间的状态交互 (未实现)
N_USData_indicate_t ISO15765_indicate=indicate_ok; //用于APP和网络层间的状态交互 (未实现)

#define ISO15765_TX_TIMEOUT (TIME_100ms*3)
#define ISO15765_RX_TIMEOUT (TIME_100ms*3)//(TIME_500ms)

#define LOC_RX_BUF_MAX_len 		(4095)
#define LOC_RSP_STmin			(1000)//unit: us
#define LOC_FC_BS  				(128)
#define ______port_end____________________________________________________________________________



static N_PDU_RX_USER_DEF_T pdu_Rx_ctl={0};
static N_PDU_TX_USER_DEF_T pdu_Tx_ctl={0};

//static uint8_t  loc_pdu_rx_buf[LOC_RX_BUF_MAX_len]={0};
static uint8_t  *loc_pdu_rx_buf=NULL;

static void deinit_pdu_Rx_ctl(void)
{
	memset((uint8_t *)(&pdu_Rx_ctl),0,sizeof(N_PDU_RX_USER_DEF_T));
}

static void init_pdu_Rx_ctl(void)
{
	deinit_pdu_Rx_ctl();
	pdu_Rx_ctl.loc_st_buf_max_len=LOC_RX_BUF_MAX_len;
	pdu_Rx_ctl.rx_responser_FC_STmin=127*1000;//default:127ms
}

static void deinit_pdu_Tx_ctl(void)
{
	memset((uint8_t *)(&pdu_Tx_ctl),0,sizeof(N_PDU_TX_USER_DEF_T));
}


static void init_pdu_Tx_ctl(void)
{
	deinit_pdu_Tx_ctl();
	pdu_Tx_ctl.loc_rsp_FC_STmin=LOC_RSP_STmin;
	pdu_Tx_ctl.loc_FC_BS_value=LOC_FC_BS;
	pdu_Tx_ctl.tx_time_tree=0;
}

static void ISO15765_init_NWL_timer(void)
{

}

static void ISO15765_update_rx_time_tree(void)
{
    OS_ERR os_err;
	pdu_Rx_ctl.rx_time_tree=OSTimeGet(&os_err);
}

static uint32_t ISO15765_get_rx_time_tree(void)
{
	return pdu_Rx_ctl.rx_time_tree;
}

static void ISO15765_update_tx_time_tree(void)
{
    OS_ERR os_err;
	pdu_Tx_ctl.tx_time_tree=OSTimeGet(&os_err);
}

static uint32_t ISO15765_get_tx_time_tree(void)
{
	return pdu_Tx_ctl.tx_time_tree;
}

static uint32_t ISO15765_get_curr_time_stamp(void)
{
	OS_ERR os_err;
	return OSTimeGet(&os_err);
}






/*
	input:
			can_id	:
			FS_para	:	(0~~2)
			STmin	:	 unit: us
						range:
						(0) 
						(100~~900)		----100  one step----
						(1000~~127000)  ----1000 one step----
	return:

			< 0 : input para error
			= 0 : can send fail
			= 1 : ok
*/
static int ISO15765_send_FC(uint32_t can_id,uint8_t FS_para,uint8_t BS_para,uint32_t STmin_para)
{
	can0_data_package_t can_tx_package={0};	
	N_PDU_FC_T fc_data={0};
	uint8_t msg_STmin_para=0;
	int ret;

	//InfoPrintf("ISO15765_send_FC(FS=%d,BS=%d,STmin=%d)\r\n",FS_para,BS_para,STmin_para);
	
	if(FS_para>FS_OVFLW)
	{
		return -1;
	}

	if(BS_para)
	{
		//    0: 表示告知发送放,一次全部发完,不用再等接收方的流控帧
		//1~255: 表示告知发送方,在连续发送了BS个帧后,需等待接收方的流控帧
		//       才能继续发送
	}

	if(STmin_para==0)
	{
		msg_STmin_para=0;
	}
	else if(STmin_para>0 && STmin_para<1000)
	{
		if(STmin_para%100 !=0 )
		{
			return -2;
		}
		
		msg_STmin_para = STmin_para/100 + 0xF0;
	}
	else if(STmin_para>=1000 && STmin_para<=127000)
	{
		if(STmin_para%1000 !=0 )
		{
			return -3;
		}
		
		msg_STmin_para = STmin_para/1000;
	}

	//Write 8 byte CAN MSG DATA cache buf 
	fc_data.PCI_Type=FC_PCI;
	fc_data.FS=FS_para;
	fc_data.BS=BS_para;
	fc_data.STmin=msg_STmin_para;  
	memset((uint8_t *)(&fc_data.reserve1),0x55,5);

	//config CAN ID,FMT,DL,and then copy 8 bytes from cache buf to tx_can_msg
	can_tx_package.id=can_id;
	can_tx_package.type=DATA_FRAME;
	can_tx_package.format=STD_ID_FORMAT;
	can_tx_package.len=8;
	memcpy(&can_tx_package.dataA[0],&fc_data,8);	

	//tx can msg
	ret=can0_send_data((uint8_t *)&can_tx_package, sizeof(can0_data_package_t));
	if(ret==sizeof(can0_data_package_t))
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

/*
	return:
			1: can send can frame now 
			0: can not send now
使用UCOS时钟节拍，导致最短时间为节拍时间,1ms,不能实现100~~900us的STmin，需改进
*/
static int ISO15765_ck_tx_data_time_interval(uint32_t last_time, uint32_t current_time,uint32_t STmin)
{	
	return 1;//直接返回!!!!!!--需设计一个定时器来计时，不能使用SYSTick--!!!!太慢!!!!!!!!!!!!!!!!!!!!!
	
	/*if((current_time-last_time)*1000 >= STmin)
	{
		return 1;
	}
	else
	{
		return 0;
	}*/
}

/*
	return:
			0:error sn
			1:right sn
*/
static int ISO15765_ck_rx_sn_continuous(uint8_t current, uint8_t last)
{
	if(current>=16 || last>=16)
	{
		return 0;
	}

	if(last==15&&current==0)
	{
		return 1;
	}
	else 
	{
		if(current<last)
		{
			return 0;
		}
		else if(current-1==last)
		{
			return 1;
		}
		else 
		{
			return 0;
		}
	}
}

/*
	return:
			1: FC para ok
			0: FC para error
*/
static int ISO15765_analysis_rx_FC_data(uint8_t can_msg_FC_FS,uint8_t can_msg_FC_BS,uint8_t can_msg_FC_STmin)
{
	if(can_msg_FC_FS>2)
	{
		return 0;
	}
	
	pdu_Rx_ctl.rx_responser_FC_FS=can_msg_FC_FS;
	pdu_Rx_ctl.rx_responser_FC_BS=can_msg_FC_BS;

	if(can_msg_FC_STmin<=0x7F)
	{
		//帧时间间隔为 0ms 至 127ms ,  STmin ms
		pdu_Rx_ctl.rx_responser_FC_STmin=can_msg_FC_STmin*1000;//unit: us
	}
	else if(can_msg_FC_STmin>=0x80 && can_msg_FC_STmin<=0xF0)
	{
		//保留
		pdu_Rx_ctl.rx_responser_FC_STmin=127*1000;
		return 1;
	}
	else if(can_msg_FC_STmin>=0xF1 && can_msg_FC_STmin<=0xF9)
	{
		//帧时间间隔为 100us 至 900us (F1--F9)
		pdu_Rx_ctl.rx_responser_FC_STmin=(can_msg_FC_STmin-0xF0)*100;//unit: us
	}
	else if(can_msg_FC_STmin>=0xFA)
	{
		//保留
		pdu_Rx_ctl.rx_responser_FC_STmin=127*1000;
		return 1;
	}	

	return 1;	
}

/*
	NWL receive  one link layer can msg
	
	return 
			1: receive one can msg
			0: not reveive msg
	note:
			support STD ID and EXTEND ID
*/
static int ISO15765_read_LKLY_data(uint32_t *id,uint8_t *data,uint8_t *data_len)
{
	OS_ERR os_err;
	OS_MSG_SIZE msg_size;
	can0_data_package_t *can_msg=NULL;

	#if 0
	can0_data_package_t test_can_msg={0};
	if(can0_receive_data((uint8_t *)&test_can_msg,sizeof(can0_data_package_t))==sizeof(can0_data_package_t))
	{
		if(((test_can_msg.id>>29) == 0)&&(test_can_msg.len<= 8))
		{
			*id=test_can_msg.id;
			memcpy(data,&(test_can_msg.dataA[0]),test_can_msg.len);
			*data_len=test_can_msg.len;
			return 1;
		}
	}
	return 0;
	#endif
		
	can_msg=(can0_data_package_t *)OSQPend(	&ISO15765_RX_CAN_MSG_Q,
											TIME_1ms,
											OS_OPT_PEND_NON_BLOCKING,//OS_OPT_PEND_BLOCKING,//
											&msg_size,
											NULL,
											&os_err
											);
	if(os_err!=OS_ERR_NONE)
	{
		//InfoPrintf("pend rx_can_msg_q err=%d\r\n",os_err);
	}
	
	if(can_msg !=NULL)
	{
		if(((can_msg->id>>29) == 0)&&(can_msg->len <= 8))
		{
			*id=can_msg->id;
			memcpy(data,can_msg->dataA,can_msg->len);
			*data_len=can_msg->len;
			user_free(can_msg,__FUNCTION__);
			return 1;
		}

		user_free(can_msg,__FUNCTION__);
	}

	return 0;
}


/*
	extract the complete app data package from separate ISO15765 can messages

	input:
			*can_msg		: received can msg`data (8 bytes)
			*len			: extracted data len
*/
static enum pdu_rx_sta ISO15765_pro_LKLY_rx_data(uint8_t *can_msg,uint16_t *received_len)//,uint8_t *malloc_ptr)
{
	//int i;
	//uint8_t *ptr;
	//uint8_t testbuf[8]={0};

	int ret;
	N_PDU_T *pdu=NULL;
	uint8_t temp_sn;
	//enum pdu_rx_sta ret_sta;
	//uint8_t address_type;
	uint16_t temp_len;
	uint8_t *temp_ptr=NULL;

	pdu=(N_PDU_T *)can_msg;
	
	#if 0
	InfoPrintf("解析15765报文:");
	ptr=(uint8_t *)pdu;
	for(i=0;i<8;i++)
	{
		InfoPrintf("%02X,",ptr[i]);
	}
	InfoPrintf("\r\n");
	#endif

	switch (pdu->n_pdu_fmt.N_PCI_Type)
	{
		case SF_PCI:
			//InfoPrintf("RX SF\r\n");
			temp_len=pdu->n_pdu_fmt.SF_dl_all_FF_dl_low_CF_sn_FC_fs;
			
			if(  (temp_len>=1&&temp_len<=7)
			   &&(pdu_Rx_ctl.current_rx_status == PDU_RX_STEP_END)	
			)
			{
				pdu_Rx_ctl.need_to_rx_info_len=temp_len;

				//malloc local buffer
				#if 1
				temp_ptr=(uint8_t *)user_malloc(pdu_Rx_ctl.need_to_rx_info_len,__FUNCTION__);
				if(temp_ptr==NULL)
				{
					pdu_Rx_ctl.need_to_rx_info_len=0;
					pdu_Rx_ctl.current_rx_status = PDU_RX_STEP_END;
					return PDU_RX_STEP_ERR_MALLOC;
				}
				loc_pdu_rx_buf=temp_ptr;
				#endif
				
				//copy data
				memcpy(&loc_pdu_rx_buf[0],&(pdu->n_pdu_fmt.SF_data1),pdu_Rx_ctl.need_to_rx_info_len);
				*received_len=pdu_Rx_ctl.need_to_rx_info_len;
				pdu_Rx_ctl.rx_result=N_OK;
				pdu_Rx_ctl.current_rx_status=PDU_RX_STEP_END;
				return PDU_RX_STEP_END;
			}
			else
			{
				pdu_Rx_ctl.rx_result=N_UNEXP_PDU;
				
				if((pdu_Rx_ctl.current_rx_status != PDU_RX_STEP_END))
				{
					pdu_Rx_ctl.error_flag = N_PDU_ERR_TYPE_CF_BREAK_CF;
				}
				else
				{
					pdu_Rx_ctl.error_flag = N_PDU_ERR_TYPE_SF_DATA_LEN;
				}

				return PDU_RX_STEP_UNVALID_DATA;
			}
			
		break;

		case FF_PCI:
			//InfoPrintf("RX FF\r\n");	
			temp_len=pdu->n_pdu_fmt.FF_MultiFrame_data_len_high;
			temp_len<<=4;
			temp_len+=pdu->n_pdu_fmt.SF_dl_all_FF_dl_low_CF_sn_FC_fs;
			//InfoPrintf("need_to_rx_info_len=%04X\r\n",pdu_Rx_ctl.need_to_rx_info_len);
			if(temp_len <= pdu_Rx_ctl.loc_st_buf_max_len)
			{
				if(temp_len>=8)
				{
					//no matter what , initialize a new Multi-frame receiving
					pdu_Rx_ctl.rx_multi_frm_CF_SN=0;//preset first Serial Number
					pdu_Rx_ctl.need_to_rx_info_len=temp_len;

					//malloc local buffer
					#if 1
					temp_ptr=(uint8_t *)user_malloc(pdu_Rx_ctl.need_to_rx_info_len,__FUNCTION__);
					if(temp_ptr==NULL)
					{
						pdu_Rx_ctl.need_to_rx_info_len=0;
						pdu_Rx_ctl.current_rx_status = PDU_RX_STEP_END;
						return PDU_RX_STEP_ERR_MALLOC;
					}
					loc_pdu_rx_buf=temp_ptr;
					#endif
				
					//copy data
					memcpy(&loc_pdu_rx_buf[0],&(pdu->n_pdu_fmt.FF_MultiFrame_data1),6);
					*received_len=6;
					if(pdu_Rx_ctl.current_rx_status==PDU_RX_STEP_END)
					{
						pdu_Rx_ctl.rx_result=N_OK;
					}
					else
					{
						pdu_Rx_ctl.rx_result=N_UNEXP_PDU;
						pdu_Rx_ctl.error_flag=N_PDU_ERR_TYPE_CF_BREAK_CF;						
					}
					
					//req CF frame
					ret=ISO15765_send_FC(ISO15765_TX_ID,FS_CTS,pdu_Tx_ctl.loc_FC_BS_value,pdu_Tx_ctl.loc_rsp_FC_STmin);
					if(ret<0)
					{
						InfoPrintf("ISO15765_send_FC() para error 1\r\n");
					}
					pdu_Rx_ctl.rx_FC_BS_counter=0;
					pdu_Rx_ctl.current_rx_status=PDU_RX_STEP_CONTINUE;
				}
				else
				{
					pdu_Rx_ctl.rx_result=N_UNEXP_PDU;
					pdu_Rx_ctl.error_flag=N_PDU_ERR_TYPE_CF_BREAK_CF;
				}
				
				return PDU_RX_STEP_CONTINUE;
			}
			else
			{
				pdu_Rx_ctl.rx_result=N_BUFFER_OVFLW;
				pdu_Rx_ctl.error_flag=N_PDU_ERR_TYPE_CF_DATA_MAX_LEN;
				ret=ISO15765_send_FC(ISO15765_TX_ID,FS_OVFLW,pdu_Tx_ctl.loc_FC_BS_value,pdu_Tx_ctl.loc_rsp_FC_STmin);
				if(ret<0)
				{
					InfoPrintf("ISO15765_send_FC() para error 2\r\n");
				}
				pdu_Rx_ctl.current_rx_status=PDU_RX_STEP_END;
				return PDU_RX_STEP_END;
			}
		break;

		case CF_PCI:
			//InfoPrintf("RX CF\r\n");			
			if(pdu_Rx_ctl.current_rx_status==PDU_RX_STEP_CONTINUE)
			{
				temp_sn=pdu->n_pdu_fmt.SF_dl_all_FF_dl_low_CF_sn_FC_fs;
				
				if(( ISO15765_ck_rx_sn_continuous(temp_sn, pdu_Rx_ctl.rx_multi_frm_CF_SN)==1)
				   //||(pdu_Rx_ctl.rx_multi_frm_CF_SN==0xFF)	
				)
				{
					pdu_Rx_ctl.rx_FC_BS_counter++;
					pdu_Rx_ctl.rx_multi_frm_CF_SN=temp_sn;

					if((*received_len)+7<=pdu_Rx_ctl.need_to_rx_info_len)
					{
						pdu_Rx_ctl.current_rx_status=PDU_RX_STEP_CONTINUE;
						temp_len=7;
						if(pdu_Rx_ctl.rx_FC_BS_counter >= pdu_Tx_ctl.loc_FC_BS_value)
						{
							pdu_Rx_ctl.rx_FC_BS_counter=0;
							//req CF frame  --------------是否要考虑APP层控制CTS 的发送 ??????????暂不考虑---------
							ISO15765_send_FC(ISO15765_TX_ID,FS_CTS,pdu_Tx_ctl.loc_FC_BS_value,pdu_Tx_ctl.loc_rsp_FC_STmin);
							if(ret<0)
							{
								InfoPrintf("ISO15765_send_FC() para error 3\r\n");
							}
						}
					}
					else
					{//end
						temp_len=(pdu_Rx_ctl.need_to_rx_info_len-6)%7;// 
					}

					//copy data
					memcpy(&loc_pdu_rx_buf[*received_len],&(pdu->n_pdu_fmt.CF_data1),temp_len);

					(*received_len)+=temp_len;
					if(*received_len==pdu_Rx_ctl.need_to_rx_info_len)
					{
						//InfoPrintf("RX CF(last)!!!!!!!\r\n");	
						pdu_Rx_ctl.rx_result=N_OK;
						pdu_Rx_ctl.current_rx_status=PDU_RX_STEP_END;
						return PDU_RX_STEP_END;
					}
					else
					{
						return PDU_RX_STEP_CONTINUE;
					}
				}
				else
				{
					pdu_Rx_ctl.rx_result=N_WRONG_SN;
					pdu_Rx_ctl.error_flag=N_PDU_ERR_TYPE_CF_SN;
					return PDU_RX_STEP_END;
				}
			}
			else
			{
				pdu_Rx_ctl.rx_result=N_UNEXP_PDU;
				pdu_Rx_ctl.error_flag = N_PDU_ERR_TYPE_CF_UNEXP_CF;
				return PDU_RX_STEP_UNVALID_DATA;
			}
		break;

		case FC_PCI:
			
			//InfoPrintf("RX FC-->");		
			if(pdu_Tx_ctl.NWL_now_sending_flag==1)
			{
				ret=ISO15765_analysis_rx_FC_data(pdu->n_pdu_fmt.SF_dl_all_FF_dl_low_CF_sn_FC_fs,
												 pdu->n_pdu_fmt.FC_BS,
												 pdu->n_pdu_fmt.FC_STmin
								   	   			);
				if(ret==1)
				{
					/*InfoPrintf("FS=%d,BS=%d,STmin=%d us \r\n",	pdu_Rx_ctl.rx_responser_FC_FS,
															pdu_Rx_ctl.rx_responser_FC_BS,
															pdu_Rx_ctl.rx_responser_FC_STmin
															);*/	
					if(pdu_Rx_ctl.rx_responser_FC_FS==0)
					{
						pdu_Rx_ctl.rx_FC_CTS_flag=1;
					}
					else if(pdu_Rx_ctl.rx_responser_FC_FS==1)
					{
						pdu_Rx_ctl.rx_FC_WAIT_flag=1;
					}
					else if(pdu_Rx_ctl.rx_responser_FC_FS==2)
					{
						pdu_Rx_ctl.rx_FC_OVFLW_flag=1;
					}
					
					pdu_Rx_ctl.current_rx_status=PDU_RX_STEP_END;
				}
				else
				{
					InfoPrintf("RX ERROR PARA FC,stop sending...confirm to app...\r\n");		
					pdu_Rx_ctl.current_rx_status=PDU_RX_STEP_ERR_FC;
					pdu_Rx_ctl.rx_result=N_INVALID_FS;
					return PDU_RX_STEP_END;
				}
			}
			else
			{
				//when no data is sending, the rx RC is abandoned!!!
				return PDU_RX_STEP_UNVALID_DATA;
			}
			
		break;

		default:
			InfoPrintf("RX NOT A N_PDU\r\n");
			
		break;	
	}
	
	return PDU_RX_STEP_END;

}

/*'
	return 
		   <0: err
			0: tx end
			1: tx FF
			2: tx CF

	note: single frame data len max     must be 7
	      multi  frame data len minmum  must be 8 
*/
static int ISO15765_pro_LKLY_tx_data(uint8_t *step,uint8_t *data,uint16_t data_len,uint16_t *had_tx_len,uint8_t *tx_SN)
{
	uint8_t  address_type;
	//uint16_t i;
	int ret;

	uint32_t can_id=ISO15765_TX_ID;
	static uint8_t remain_bytes=0;
	//static uint8_t  additional_frame_flag=0;
	
	N_PDU_T temp_tx_pdu={0xFF,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};//9 个数 赋值给 size=8的结构体 !!!
	
	can0_data_package_t can_tx_package={0};

	//InfoPrintf("pdu_Rx_ctl.rx_responser_FC_STmin=%d\r\n",pdu_Rx_ctl.rx_responser_FC_STmin);

	if(data_len>4095)
	{
		(*step)=0;
		return -1;
	}
	
	//#################################--Single Frame--########################################## 
	address_type=0;//0:standard address
	if((data_len<=6) ||((data_len==7)&&(address_type==0)))
	{
		if((*step)!=0)
		{
			(*step)=0;
			return -2;
		}
		can_tx_package.id=can_id;
		can_tx_package.type=DATA_FRAME;
		can_tx_package.format=STD_ID_FORMAT;
		can_tx_package.len=8;//data_len; zgc ?????????
		//write can data SF
		memset(&temp_tx_pdu.DATA[0],0x55,8);
		temp_tx_pdu.n_pdu_fmt.N_PCI_Type=SF_PCI;
		temp_tx_pdu.n_pdu_fmt.SF_dl_all_FF_dl_low_CF_sn_FC_fs=data_len;
		memcpy(&(temp_tx_pdu.n_pdu_fmt.SF_data1),data,data_len);	
		
		memcpy(&can_tx_package.dataA[0],&temp_tx_pdu.DATA[0],8);	
		ret=can0_send_data((uint8_t *)&can_tx_package, sizeof(can0_data_package_t));
		if(ret!=sizeof(can0_data_package_t)) 
		{
			(*step)=0;
			return -3;
		}
		*had_tx_len=data_len;
		(*step)=0;
		return 0;
	}
	else 
	//#################################--Multi Frame--########################################## 	
	{	
		//(1)Fisrt Frame ------------------------------------------------------------------------------
		if(*had_tx_len==0)
		{
			if((*step)!=0)
			{
				(*step)=0;
				return -4;
			}
			(*step)=1;
			//InfoPrintf("发送首帧\r\n");
			can_tx_package.id=can_id;
			can_tx_package.type=DATA_FRAME;
			can_tx_package.format=STD_ID_FORMAT;
			can_tx_package.len=8;//data_len; zgc ?????????
			//write can data FF
			//memset(&temp_tx_pdu.DATA[0],0x55,8);
			temp_tx_pdu.n_pdu_fmt.N_PCI_Type=FF_PCI;
			temp_tx_pdu.n_pdu_fmt.SF_dl_all_FF_dl_low_CF_sn_FC_fs=(uint8_t)(data_len&0x0F);
			temp_tx_pdu.n_pdu_fmt.FF_MultiFrame_data_len_high=(uint8_t)(data_len>>4);
			//不是标准地址的时候,未考虑!!!!!
			memcpy(&(temp_tx_pdu.n_pdu_fmt.FF_MultiFrame_data1),data,6);	
			
			memcpy(&can_tx_package.dataA[0],&temp_tx_pdu.DATA[0],8);	
			ret=can0_send_data((uint8_t *)&can_tx_package, sizeof(can0_data_package_t));
			if(ret!=sizeof(can0_data_package_t)) 
			{
				(*step)=0;
				return -5;
			}
			*tx_SN=1;//wikipedia consider this shuld be 1 here!!! yes
			*had_tx_len=6;
			return 1;
		}
	

		if(*had_tx_len==6)
		{
			remain_bytes=(data_len-(*had_tx_len))%7;
			if(remain_bytes!= 0)
			{
				//additional_frame_flag=1;
			}
			else
			{
				//additional_frame_flag=0;
			}
		}


		if((*step)!=1)
		{
			(*step)=0;
			return -6;
		}

		//(2)Second to (frame_amount-1) frame ------------------------------------------------------
		if((*had_tx_len)+7 <= data_len)
		{
			//InfoPrintf("发送连续帧--SN=%d\r\n",(*tx_SN));
			//copy can msg data
			can_tx_package.id=can_id;
			can_tx_package.type=DATA_FRAME;
			can_tx_package.format=STD_ID_FORMAT;
			can_tx_package.len=8;

			//write can data CF
			//memset(&temp_tx_pdu.DATA[0],0x55,8);
			temp_tx_pdu.n_pdu_fmt.N_PCI_Type=CF_PCI;
			temp_tx_pdu.n_pdu_fmt.SF_dl_all_FF_dl_low_CF_sn_FC_fs=(uint8_t)((*tx_SN)&0x0F);
			(*tx_SN)++;
			if(*tx_SN>=16)*tx_SN=0;
			memcpy(&(temp_tx_pdu.n_pdu_fmt.CF_data1),data+(*had_tx_len),7);

			memcpy(&can_tx_package.dataA[0],&temp_tx_pdu.DATA[0],8);	
			ret=can0_send_data((uint8_t *)&can_tx_package, sizeof(can0_data_package_t));
			if(ret!=sizeof(can0_data_package_t)) 
			{
				(*step)=0;
				return -7;
			}
						
			(*had_tx_len)+=7;

			if((*had_tx_len) >= data_len)
			{
				(*step)=0;
				return 0;
			}
			else
			{
				return 2;
			}
		}

		// send remain bytes
		//InfoPrintf("发送余数字节帧\r\n");
		can_tx_package.id=can_id;
		can_tx_package.type=DATA_FRAME;
		can_tx_package.format=STD_ID_FORMAT;
		can_tx_package.len=8;//(remain_bytes+1); zgc ?????????
		
		//memset(&temp_tx_pdu.DATA[0],0x55,8);
		temp_tx_pdu.n_pdu_fmt.N_PCI_Type=CF_PCI;
		temp_tx_pdu.n_pdu_fmt.SF_dl_all_FF_dl_low_CF_sn_FC_fs=(uint8_t)(*tx_SN&0x0F);
		(*tx_SN)++;
		if(*tx_SN>=16)*tx_SN=0;
		memcpy(&(temp_tx_pdu.n_pdu_fmt.CF_data1),data+(*had_tx_len),remain_bytes);
			
		memcpy(&can_tx_package.dataA[0],&temp_tx_pdu.DATA[0],8);	
		ret=can0_send_data((uint8_t *)&can_tx_package, sizeof(can0_data_package_t));
		if(ret!=sizeof(can0_data_package_t)) 
		{
			(*step)=0;
			return -8;
		}

		(*had_tx_len)+=remain_bytes;
		(*step)=0;
		return 0;//ok, ending tx
	}
	
	(*step)=0;
	return 0;
}



#define _____interface_to_caller______________________________________________________________




/*
	return 
		   <0: input para error
			0: send fail
			1: send successfully
*/
int APP_send_ISO15765_data(uint8_t *data_ptr,uint16_t data_len)
{
	OS_ERR os_err;
	uint8_t *malloc_ptr;

	if(data_len<=4095)
	{
		malloc_ptr = user_malloc(data_len,__FUNCTION__);
		if(malloc_ptr!=NULL)
		{
			memcpy(malloc_ptr,data_ptr,data_len);
			
			OSTaskQPost(&ISO15765Task_Tcb,malloc_ptr,data_len,OS_OPT_POST_FIFO,&os_err);
			if(os_err==OS_ERR_NONE)
			{
				InfoPrintf("APP_TO_NWL_send_data ok (%d bytes)!!\r\n",data_len);
				return 1;
			}
			else 
			{
				InfoPrintf("APP_TO_NWL_send_data faile -%d-\r\n",os_err);
				user_free(malloc_ptr,__FUNCTION__);
				return 0;				
			}
		}
        else
        {
            return -2;
        }
	}
	else
	{
		return -1;	
	}
}



void ISO15765Task(void *p_arg)
{
	OS_ERR os_err;
	//int i=0;
	can0_data_package_t can_rx_package;
	static uint32_t timestamp=0;
	//can0_data_package_t can_tx_msg[5];
	//unsigned char *ptr=NULL;
	uint16_t sent_len=0;
	uint16_t need_send_len;
	uint8_t  *need_send_ptr;
	int ret;

	//rx variable
	//uint16_t rx_app_data_len=0;

	//os variable
	OS_MSG_SIZE task_Q_len=0;
	uint8_t *task_Q_ptr=NULL; //need to be free
	
	init_pdu_Rx_ctl();
	init_pdu_Tx_ctl();
	ISO15765_init_NWL_timer();

	//don`t delete , wait can peripheral initialed ok !!
	user_delay_ms(TIME_10ms);

	//ISO15765 RECEIVE AND TRANSMIT PROCESS .....
	while(1)
	{
		if(OSTimeGet(&os_err)-timestamp>5000)
		{
			timestamp=OSTimeGet(&os_err);
			InfoPrintf("ISO15765Task loop...\r\n");
		}

		//check whether have data need to send
		if(pdu_Tx_ctl.NWL_now_sending_flag==0)
		{
			task_Q_ptr=OSTaskQPend(TIME_10ms,OS_OPT_PEND_NON_BLOCKING,&task_Q_len,NULL,&os_err);
			if(task_Q_ptr!=NULL)
			{
				pdu_Tx_ctl.NWL_now_sending_flag=1;
				pdu_Tx_ctl.tx_step=0;
				
				pdu_Tx_ctl.initiate_tx_flag=1;
				pdu_Rx_ctl.rx_FC_WAIT_flag=0;
				pdu_Rx_ctl.rx_FC_OVFLW_flag=0;
				need_send_len=task_Q_len;
				need_send_ptr=task_Q_ptr;
				sent_len=0;
				//InfoPrintf("-%d-触发发送(%d)bytes:\r\n",OSTimeGet(&os_err),task_Q_len);
				//for(i=0;i<task_Q_len;i++)InfoPrintf("%02X,",task_Q_ptr[i]);InfoPrintf("\r\n");
			}
		}

		while(1)
		{
			//send data (network layer)#################################################################################
			if(	 ((ISO15765_ck_tx_data_time_interval(ISO15765_get_tx_time_tree(),
				 									 ISO15765_get_curr_time_stamp(),
													 pdu_Rx_ctl.rx_responser_FC_STmin))==1)//RX FC frame rightly && no timeout
													 
			   &&(pdu_Tx_ctl.initiate_tx_flag==1||pdu_Rx_ctl.rx_FC_CTS_flag==1)//init send,or has rx fc 
			   
			   &&(pdu_Tx_ctl.NWL_now_sending_flag==1)
			   
			   &&(need_send_len>=1)
			  )
			{
				pdu_Tx_ctl.initiate_tx_flag=0;//clear
				
				ret=ISO15765_pro_LKLY_tx_data(&pdu_Tx_ctl.tx_step,need_send_ptr,need_send_len,&sent_len,&pdu_Tx_ctl.tx_SN);
				//tx status process.............................................................................
				if(ret==0)
				{
					//InfoPrintf("-%d-ISO15765 send %d bytes successfully\r\n",OSTimeGet(&os_err),sent_len);
					sent_len=0;
					pdu_Tx_ctl.tx_step=0;
					pdu_Rx_ctl.rx_FC_CTS_flag=0;
					pdu_Tx_ctl.NWL_now_sending_flag=0;
					user_free(need_send_ptr,__FUNCTION__);
					//confirm to upper layer
					//...
					//...
				}
				else if(ret==1)
				{
					pdu_Tx_ctl.tx_FC_BS_counter=0;
					pdu_Rx_ctl.rx_FC_CTS_flag=0;//use to wait FC after tx FF
					//InfoPrintf("wait FC frame\r\n");
				}
				else if(ret==2)
				{
					/*InfoPrintf("tx %dth CF ok,接收端BS=%d\r\n",
															pdu_Tx_ctl.tx_FC_BS_counter,
															pdu_Rx_ctl.rx_responser_FC_BS);*/
					pdu_Tx_ctl.tx_FC_BS_counter++;
					
					if(  (pdu_Tx_ctl.tx_FC_BS_counter < pdu_Rx_ctl.rx_responser_FC_BS)
					   ||(pdu_Rx_ctl.rx_responser_FC_BS==0)
					  )
					{
						ISO15765_update_tx_time_tree();
						//InfoPrintf("继续.............\r\n");
						continue;
					}
					else
					{
						//InfoPrintf("tx 1 BS end !!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
						pdu_Tx_ctl.tx_FC_BS_counter=0;
						pdu_Rx_ctl.rx_FC_CTS_flag=0;
					}
				}
				else
				{
					InfoPrintf("ISO15765 send %d bytes error,err=%d\r\n",sent_len,ret);
					sent_len=0;
					pdu_Tx_ctl.tx_step=0;
					pdu_Rx_ctl.rx_FC_CTS_flag=0;
					pdu_Tx_ctl.NWL_now_sending_flag=0;
					user_free(need_send_ptr,__FUNCTION__);
					//confirm to upper layer
					//...
					//...
				}
				
				ISO15765_update_tx_time_tree();
			}
			else 
			{
				pdu_Tx_ctl.initiate_tx_flag=0;//clear
				
				if(need_send_len==0 &&pdu_Tx_ctl.NWL_now_sending_flag==1)
				{
					InfoPrintf("NWL:tx len=0,err\r\n");
					sent_len=0;
					pdu_Tx_ctl.tx_step=0;
					pdu_Rx_ctl.rx_FC_CTS_flag=0;
					pdu_Tx_ctl.NWL_now_sending_flag=0;
					user_free(need_send_ptr,__FUNCTION__);
					//confirm to upper layer
					//...
					//...
				}
			}
			
			//tx wait,overflow,timeout pro
			if(pdu_Tx_ctl.NWL_now_sending_flag==1)
			{
				if(pdu_Rx_ctl.rx_FC_WAIT_flag==1)
				{
					InfoPrintf("RX WAIT FC ...re-init timeout counter...\r\n");
					//re-init the timeout counter
					ISO15765_update_tx_time_tree();
					pdu_Rx_ctl.rx_FC_WAIT_flag=0;
				}

				if(pdu_Rx_ctl.rx_FC_OVFLW_flag==1)
				{
					InfoPrintf("RX OVFLW FC ...stop current tx operation!!!\r\n");
					ISO15765_update_tx_time_tree();
					pdu_Tx_ctl.tx_step=0;
					pdu_Rx_ctl.rx_FC_OVFLW_flag=0;
					pdu_Tx_ctl.NWL_now_sending_flag=0;
					user_free(need_send_ptr,__FUNCTION__);
					pdu_Rx_ctl.rx_FC_CTS_flag=0;
					sent_len=0;
					//confirm to upper layer
					//...
					//...
				}
			
				if(  (ISO15765_get_curr_time_stamp()-ISO15765_get_tx_time_tree() > ISO15765_TX_TIMEOUT)
				   &&(pdu_Rx_ctl.rx_FC_CTS_flag==0)	 
				)
				{
					sent_len=0;
					pdu_Tx_ctl.tx_step=0;
					pdu_Tx_ctl.NWL_now_sending_flag=0;
					user_free(need_send_ptr,__FUNCTION__);
					//confirm to upper layer
					//...
					//...
					InfoPrintf("tx timeout ...reinit...\r\n");
				}
			}
			
			//receive data (network layer)##############################################################################
			if(ISO15765_read_LKLY_data(&(can_rx_package.id),can_rx_package.dataA,&(can_rx_package.len)))
			{//received one can msg
				//pro one can msg
				ret=ISO15765_pro_LKLY_rx_data(&(can_rx_package.dataA[0]),&pdu_Rx_ctl.had_rx_info_len);
				//rx status process.........................................................................
				if(ret==PDU_RX_STEP_END)
				{	
					if(pdu_Rx_ctl.rx_result!=N_OK)
					{
						//indicate to upper layer
						//...
						//...
						InfoPrintf("ISO15765:rx end,rx_result=%d\r\n",pdu_Rx_ctl.rx_result);
						InfoPrintf("err reason:%d,received data will be abandoned\r\n",pdu_Rx_ctl.error_flag);
					}
					#if 0
					if(pdu_Rx_ctl.had_rx_info_len!=0 &&pdu_Rx_ctl.rx_result==N_OK&&loc_pdu_rx_buf!=NULL){
					InfoPrintf("ISO15765 RX(%d):",pdu_Rx_ctl.had_rx_info_len);
					for(i=0;i<pdu_Rx_ctl.had_rx_info_len;i++)
					InfoPrintf("%02X,",loc_pdu_rx_buf[i]);
					InfoPrintf("\r\n");}
					#endif

					if(pdu_Rx_ctl.had_rx_info_len>0)
					{
						OSQPost(&ISO15765_RX_APP_DATA_Q,loc_pdu_rx_buf,pdu_Rx_ctl.had_rx_info_len,OS_OPT_POST_FIFO,&os_err);	
						if(os_err!=OS_ERR_NONE)
						{
							InfoPrintf("post ISO15765_RX_APP_DATA_Q faile -%d-\r\n",os_err);
						}
					}
					pdu_Rx_ctl.had_rx_info_len=0;
					pdu_Rx_ctl.NWL_now_receiving_flag=0;
					pdu_Rx_ctl.rx_result=N_OK;
					pdu_Rx_ctl.error_flag=N_PDU_ERR_TYPE_NONE;
				}
				else if(ret==PDU_RX_STEP_CONTINUE)
				{
					//take care of CF rx timeout check
					pdu_Rx_ctl.NWL_now_receiving_flag=1;

					//update the time stamp
					ISO15765_update_rx_time_tree();

					if(pdu_Rx_ctl.rx_result!=N_OK)
					{
						InfoPrintf("rx continue,rx_result=%d\r\n",pdu_Rx_ctl.rx_result);
					}
				}
				else if(ret==PDU_RX_STEP_UNVALID_DATA)
				{
					InfoPrintf("ISO15765 RX unvalid data,reason:%d,will be abandoned\r\n",pdu_Rx_ctl.error_flag);
					InfoPrintf("but rx continue,rx_result=%d\r\n",pdu_Rx_ctl.rx_result);
				}
				else if(ret==PDU_RX_STEP_ERR_MALLOC)
				{
					pdu_Rx_ctl.had_rx_info_len=0;
					pdu_Rx_ctl.NWL_now_receiving_flag=0;
					pdu_Rx_ctl.rx_result=N_OK;
					pdu_Rx_ctl.error_flag=N_PDU_ERR_TYPE_NONE;
				}

				continue;
			}

			//rx timeout pro
			if(pdu_Rx_ctl.NWL_now_receiving_flag==1)
			{
				if(ISO15765_get_curr_time_stamp()-ISO15765_get_rx_time_tree() > ISO15765_RX_TIMEOUT)
				{
					InfoPrintf("ISO15765 RX timeout\r\n");
					
					pdu_Rx_ctl.NWL_now_receiving_flag=0;
					pdu_Rx_ctl.had_rx_info_len=0;
					pdu_Rx_ctl.current_rx_status = PDU_RX_STEP_END;

					//indicate to upper layer
					//...
					//...
				}	
			}

			break;
		}

		if(pdu_Tx_ctl.NWL_now_sending_flag==0)
		{
			user_delay_ms(TIME_10ms);
		}
		else
		{
			user_delay_ms(TIME_1ms);
		}
	}
}


