#include "user_system_cfg.h"

#ifdef USE_SIM6320_MODEM

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "os_cfg_app.h"			//TCB,STACK,PRIORITY,任务间消息的结构体定义和宏
#include "os.h"
#include "modem_sim6320_AT_pro.h"
#include "uart1_network_layer.h"
#include "user_debug_app.h"


#define AT_TBL_LEN (  20   +  1) 
const char *at_tbl[AT_TBL_LEN]={
"\r\n+CSQ:",// 0
"\r\n+CREG",// 1
"\r\n+CGATT:",// 2
"\r\nCONNECT OK",// 3
"\r\nSHUT OK",// 4
"\r\nSTATE: CONNECT OK",//5
"\r\nERROR\r\n",// 6
"\r\nOK\r\n",// 7
"\r\nALREADY CONNECT",//8
//--------------6320 special-------------------------
"\r\n+SMEID:",		//9
"\r\n+CCLK:",		//10 current date & time
"\r\n^HDRCSQ:",		//11
"\r\n+CGREG:",		//12
"\r\n+CGMR:",		//13 module sw version
"\r\n+NETOPEN:",	//14
"\r\n+IP ERROR: Network is already closed",	//15
"\r\n+NETOPEN:",	//16
"\r\n+CIPOPEN:",	//17
"\r\n+CIPSEND:",	//18
"\r\n+ICCID:",		//19
"\r\n+IP ERROR: Network is already opened"	//20

};

#define AT_IN_BUF_LEN (1550) 
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
#define at_parse_one_frame_print
static int at_parse_one_frame(uint8_t *frame,int frame_len,char Rxing_at_or_gprs,char wait_cip_ack)
{
	OS_ERR os_err; 
	int i;
	int j;
	//int dlci;
	uint32_t malloc_len =0;
	uint8_t *ptr=NULL;
	OS_Q *p_Q=NULL;

	//static uint8_t flag =0;

	

	//--------提取出一个完整帧--------------------
	
	//区分出GPRS数据帧,通用AT指令帧,SEND OK帧,URC帧
	if(Rxing_at_or_gprs==1)
	{
		#ifdef at_parse_one_frame_print
		//#if 0
		InfoPrintf("驱动层---提取出%d字节GPRS数据:",frame_len);
		for(j=0;j<frame_len;j++)InfoPrintf("%02X",frame[j]);InfoPrintf("\r\n");
		#endif
		p_Q=(&modem_receive_gprs_data_q);
	}
	else
	{		
		#ifdef at_parse_one_frame_print
		InfoPrintf("驱动层---提取出%d字节AT指令数据:",frame_len);
		for(j=0;j<frame_len;j++)
		{
			//if(frame[j]!=0x0D && frame[j]!=0x0A&&frame[j]>0x1F&&frame[j]<0x7F)
			InfoPrintf("%c",frame[j]);
		}
		//InfoPrintf("\r\n");
		#endif
	
		if(strncmp((char const *)frame,"\r\n+CIPSEND: 0,",14)==0)
		{
			p_Q = &modem_send_gprs_at_rsp_q;
			InfoPrintf("驱动层-1-: modem_send_gprs_at_rsp_q\r\n");
		}
		else if(strncmp((char const *)frame,"\r\nRECV FROM:183.230.40.42:4362",28)==0)
		{
			InfoPrintf("驱动层-1-: 收到GPRS数据!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
		}
		else if(strncmp((char const *)frame,"\r\n>",3)==0)
		{
			p_Q=&modem_send_gprs_at_rsp_q;		
			InfoPrintf("驱动层-2-: modem_send_gprs_at_rsp_q\r\n");
		}
		else 	
		{	
			if((frame[2]>=0X30)&&(frame[2]<=0X39))//0~9 for ip ,10.21.255.210
			{
				p_Q=&modem_send_at_rsp_q;
				InfoPrintf("驱动层-3-: modem_send_at_rsp_q\r\n");
			}
			else
			{
				for(i=0;i<AT_TBL_LEN;i++)
				{
					if(strncmp((char const *)frame,at_tbl[i],strlen(at_tbl[i]))==0)
					{
						//post at_rsp
						p_Q=&modem_send_at_rsp_q;
						InfoPrintf("驱动层-4-: modem_send_at_rsp_q\r\n");
						if(i==6)// "\r\n ERROR \r\n"
						{
							if(wait_cip_ack>0)
							{
								p_Q=&modem_send_gprs_at_rsp_q;
								InfoPrintf("驱动层-5-: modem_send_gprs_at_rsp_q\r\n");
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
					InfoPrintf("驱动层-6-: modem_receive_urc_q\r\n");
				}
			}
		}
	}

	if(p_Q!=NULL)
	{
		malloc_len = frame_len;
		
		ptr = (uint8_t*)user_malloc(malloc_len,__FUNCTION__);
		if(ptr!=NULL)
		{
			memset(ptr,0,malloc_len);
			memcpy(ptr,frame,frame_len);
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

static int at_extract_one_rx_frame(char *inbuf,int inbuf_len,int *extracted_frame_len, char *at_or_gprs_mode)
{
	int i=0;
    int j=0;
	//uint8_t result=0;
	//static uint8_t len_size;
	//int unuseful_amount=0;	
	static uint32_t need_rx_gprs_data_len=0; //+IPD,xxx: ,rx GPRS数据帧length
	int need_shift_out_amount=0;

	*extracted_frame_len=0;

	#if 1
	{
		InfoPrintf("----------开始----------------------------\r\n");
		InfoPrintf("缓冲buf:");
		for(i=0;i<inbuf_len;i++)
		{
			InfoPrintf("%02X,",inbuf[i]);
		}
		InfoPrintf("\r\n--------------------------------------\r\n");
		for(i=0;i<inbuf_len;i++)
		{
			InfoPrintf("%c",inbuf[i]);
		}
		InfoPrintf("\r\n--------结束------------------------------\r\n");
	}

	#endif

	

	//--1-----------------------------------------------------------------------------
	//rx gprs data
	if(*at_or_gprs_mode==1)
	{//正在接收GPRS数据
		if(inbuf_len>=need_rx_gprs_data_len)
		{
			*extracted_frame_len  = need_rx_gprs_data_len;
			need_shift_out_amount = need_rx_gprs_data_len; 
		}
		else
		{
			need_shift_out_amount=0;
		}
		
		return need_shift_out_amount;
	}

	//--2-- FIND HEAD ------------------------------------------------------------------
	//rx AT cmd
	for(i=0;i<inbuf_len-1;i++)
	{
		if((inbuf[i]==0x0D)&&(inbuf[i+1]==0x0A))//(porting area...........)
		{
			if(i==0)
			{
				break;
			}
			else
			{
				#if 1
				InfoPrintf("HEAD无效字节---%d---\r\n",i);
				for(j=0;j<i;j++)InfoPrintf("%02X,",inbuf[j]);
				InfoPrintf("\r\n---------------\r\n");
				for(j=0;j<i;j++)InfoPrintf("%c",inbuf[j]);
				InfoPrintf("\r\n");InfoPrintf("\r\n");InfoPrintf("\r\n");
				#endif
				need_shift_out_amount=i; 
				
				return need_shift_out_amount;	
			}
		}
	}

	//--3--已确保inbuf的前2字节为 0x0D,0x0A 并且 inbuf_len>=4------------------------------ 
	#if 1
	if(inbuf_len>=8)//(porting area...........)
	{
		// " \r\n+IPD,1:  7E   XX XX XX 7E "
		if(strncmp(inbuf,"\r\n+IPD",6)==0)		//(porting area...........)
		{
			InfoPrintf("提取出+IPD\r\n");
			for(i=6;i<inbuf_len;i++)
			{
				if(inbuf[i]=='\r')				//(porting area...........)
				{
					sscanf((char*)(inbuf+6),"%u",&need_rx_gprs_data_len);
					InfoPrintf("需要接收 %d 字节的GPRS数据\r\n",need_rx_gprs_data_len);
					
					if(need_rx_gprs_data_len<=AT_IN_BUF_LEN-20)//预留20个字节
					{
						*at_or_gprs_mode=1;//set
						//shift out "\r\n+IPD,123:" char bytes
						need_shift_out_amount=i+1;		
					}
					else
					{
						//GPRS 单包数据超长,终端无法缓冲.
						//shift out all char bytes
						need_shift_out_amount=AT_IN_BUF_LEN;
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
	#else
	if(inbuf_len>=6)//(porting area...........)
	{
		// " \r\n+IPD,1:  7E   XX XX XX 7E "
		if(strncmp(inbuf,"+IPD",4)==0)		//(porting area...........)
		{
			InfoPrintf("提取出+IPD\r\n");
			for(i=4;i<inbuf_len;i++)
			{
				if(inbuf[i]=='\r')				//(porting area...........)
				{
					sscanf((char*)(inbuf+4),"%u",&need_rx_gprs_data_len);
					InfoPrintf("需要接收 %d 字节的GPRS数据\r\n",need_rx_gprs_data_len);
					
					if(need_rx_gprs_data_len<=AT_IN_BUF_LEN-20)//预留20个字节
					{
						*at_or_gprs_mode=1;//set
						//shift out "\r\n+IPD,123:" char bytes
						need_shift_out_amount=i+1;		
					}
					else
					{
						//GPRS 单包数据超长,终端无法缓冲.
						//shift out all char bytes
						need_shift_out_amount=AT_IN_BUF_LEN;
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
	#endif
	
	//--4--找 > ------------------------------------------------------------------------------
	if(strncmp(inbuf,"\r\n>",3)==0)			//(porting area...........)
	{
		InfoPrintf("提取出尖括号\r\n");
		*extracted_frame_len=3;
		need_shift_out_amount=3;
		return need_shift_out_amount;
	}

	//--5--一般的AT指令----------------------------------------------------------------------
	for(i=2;i<inbuf_len-1;i++)
	{
		if((inbuf[i]==0x0D)&&(inbuf[i+1]==0x0A))//(porting area...........)
		{
			if(i>2)
			{
				InfoPrintf("提取出一般的AT命令\r\n");
				*extracted_frame_len  = i+2;
				need_shift_out_amount = i+2; 
				#if 1
				{
					InfoPrintf("----------一般的AT命令开始----------------------------\r\n");
					InfoPrintf("缓冲buf:");
					for(i=0;i<need_shift_out_amount;i++)
					{
						InfoPrintf("%02X,",inbuf[i]);
					}
					InfoPrintf("\r\n--------------------------------------\r\n");
					for(i=0;i<need_shift_out_amount;i++)
					{
						InfoPrintf("%c",inbuf[i]);
					}
					InfoPrintf("\r\n--------一般的AT命令结束------------------------------\r\n");
				}
				#endif
				return need_shift_out_amount;
			}
			else
			{
				InfoPrintf("提取出AT无效字节--2-(0D 0A)--\r\n");
				need_shift_out_amount=2;
				return need_shift_out_amount;
			}
		}

		if(i>100)//仅支持最长100个字节的AT命令响应 user define
		{
			need_shift_out_amount=100;
			return need_shift_out_amount;
		}
	}

	InfoPrintf("提取错误\r\n");
	need_shift_out_amount=0;
	return need_shift_out_amount;
}

int at_pro_2g_module_data(char *at_gprs_mode,char *wait_cipsend_ack)
{
	static int remain_len=0;	
	int read_len=0;
	int extractOut_buf_len=0;
	int shift_len=0; 
	
	read_len=uart1_receive_data(inbuf+remain_len,AT_IN_BUF_LEN-remain_len);
	if(read_len>0)		
	{		
		remain_len+=read_len;
		
		while(1)	
		{
			if(remain_len>=3)
			{
				shift_len=at_extract_one_rx_frame((char *)inbuf,remain_len,&extractOut_buf_len,at_gprs_mode);
				remain_len-=shift_len;
				
				if(shift_len>0)
				{
					#if 0
					{
						int i;
						InfoPrintf("----------开始----------------------------\r\n");
						InfoPrintf("缓冲buf:");
						for(i=0;i<remain_len;i++)
						{
							InfoPrintf("%02X,",inbuf[i]);
						}
						InfoPrintf("\r\n--------------------------------------\r\n");
						for(i=0;i<remain_len;i++)
						{
							InfoPrintf("%c",inbuf[i]);
						}
						InfoPrintf("\r\n--------结束------------------------------\r\n");
					}

					#endif
				
					if(extractOut_buf_len>0)
					{					
						at_parse_one_frame(inbuf,extractOut_buf_len,*at_gprs_mode,*wait_cipsend_ack);	
						*at_gprs_mode=0;
					}
					
					memmove(inbuf,inbuf+shift_len,remain_len);
				}
				else
				{//数据有效,但暂未构成完整帧
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
    else if(read_len<0)
    {
  		InfoPrintf("uart1_receive_data(),input len error\r\n");    
    }
	return 0;
}

#endif


