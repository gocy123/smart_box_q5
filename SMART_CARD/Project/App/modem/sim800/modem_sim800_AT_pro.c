#include "user_system_cfg.h"

#ifdef USE_SIM800_MODEM

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "os_cfg_app.h"			//TCB,STACK,PRIORITY,任务间消息的结构体定义和宏
#include "os.h"
#include "modem_sim800_AT_pro.h"
#include "uart1_network_layer.h"
#include "user_debug_app.h"


#define AT_TBL_LEN 9 
const char *at_tbl[AT_TBL_LEN]={
"\r\n+CSQ:",// 0
"\r\n+CREG",// 1
"\r\n+CGATT:",// 2
"\r\nCONNECT OK",// 3
"\r\nSHUT OK",// 4
"\r\nSTATE: CONNECT OK",//5
"\r\nERROR\r\n",// 6
"\r\nOK\r\n",// 7
"\r\nALREADY CONNECT"//8

};

#define AT_IN_BUF_LEN (128) 
static uint8_t inbuf[AT_IN_BUF_LEN]={0};  //用于保存:从RX中断缓冲buffer中读出的数据

extern OS_Q modem_send_at_rsp_q;
extern OS_Q modem_send_gprs_at_rsp_q;
extern OS_Q modem_receive_urc_q;
extern OS_Q modem_receive_gprs_data_q;

extern 	void *user_malloc(unsigned int size,char const*caller);
extern 	void user_free(void *rmem,char const*caller);


/*
input:
		frame:
		frame_len:
		Rxing_at_or_gprs:
						 1:正在接收GPRS数据
						 0:在接收非GPRS数据以外的数据
		wait_cip_ack:
					 1:正在等待 > 括号
					 0:其它		
return:
		0		: parse ok
		others	: failed

*/
//#define at_parse_one_frame_print
static int at_parse_one_frame(uint8_t *frame,uint32_t frame_len,char Rxing_at_or_gprs,char wait_cip_ack)
{
	OS_ERR os_err; 
	int i;
        //#ifdef at_parse_one_frame_print
	int j;
        //#endif       
	//int dlci;
	uint8_t *ptr=NULL;
	OS_Q *p_Q=NULL;

	//--------提取出一个完整帧--------------------
	
	//区分出GPRS数据帧,通用AT指令帧,SEND OK帧,URC帧
	if(Rxing_at_or_gprs==1)
	{
		//#ifdef at_parse_one_frame_print
		InfoPrintf("驱动层提取出%d字节GPRS数据:",frame_len);
		for(j=0;j<frame_len;j++)InfoPrintf("%02X",frame[j]);InfoPrintf("\r\n");
		//#endif
		p_Q=(&modem_receive_gprs_data_q);
	}
	else
	{		
		#ifdef at_parse_one_frame_print
		InfoPrintf("驱动层提取出%d字节AT指令数据:",frame_len);for(j=0;j<frame_len;j++){
		if(frame[j]!=0x0D && frame[j]!=0x0A&&frame[j]>0x1F&&frame[j]<0x7F)InfoPrintf("%c",frame[j]);}
		InfoPrintf("\r\n");
		#endif
	
		if(strncmp((char const *)frame,"\r\nSEND OK\r\n",11)==0)
		{
			p_Q = &modem_send_gprs_at_rsp_q;
		}
		else if(strncmp((char const *)frame,"\r\n> ",4)==0)
		{
			p_Q = &modem_send_gprs_at_rsp_q;			
		}
		else 	
		{	
			if((frame[2]>=0X30)&&(frame[2]<=0X39))//0~9 for ip ,10.21.255.210
			{
				p_Q=&modem_send_at_rsp_q;
			}
			else
			{
				for(i=0;i<AT_TBL_LEN;i++)
				{
					if(strncmp((char const *)frame,at_tbl[i],strlen(at_tbl[i]))==0)
					{
						//post at_rsp
						p_Q=&modem_send_at_rsp_q;
						if(i==6)// "\r\n ERROR \r\n"
						{
							if(wait_cip_ack>0)
							{
								p_Q=&modem_send_gprs_at_rsp_q;
							}
						}
						break;
					}
				}
				
				if(i>=AT_TBL_LEN)
				{	
					//post urc 
					#if 0
					InfoPrintf("URC帧:");for(j=0;j<frame_len;j++){
					if(ptr[j]!=0x0D && ptr[j]!=0x0A&&ptr[j]>0x1F&&ptr[j]<0x7F)
					InfoPrintf("%c",ptr[j]);}
					InfoPrintf("完Q毕\r\n");
					#endif
					p_Q = &modem_receive_urc_q;
				}
			}
		}
	}

	if(p_Q!=NULL)
	{
		ptr = (uint8_t*)user_malloc(frame_len+1,__FUNCTION__);	//额外增加一个字节,填值0x00，预防后续库函数 字符串操作越界
		if(ptr!=NULL)
		{
			//memset(ptr,0,frame_len);
			ptr[frame_len]=0;									//额外增加一个字节,填值0x00，预防后续库函数 字符串操作越界
			memcpy(ptr,frame,frame_len);
			#if 0
			InfoPrintf("driver post:");
			if(modem_get_ptr_recv_remote_data_q()==p_Q)
			{
				InfoPrintf("modem_recv_remote_data q \r\n");
			}
			else if(modem_get_ptr_gprs_at_rsp_q()==p_Q)
			{
				InfoPrintf("modem_send_gprs_at_rsp q \r\n");
			}
			else if(modem_get_ptr_send_at_rsp_q()==p_Q)
			{
				InfoPrintf("modem_send_at_rsp q \r\n");
			}
			else if(modem_get_ptr_receive_urc_q()==p_Q)
			{
				InfoPrintf("modem_receive_urc q \r\n");
			}
			else 
			{
				InfoPrintf("其他Q \r\n");
			}
			InfoPrintf(":");
			for(j=0;j<frame_len;j++)
			{
				if(ptr[j]!=0x0D && ptr[j]!=0x0A&&ptr[j]>0x1F&&ptr[j]<0x7F)
				InfoPrintf("%c",ptr[j]);
			}
			InfoPrintf("完毕\r\n");
			#endif

			OSQPost(p_Q,ptr,frame_len,OS_OPT_POST_FIFO,&os_err);
			if(os_err != OS_ERR_NONE)
			{
				InfoPrintf("post AT Q fail,%u,",os_err);		
				#if 0
				InfoPrintf("Q:");
				for(j=0;j<frame_len;j++)
				{
					if(ptr[j]!=0x0D && ptr[j]!=0x0A&&ptr[j]>0x1F&&ptr[j]<0x7F)
					InfoPrintf("%c",ptr[j]);
				}
				InfoPrintf("\r\n");
				#endif
				user_free(ptr,__FUNCTION__);
			}
			return os_err;
		}
		else
		{	
			InfoPrintf("tx Q 分配RAM 失败:");
			if(&modem_receive_gprs_data_q==p_Q)
			{
				InfoPrintf("modem_recv_remote_data q \r\n");
			}
			else if(&modem_send_gprs_at_rsp_q==p_Q)
			{
				InfoPrintf("modem_send_gprs_at_rsp q \r\n");
			}
			else if(&modem_send_at_rsp_q==p_Q)
			{
				InfoPrintf("modem_send_at_rsp q \r\n");
			}
			else if(&modem_receive_urc_q==p_Q)
			{
				InfoPrintf("modem_receive_urc q \r\n");
			}
			else 
			{
				InfoPrintf("其他Q \r\n");
			}
			return -1;
		}
	}
	else
	{
		InfoPrintf("不需要post\r\n");
	}
	return 0;
}


/*	
brief: extract one frame from the ringbuffer

	input :
			inbuf    : wait for extract buf
			inbuf_len: wait buf`s len
	output:
			extracted_frame_len: extracted frame`s length; if >0 ,means extracted one frame successfully
			at_or_gprs_mode: 
							0: general AT cmd mode
							1: RXing GPRS data 
	return:
			-1		: error calling
			others	: need_shift_out_amount
*/

static int at_extract_one_rx_frame(char *inbuf,uint32_t inbuf_cache_len,uint32_t *extracted_frame_len, char *at_or_gprs_mode)
{
	int i=0;
	//uint8_t result=0;
	//static uint8_t len_size;
	//int unuseful_amount=0;	
	static uint32_t need_rx_gprs_data_len=0; //+IPD,xxx: ,rx GPRS数据帧length
	int need_shift_out_amount=0;

	*extracted_frame_len=0;

	//--1-----------------------------------------------------------------------------
	//rx gprs data
	if(*at_or_gprs_mode==1)
	{
		if(inbuf_cache_len>=need_rx_gprs_data_len)
		{
			*extracted_frame_len  = need_rx_gprs_data_len;
			need_shift_out_amount = need_rx_gprs_data_len; 
			//*at_or_gprs_mode=0;
		}
		else
		{
			need_shift_out_amount=0;
		}
		
		return need_shift_out_amount;
	}

	//--2-- FIND HEAD ----general at cmd data,except the essential gprs data----------------
	//rx AT cmd
	for(i=0;i<inbuf_cache_len-1;i++)
	{
		if((inbuf[i]==0x0D)&&(inbuf[i+1]==0x0A))//(porting area...........)
		{
			if(i==0)
			{
				break;
			}
			else
			{
				#if 0
				InfoPrintf("HEAD无效字节---%d---\r\n");
				for(j=0;j<i;j++)InfoPrintf("%02X,",inbuf[j]);
				InfoPrintf("\r\n");
				#endif
				need_shift_out_amount=i; 
				
				return need_shift_out_amount;	
			}
		}
	}

	//--3--已确保inbuf的 (前2字节为:0x0D,0x0A)     并且 inbuf_len>=4------------------------
	
	if(inbuf_cache_len>=9)//9 
	{
		// " \r\n+IPD,1:  7E   XX XX XX 7E "
		if(strncmp(inbuf,"\r\n+IPD,",7)==0)		//(porting area...........)
		{
			for(i=7;i<inbuf_cache_len;i++)
			{
				if(inbuf[i]==':')				//(porting area...........)
				{
					sscanf((char*)(inbuf+7),"%u",&need_rx_gprs_data_len);
					
					if((need_rx_gprs_data_len<=AT_IN_BUF_LEN-20) && (need_rx_gprs_data_len>0))//预留20个字节
					{
						*at_or_gprs_mode=1;//set
						//shift out "\r\n+IPD,123:" char bytes
						need_shift_out_amount=i+1; 			
					}
					else
					{
						//GPRS 单包数据超长,终端无法缓冲.
						//shift out all cache bytes
						need_shift_out_amount=inbuf_cache_len;
					}
					
					return need_shift_out_amount;						
				}

				//若没有找到":"怎么办 ??
				if(i>=12)//12只是举例,12-7=5,最长 99999,已经超长了,所以12足够了
				{
					need_shift_out_amount=12;
					return need_shift_out_amount;
				}
			}
		}
	}
	
	//--4--找 > ------------------------------------------------------------------------------
	if(strncmp(inbuf,"\r\n> ",4)==0)			//(porting area...........)
	{
		*extracted_frame_len=4;
		need_shift_out_amount=4;
		return need_shift_out_amount;
	}

	//--5--一般的AT指令----------------------------------------------------------------------
	for(i=2;i<inbuf_cache_len-1;i++)
	{
		if((inbuf[i]==0x0D)&&(inbuf[i+1]==0x0A))//(porting area...........)
		{
			if(i>2)
			{
				*extracted_frame_len  = i+2;
				need_shift_out_amount = i+2; 
				return need_shift_out_amount;
			}
			else
			{
				//InfoPrintf("AT无效字节--2-(0D 0A)--\r\n");
				need_shift_out_amount=2;
				return need_shift_out_amount;
			}
		}

		if(i>100)//仅支持最长100个字节的AT命令响应 user define
		{
			InfoPrintf("extract at fail,len err!!!!!\r\n"); 
			need_shift_out_amount=100;
			return need_shift_out_amount;
		}
	}

	need_shift_out_amount=0;
	return need_shift_out_amount;
}

int at_pro_2g_module_data(char *at_gprs_mode,char *wait_cipsend_ack)
{
	static uint32_t remain_len=0;	
	int this_time_read_len=0;
	uint32_t extractOut_frame_len=0;
	int shift_len=0; 
	
	if(remain_len>AT_IN_BUF_LEN)
	{
		InfoPrintf("remain_len overflow ,error!!!!!\r\n"); 
		remain_len=0;
	}
		
	this_time_read_len=uart1_receive_data(inbuf+remain_len,AT_IN_BUF_LEN-remain_len);
	if(this_time_read_len>0)		
	{		
		//InfoPrintf("====================%d\r\n",this_time_read_len); 
	
		remain_len+=this_time_read_len;
		
		while(1)	
		{
			if(remain_len>=4)
			{
				shift_len=at_extract_one_rx_frame((char *)inbuf,remain_len,&extractOut_frame_len,at_gprs_mode);
				if(shift_len <= remain_len)
				{	
					remain_len-=shift_len;
					
					if(shift_len>0)	//check one frame ok
					{
						if(extractOut_frame_len>0)
						{					
							at_parse_one_frame(inbuf,extractOut_frame_len,*at_gprs_mode,*wait_cipsend_ack);	
							*at_gprs_mode=0;
						}
						
						memmove(inbuf,inbuf+shift_len,remain_len);
					}
					else           	//check one frame fail, but not error
					{
						break;		//数据有效,但暂未构成完整帧
					}
				}
				else
				{
					InfoPrintf("shift_len error\r\n");  
					remain_len=0;
					*at_gprs_mode=0;
					return -1;
				}
			}
			else
			{
				break;
			}
		}
	}
	else if(this_time_read_len==0)
	{
		//缓存数据 小于 最小数据单元
	}
    else if(this_time_read_len<0)
    {
  		InfoPrintf("uart1_receive_data(),input len error\r\n");    
    }
	
	return 0;
}

#endif


