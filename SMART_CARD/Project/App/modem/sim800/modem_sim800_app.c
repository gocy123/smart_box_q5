#include "user_system_cfg.h"

#ifdef USE_SIM800_MODEM

#define PORT_TEST_MODEM 

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "os_cfg_app.h"			//TCB,STACK,PRIORITY,任务间消息的结构体定义和宏

#include "os.h"
#include "heads.h"
#include "uart1_network_layer.h"
#include "modem_sim800_AT_pro.h"
#include "modem_sim800_app.h"

#include "user_debug_app.h"


/*
此结构体:用于TCP连接, SOCKET创建与维护 的相关的变量标志控制(属于驱动层);
但是,不涉及 应用层空中协议相关的控制,如登陆,心跳,退陆等,它们被设计在report_task中(属于应用层).*/
static modem_t modem_ctl={0};

OS_Q 		modem_send_at_rsp_q;
OS_Q 		modem_send_gprs_at_rsp_q;
OS_Q 		modem_receive_urc_q;
OS_Q 		modem_receive_gprs_data_q; 

static OS_TMR 		modem_led_control_tmr;
static OS_MUTEX 	modem_uart_tx_ringbuf_mutex={0};//在多个任务里，可能嵌套使用该串口资源,所以要互斥
static OS_MUTEX 	modem_send_net_data_mutex={0};

extern uint8_t     	get_already_login_flag(void);
extern int 			get_gprs_send_fun(void);


//代其他文件定义
#ifdef PORT_TEST_MODEM 
static char* get_remote_dn(void)
{
	#ifdef SGMW_IP
	static char *ip="120.55.166.5";
	#endif
	#ifdef WEIGE_IP
	static char *ip="120.25.67.37";
	#endif
	#ifdef ONENET_IP
	static char *ip="183.230.40.42";
	#endif
	#ifdef EXLIVE_IP
	static char *ip="121.42.162.93";
	#endif

	return ip;
}

static uint16_t get_remote_port(void)
{
	#ifdef SGMW_IP
	return 12398;
	#endif
	#ifdef WEIGE_IP
	return 8008;
	#endif
	#ifdef ONENET_IP
	return 4362;
	#endif
	#ifdef EXLIVE_IP
	return 6973;
	#endif
}



uint8_t get_modem_err_state(void)
{
  return 0;
}

#endif


#define __MODEM____DRIVER________


/*
	input:
			0: led off
			1: led on
			2: toggle
*/
static void modem_led_OnOff(uint8_t ctl)
{
	static uint8_t toggle_cnt=0;
	
	/*PINSEL_ConfigPin(4, 4, 0);
	GPIO_SetDir(4, (1<<4), GPIO_DIRECTION_OUTPUT);

	
	if(ctl==1)
	{
		GPIO_OutputValue(4, (1<<4), 0);
	}
	else if(ctl==2)
	{
		toggle_cnt++;
		if((toggle_cnt%2)==0)
		{
			GPIO_OutputValue(4, (1<<4), 0);
		}
		else
		{
			GPIO_OutputValue(4, (1<<4), 1);
		}
	}
	else
	{
		GPIO_OutputValue(4, (1<<4), 1);
	}*/
}


/*
并没有严格按照模块的上电顺序要求!!!!
*/
void modem_power_up(void)
{
	//modem VBUS Power
	/*PINSEL_ConfigPin(2, 3, 0);
	GPIO_SetDir(2, (1<<3), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(2, (1<<3), 0);

	//modem PWRKEY
	PINSEL_ConfigPin(1, 27, 0);
	GPIO_SetDir(1, (1<<27), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(1, (1<<27), 1);

	//modem reset pin
	PINSEL_ConfigPin(2, 2, 0);
	GPIO_SetDir(2, (1<<2), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(2, (1<<2), 0);

	//modem 4.1V power en
	PINSEL_ConfigPin(2, 9, 0);
	GPIO_SetDir(2, (1<<9), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(2, (1<<9), 1);*/
}

void modem_power_down(void)
{
	//3g_en pin
	//modem 4.1V power en
	/*PINSEL_ConfigPin(2, 9, 0);
	GPIO_SetDir(2, (1<<9), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(2, (1<<9), 0);

	//modem PWRKEY
	PINSEL_ConfigPin(1, 27, 0);
	GPIO_SetDir(1, (1<<27), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(1, (1<<27), 0);

	//reset pin
	PINSEL_ConfigPin(2, 2, 0);
	GPIO_SetDir(2, (1<<2), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(2, (1<<2), 0);

	//modem VBUS Power
	PINSEL_ConfigPin(2, 3, 0);
	GPIO_SetDir(2, (1<<3), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(2, (1<<3), 0);*/
}

void modem_open_module(void)
{
	InfoPrintf("modem_open_module()...\r\n");	
	modem_power_up();
	user_uart1_open();
}

void modem_close_module(void)
{
	user_uart1_close();
	modem_power_down();
}

#define __MODEM_____APP___________

void *modem_pend_send_at_rsp_q(uint16_t wait_time,uint16_t *msg_size)
{
	uint8_t *ptr;
	OS_ERR os_err;
	ptr=(uint8_t *)OSQPend(&modem_send_at_rsp_q,wait_time,OS_OPT_PEND_BLOCKING,msg_size,NULL,&os_err);
	if((os_err==OS_ERR_NONE)&&(ptr!=NULL))
	{
		return ptr;
	}

	if(ptr!=NULL)
	{
		user_free(ptr,__FUNCTION__);
	}
	return NULL;
}

void *modem_pend_gprs_at_rsp_q(uint16_t wait_time,uint16_t *msg_size)
{
	uint8_t *ptr;
	OS_ERR os_err;
	ptr=(uint8_t *)OSQPend(&modem_send_gprs_at_rsp_q,wait_time,OS_OPT_PEND_BLOCKING,msg_size,NULL,&os_err);
	if((os_err==OS_ERR_NONE)&&(ptr!=NULL))
	{
		return ptr;
	}

	if(ptr!=NULL)
	{
		user_free(ptr,__FUNCTION__);
	}
	return NULL;
}

void *modem_pend_receive_urc_q(uint16_t wait_time,uint16_t *msg_size)
{
	uint8_t *ptr;
	OS_ERR os_err;
	ptr=(uint8_t *)OSQPend(&modem_receive_urc_q,wait_time,OS_OPT_PEND_BLOCKING,msg_size,NULL,&os_err);
	if((os_err==OS_ERR_NONE)&&(ptr!=NULL))
	{
		return ptr;
	}

	if(ptr!=NULL)
	{
		user_free(ptr,__FUNCTION__);
	}
	return NULL;
}


void net_state_init(void)
{
	modem_ctl.rx_gprs_state=0;
	modem_ctl.waiting_cipsend_ack=0;
	modem_ctl.socket_link_state = 0;
}

void net_state_deinit(void)
{
	modem_ctl.socket_link_state = 0;
}


/*
	brief: pack *buf data into Multiplexer format

	return: 
			0		: successful
			others	: failed
*/
static int modem_tx_at_cmd(uint8_t *src)
{
	int ret;
    OS_ERR os_err;
    
	OSMutexPend(&modem_uart_tx_ringbuf_mutex,0,OS_OPT_PEND_BLOCKING,NULL,&os_err);
	ret=uart1_send_data((uint8_t*)src, strlen((char const *)src));
	if(ret == strlen((char const *)src))
		ret = 0;
	else
		ret = 1;	
	OSMutexPost(&modem_uart_tx_ringbuf_mutex,OS_OPT_POST_NONE,&os_err);
	return ret;
}

static int modem_wr_gprs_data(uint8_t *data,uint32_t len)
{
	int wr_len;
	OS_ERR os_err;
	
	OSMutexPend(&modem_uart_tx_ringbuf_mutex,0,OS_OPT_PEND_BLOCKING,NULL,&os_err);
	wr_len= uart1_send_data((uint8_t*)data, len);
	OSMutexPost(&modem_uart_tx_ringbuf_mutex,OS_OPT_POST_NONE,&os_err);
	return wr_len;
}

int modem_send_net_data(uint8_t *p_data,uint32_t len)
{
	//#define modem_send_net_data_print
	char at_buf[18]="AT+CIPSEND=";
	char *p_msg;
	uint16_t msg_size;
	OS_ERR os_err;
	int i;
	int ret_len=0;

	if(modem_ctl.socket_link_state != 1)
	{ 
		user_delay_ms(TIME_100ms);
		InfoPrintf("modem_send_net_data() failed --0--!!!\r\n");
		return 0;
	}

	if(get_gprs_send_fun()==0)
	{
		user_delay_ms(TIME_100ms);
		InfoPrintf("modem_send_net_data() failed --GPRS SERVER CLOSE--!!!\r\n");
		return 0;		
	}
	
	OSMutexPend(&modem_send_net_data_mutex,0,OS_OPT_PEND_BLOCKING,NULL,&os_err);

	sprintf( (char*)&at_buf[strlen((const char*)at_buf)], "%u\r", len);

	user_delay_ms(TIME_10ms);
	p_msg=modem_pend_gprs_at_rsp_q(1,&msg_size);
	if(p_msg != NULL)
	{		
		user_free(p_msg,__FUNCTION__);
	}
	
	#ifdef modem_send_net_data_print
	InfoPrintf("%s\r\n",at_buf);
	#endif
	
	//--TX " AT+CIPSEND=xxx\r "
	modem_tx_at_cmd((uint8_t *)at_buf);
	modem_ctl.waiting_cipsend_ack=1;
	//wait for '>'
	p_msg=modem_pend_gprs_at_rsp_q(T_3S,&msg_size);
	modem_ctl.waiting_cipsend_ack=0;
	if(p_msg != NULL)
	{
		#ifdef modem_send_net_data_print
		InfoPrintf("等 > 括号结果:%s\r\n",p_msg);user_delay_ms(TIME_100ms);
		#endif
	
        if( strchr((const char*)p_msg,'>') != NULL)
        {
			user_free(p_msg,__FUNCTION__);
			
			if( modem_wr_gprs_data(p_data,len)==len)
			{	
				#ifdef modem_send_net_data_print
				InfoPrintf("发送GPRS数据--%d--字节:",len);
				for(i=0;i<len;i++)InfoPrintf("%02X,",p_data[i]);
				InfoPrintf("发送完毕,,,等待SEND OK....\r\n");
				#endif
				
				p_msg=modem_pend_gprs_at_rsp_q(T_5S,&msg_size);
				if(p_msg != NULL)
	            {
	            	#ifdef modem_send_net_data_print
					InfoPrintf("等 SEDN OK 括号结果:%s\r\n",p_msg);
					#endif
				
	                if(strstr((char*)p_msg,"SEND OK") != NULL)
					{
						user_free(p_msg,__FUNCTION__);
						OSMutexPost(&modem_send_net_data_mutex,OS_OPT_POST_NONE,&os_err);
						ret_len=len;
						return ret_len;
					}
					else
					{
						InfoPrintf("wait SEDN OK fail:%s\r\n",p_msg);
						user_free(p_msg,__FUNCTION__);
						ret_len=0;
					}
	            }
				else
				{
					InfoPrintf("gprs send:wait SEND OK timeout\r\n");
					ret_len=0;
				}
			}
			else
			{
				InfoPrintf("write ringbuf fail!!!\r\n");
				ret_len=0;
			}
        }
		else
		{
			InfoPrintf("AT+CIPSEND,wait > ,but rx :");
			for(i=0;i<msg_size;i++)InfoPrintf("%c",p_msg[i]);InfoPrintf("\r\n");
			
			user_free(p_msg,__FUNCTION__);
			ret_len=0;
		}
	}
	else
	{
		InfoPrintf("AT+CIPSEND,wait > timeout\r\n");
		ret_len=0;
	}

	OSMutexPost(&modem_send_net_data_mutex,OS_OPT_POST_NONE,&os_err);
	modem_ReCreat_socket();
	return 0;	
}

int modem_send_sms(uint8_t *pbuf,uint32_t len,uint8_t *dest)
{
	return 0;
}


/*
	input:
			cmd		: general AT cmd
			rsp1	: expected response 1
			rsp2	: expected response 2
			time_out: max time of waiting rsp1 and rsp2 
	return:
			NULL	: operation failed
			others	: sucess,return the pointer of the buffer that contain rsp1 and rsp2
*/
//#define modem_send_cmd_wait_rsp_print
char* modem_send_cmd_wait_rsp(char *cmd,char*rsp1,char*rsp2,uint32_t time_out)
{
	#define RSP_MAX_LEN   80
	char  *p_msg=NULL,*p=NULL,*p_start_rsp2=NULL;
	uint16_t msg_size;
	uint32_t time_start,time_curr;
    OS_ERR os_err;
	uint16_t Remain_len;
	char *compare_buf=NULL;
	char get_rsp1=0;
	uint8_t i;

	static uint16_t fail_counter=0;

	compare_buf=user_malloc(RSP_MAX_LEN,__FUNCTION__);
	if(compare_buf!=NULL)
	{
		memset(compare_buf,0,RSP_MAX_LEN);
	}
	else
	{
		return NULL;
	}

	i=2;
	while(i--)
	{
		p_msg=modem_pend_send_at_rsp_q(2,&msg_size);
		if(p_msg != NULL)
		{
			InfoPrintf("有AT数据被丢弃-1-!!!\r\n");
			user_free(p_msg,__FUNCTION__);
		}
	}
	
	#ifdef modem_send_cmd_wait_rsp_print
	InfoPrintf("modem_send_cmd_wait_rsp:%s\n",cmd);
	#endif
	
	if(modem_tx_at_cmd((uint8_t*)cmd)!=0)
	{//uart tx buf full
		user_delay_ms(TIME_20ms);
		if(modem_tx_at_cmd((uint8_t*)cmd)!=0)
		{//uart tx buf full
			user_free(compare_buf,__FUNCTION__);
			return NULL;//uart tx failed
		}
	}
	
	//if(rsp2!=NULL)InfoPrintf("\r\n\r\n参数1:%s ,参数2:%s\r\n",rsp1,rsp2);
	Remain_len = 0;
	time_start =  OSTimeGet(&os_err);
    time_curr = time_start;
	while( (time_curr - time_start) < time_out)
	{
		p_msg=modem_pend_send_at_rsp_q(T_10MS,&msg_size);
		if(p_msg != NULL)
		{	
			if((Remain_len + msg_size) >= RSP_MAX_LEN)
			{
				msg_size = RSP_MAX_LEN- Remain_len -2; //-2为预留空间
			}

			memcpy(&compare_buf[Remain_len],p_msg,msg_size);
			Remain_len += msg_size;
			
			user_free(p_msg,__FUNCTION__);

			if(get_rsp1==0)
			{			
				p = strstr((const char*)compare_buf,rsp1);
				if(p != NULL)
				{
					#ifdef modem_send_cmd_wait_rsp_print
					InfoPrintf("--AT cmd-1-%s--响应成功 pbuf=%s",cmd,compare_buf);
					#endif
					get_rsp1=1;
					if(rsp2 == NULL)
					{
						fail_counter=0;
						return compare_buf;
					}
					else
					{
						p_start_rsp2=p+strlen(rsp1);
					}
				} 
			}
			else 
			{
				p = strstr((const char*)p_start_rsp2,rsp2);
				if(p != NULL)
				{
					#ifdef modem_send_cmd_wait_rsp_print
					InfoPrintf("--AT cmd-2-%s--响应成功 pbuf=%s",cmd,compare_buf);
					#endif
					fail_counter=0;
					return compare_buf;
				} 
			}
			
			p = strstr((const char*)compare_buf,"ERROR");
			if(p != NULL)
			{
				#if 1
				InfoPrintf("--AT cmd--%s--响应为:%s,return...\r\n",cmd,compare_buf);
				#endif
				user_free(compare_buf,__FUNCTION__);
				
				fail_counter++;
				if(fail_counter>100)
				{
					fail_counter=0;
					modem_power_down();
					user_delay_ms(1000);
					modem_power_up();
				}
				
				return NULL;
			} 
		}
		
		time_curr = OSTimeGet(&os_err);
	}
	
	#if 1
	InfoPrintf("--AT cmd--%s--响应为:超时\r\n\r\n",cmd);
	#endif
	
	user_free(compare_buf,__FUNCTION__);
	fail_counter++;
	if(fail_counter>100)
	{
		fail_counter=0;
		modem_power_down();
		user_delay_ms(1000);
		modem_power_up();
	}
	return NULL;
}

#define __different_gsm_module__config______________________________
/*
	return:
			0: failed
			1: successfully
*/
static int modem_check_mod_ready(void)
{
	//OS_ERR os_err; 	
	unsigned char buf[50]={0};
	int i=0;
    uint32_t len;
	//可设置固定的波特率,默认是自适应
	user_delay_ms(500*5+300);
	modem_tx_at_cmd("AT\r");
	while(1)
	{
		user_delay_ms(TIME_100ms);
		
		len = uart1_receive_data(buf,50);
		if(len != 0)
		{
			if(strstr((const char*)buf,"OK")!=NULL)
            {
            	InfoPrintf("modem boot ok !!!!\r\n");
				i=2;
				while(i--)
				{
					uart1_receive_data(buf,50);
					user_delay_ms(2);
				}
                return 1;
            }
		}

		if(i++%25==0)//2.5 second
		{	
			modem_tx_at_cmd("AT\r");	
		}
		
		if(i++>75)//7.5 second
		{
			InfoPrintf("modem boot time out\r\n");
			return 0;
		}
	}
}

static int modem_module_config(void)
{  
	char *p_msg;
	p_msg = modem_send_cmd_wait_rsp("ATE0\r","OK",NULL,T_1S);
	if(p_msg == NULL)
	{
		return -1;
	}
	user_free(p_msg,__FUNCTION__);

	p_msg = modem_send_cmd_wait_rsp("AT+CFUN=1\r","OK",NULL,T_1S);
	if(p_msg == NULL)
	{
		return -1;
	}
	user_free(p_msg,__FUNCTION__);

	#if 1
	p_msg = modem_send_cmd_wait_rsp("AT+CGMR\r","OK",NULL,T_1S);//软件版本号
	if(p_msg == NULL)
	{
		return -1;
	}
	user_free(p_msg,__FUNCTION__);

	p_msg = modem_send_cmd_wait_rsp("AT+CGSN\r","OK",NULL,T_1S);//模块序列号 ID
	if(p_msg == NULL)
	{
		return -1;
	}
	user_free(p_msg,__FUNCTION__);
	#endif
		
	p_msg = modem_send_cmd_wait_rsp("AT+EXUNSOL=\"SQ\",1\r","OK",NULL,T_1S);//主动上报SQ

	if(p_msg == NULL)
	{
		return -1;
	}
	user_free(p_msg,__FUNCTION__);
    return 0;
}



/*
 *****************************************************************************************
 *function name:modem_chk_reg_state
 * Description: 检查 网络注册状态
 * Arguments  : 
 *
 * Returns : 1，已经注册，0，没有注册
 *			
 *******************************************************************************************
 */

static int modem_chk_reg_state(void)
{
	char  *p_msg=NULL,*p=NULL;
	unsigned int tmp=0;

	p_msg=modem_send_cmd_wait_rsp("AT+CREG?\r","+CREG: ","\r\nOK\r\n",T_1S);	
	if(p_msg != NULL)
	{
		p = strstr((const char*)p_msg,"+CREG: 0,1");
		//InfoPrintf("%s",p_msg);
		user_free(p_msg,__FUNCTION__);
		if(p == NULL)
		{
			return -1;
		} 
	}

	p_msg=modem_send_cmd_wait_rsp("AT+CGATT?\r","+CGATT:","\r\nOK\r\n",T_1S);
	if(p_msg != NULL)
	{	
		//InfoPrintf("%s",p_msg);
		p = strstr((const char*)p_msg,"+CGATT: 1");
		user_free(p_msg,__FUNCTION__);
		if(p != NULL)
		{
			return 0;//OK	
		}
		else
		{
			return -1;	
		}
	}

	p_msg = modem_send_cmd_wait_rsp("AT+CSQ\r","+CSQ","\r\nOK\r\n",T_1S);  //查询信号强度
	if(p_msg != NULL)
	{	
		p = strstr((const char*)p_msg,"+CSQ");	
		if(p != NULL)
		{
			sscanf((char*)p,"%*[^:]:%u[^,]",&tmp);
			modem_ctl.sig_qty_2g = tmp;
		}  
		user_free(p_msg,__FUNCTION__);
	}
	
	return -1;
}

static int modem_check_link(void)
{
	char *p_msg;
    uint16_t msg_size;
	uint32_t time_start;//,time_curr;
	OS_ERR os_err;
	
	p_msg=modem_pend_send_at_rsp_q(1,&msg_size);
	if(p_msg != NULL)
	{
		user_free(p_msg,__FUNCTION__);
	}
	
    modem_tx_at_cmd("AT+CIFSR\r");
	time_start =  OSTimeGet(&os_err);
	
	while( (OSTimeGet(&os_err) - time_start) < T_4S)
	{
		p_msg=modem_pend_send_at_rsp_q(T_100MS,&msg_size);
		if(p_msg != NULL)
		{
			if(msg_size<5)
			{
				user_free(p_msg,__FUNCTION__);
				continue;
			}
			
			if(strstr((char const*)p_msg,"ERROR") != NULL)
			{
				InfoPrintf("get loc ip fail\r\n");
				user_free(p_msg,__FUNCTION__);
				return -1;
			}

			//net is opened
			InfoPrintf("get loc ip ok,%s\r\n",p_msg);
			user_free(p_msg,__FUNCTION__);
			return 0;
		}
	}
	InfoPrintf("get loc ip fail, timeout!!!\r\n");
	return -1;
}

static int modem_set_apn(void)
{
	char *p_msg;
    
	p_msg=modem_send_cmd_wait_rsp("AT+CSTT=\"CMIOT\"\r","OK",NULL,T_1S);
	if(p_msg != NULL)
	{	
		user_free(p_msg,__FUNCTION__);
		return 0;
	}

	return -1;
}

static int modem_set_add_iphead(void)
{
	char *p_msg;
	p_msg = modem_send_cmd_wait_rsp("AT+CIPHEAD=1\r","OK",NULL,T_1S);
	if(p_msg  != NULL)
	{
		user_free(p_msg,__FUNCTION__);
		return 0;
	}
	

	////+CIPCCFG: 5,2,1024,1,0,1460,50
	/*p_msg = modem_send_cmd_wait_rsp("AT+CIPCCFG=3,2,1024,1,0,1460,50\r","OK",T_1S);
	if(p_msg != NULL)
	{
		user_free(p_msg,__FUNCTION__);
		return 0;
	}
	*/
	return -1;
}
static int modem_shut_link(void)
{
	char *p_msg;
    
	p_msg =modem_send_cmd_wait_rsp("AT+CIPSHUT\r","SHUT OK",NULL,T_2S);
	if(p_msg != NULL)
	{
		user_free(p_msg,__FUNCTION__);
	}
	return 0;
}

static int modem_setup_link(void)
{
	char *p_msg;
	InfoPrintf("setup link,wait 86 S...\r\n");
	//p_msg=modem_send_cmd_wait_rsp("AT+CIICR\r","OK",NULL,T_1S*86);
	p_msg=modem_send_cmd_wait_rsp("AT+CIICR\r","OK",NULL,T_1S*6);
	if(p_msg != NULL)
	{
		user_free(p_msg,__FUNCTION__);
		return 0;
	}
	InfoPrintf("setup link fail\r\n");
	return -1;
}

static int modem_setup_connect(char *dn,uint16_t port)
{
	char at_cmd[100]={0};
	char  *p_msg=NULL;
	uint16_t msg_size;
	uint32_t time_start,time_curr;
	OS_ERR os_err;

	sprintf(at_cmd,"AT+CIPSTART=\"TCP\",\"%s\",%u\r",dn,port);
	
	//sprintf(at_cmd,"AT+CIPSTART=\"TCP\",\"202.105.139.90\",7015\r");//7016,7015
	//sprintf(at_cmd,"AT+CIPSTART=\"TCP\",\"120.55.166.5\",12398\r");
	//sprintf(at_cmd,"AT+CIPSTART=\"TCP\",\"120.55.166.5\",12398\r");

    InfoPrintf("modem_setup_connect...send at:%s\r\n",at_cmd);
	
	p_msg=modem_pend_send_at_rsp_q(1,&msg_size);
	if(p_msg != NULL)
	{
		user_free(p_msg,__FUNCTION__);
	}
	
	modem_tx_at_cmd((uint8_t*)at_cmd);
	
	time_start =  OSTimeGet(&os_err);
	time_curr = time_start;
	while( (time_curr - time_start) < T_6S)
	{
		p_msg=modem_pend_send_at_rsp_q(T_100MS,&msg_size);
		if(p_msg != NULL)
		{	
			if(strstr((const char*)p_msg,"CONNECT OK")!=NULL)
			{
				user_free(p_msg,__FUNCTION__);
				return 0;
			}
			else if(strstr((const char*)p_msg,"\r\nERROR\r\n")!=NULL)
			{
				//delay
				user_free(p_msg,__FUNCTION__);
				break;
			}
			else
			{
				user_free(p_msg,__FUNCTION__);
			}
		}
		time_curr = OSTimeGet(&os_err);
	}
	
	p_msg=modem_pend_send_at_rsp_q(T_100MS,&msg_size);
	if(p_msg != NULL)
	{	
		if(strstr((const char*)p_msg,"ALREADY CONNECT")!=NULL)
		{
			user_free(p_msg,__FUNCTION__);
			return 0;
		}
		else
		{
			user_free(p_msg,__FUNCTION__);
		}
	}

	return -1;
}

int modem_ReCreat_socket(void)
{
	modem_ctl.socket_link_state = 0;
    return 0;
}

void modem_check_SQ(void)
{
	char  *p_msg=NULL,*p=NULL;
	unsigned int tmp=0;

	p_msg = modem_send_cmd_wait_rsp("AT+CSQ\r","+CSQ","\r\nOK\r\n",T_1S);  //查询信号强度
	if(p_msg != NULL)
	{	
		p = strstr((const char*)p_msg,"+CSQ");	
		if(p != NULL)
		{
		  sscanf((char*)p,"%*[^:]:%u[^,]",&tmp);
		  modem_ctl.sig_qty_2g = tmp;
		  InfoPrintf("-----------------------------------------------csq=%d\r\n",tmp);
		}  

		user_free(p_msg,__FUNCTION__);
	}
}

unsigned char modem_get_sig_val(void)
{
	return modem_ctl.sig_qty_2g;

}
/*
	return : 
			1:get ok
			0:fail                             */
int modem_get_ccid(void)
{
	char *p_msg;
	int ret=0;
	
	 /*
	AT+CCID

	89860040191572516655

	OK

	*/
	p_msg = modem_send_cmd_wait_rsp("AT+CCID\r","OK",NULL,T_2S);
	if(p_msg != NULL)
	{
		if((p_msg[2]>=0x30)&&(p_msg[2]<=0x39))
		{//正确的CCID号
			InfoPrintf("GET CCID OK:%s",p_msg);
			ret=1;
		}
		else
		{
			ret=0;
		}
		user_free(p_msg,__FUNCTION__);
	}

	return ret;
}

static void modem_manage_init(void)
{
	net_state_init();
}

void modem_led_control(void*p_Tmr,void*p_Arg)
{
 	OS_ERR os_err;
  	static uint32_t modem_led_t_wait=0;

  	if(get_modem_err_state( )) //=1 mean err 0 mean ok if(ret==1)
	{
		modem_ctl.sig_qty_2g=0x00; //
		modem_led_OnOff(0);
	}
	else
	{
		if(get_already_login_flag()==1)
		{
			modem_led_OnOff(1);
		}
		else
		{
			if(OSTimeGet(&os_err)- modem_led_t_wait > T_1S)
			{
				modem_led_t_wait=OSTimeGet(&os_err);
				modem_led_OnOff(2);
			}
		}
	}
}

/*
	input:
			sta: 
				0: socket closed
				1: socket setup ok
	brief:
	     	post socket state to high layer,
*/
static int modem_post_socket_state_to_app(uint8_t sta)
{
	OS_ERR os_err=0Xffffffff;
	TASK_MSG_T *ptr;
	ptr=(TASK_MSG_T *)user_malloc(sizeof(TASK_MSG_T),__FUNCTION__);
	if(ptr!=NULL)
	{
		ptr->msg_src_id=MODEM_MANAGE_TASK_ID;
		ptr->msg_father_type=FATHER_MSG_T_MODEM_SOCKET_STATE;
		ptr->msg_son_type=sta;
		
		OSTaskQPost(&telecom_prot_manage_task_Tcb,(void*)ptr,sizeof(TASK_MSG_T), OS_OPT_POST_FIFO,&os_err);
		if(os_err!=0)
		{
			InfoPrintf("modem post net state to report task faild -%d-\r\n",os_err);
			user_free(ptr,__FUNCTION__);
		}
	}
	return os_err;
}

void modem_urc_handle(void * p_msg)
{
    //OS_ERR os_err;
	char *p = NULL;
	unsigned int tmp=0;
    //InfoPrintf("modem_urc_handle,%s\r\n",in_buf);
	if(strstr((char const*)p_msg,"\r\n+RING")!= NULL)
	{
		//modem_ctl.state = MODEM_STATE_PHONE;
	}
	else if(strstr((char const*)p_msg,"\r\nCLOSED")!= NULL)
	{
		modem_ReCreat_socket();
	}
	else if(strstr((char const*)p_msg,"+PDP: DEACT")!= NULL)
	{
		modem_ctl.tcp_link_state = 0;
	}
	else if(strstr((char const*)p_msg,"Call Ready")!= NULL)
	{ 
	}
	else if(strstr((char const*)p_msg,"SMS Ready")!= NULL)
	{ 
	}
	else if((p=strstr((char const*)p_msg,"+CSQN:"))!= NULL)
	{ 
		sscanf((char*)p,"%*[^:]:%u[^,]",&tmp);
		modem_ctl.sig_qty_2g = tmp;
		InfoPrintf("modem sig:%u\r\n",modem_ctl.sig_qty_2g);
	} 
	
	user_free(p_msg,__FUNCTION__);
}

/*
	input:
			op_cls_network:
						 	1: open network,setup socket connect
						 	0: quit socket,set modem module idle.
*/
void  modem_internet_control(uint8_t op_cls_network)
{
 	OS_ERR os_err;
	static uint32_t last_time;
	
	static enum MODEM_STATE setup_socket_step=MODEM_STATE_INIT;

	if(setup_socket_step==MODEM_STATE_OFF)
	{
		setup_socket_step=MODEM_STATE_INIT;
	}
	
	if(op_cls_network==1)
	{//setup socket
	
		switch(setup_socket_step) 
		{        			
			case MODEM_STATE_RESET:
				modem_power_down();
				user_delay_ms(1000);
				modem_power_up();
				setup_socket_step = MODEM_STATE_INIT;
				last_time = OSTimeGet(&os_err);
				break;
	    	case MODEM_STATE_INIT:
				net_state_init();
				modem_post_socket_state_to_app(0);
				last_time = OSTimeGet(&os_err);
	            setup_socket_step = MODEM_STATE_INIT_WAIT;
			case MODEM_STATE_INIT_WAIT:
				if( modem_module_config() == 0 ) //3秒
	            {	// GSM模块初始化
					InfoPrintf("-1-modem module init ok\r\n");
					setup_socket_step = MODEM_STATE_CHK_CREG;
					last_time = OSTimeGet(&os_err);
				}
				else
				{
					//InfoPrintf("modem module init fail\r\n");
					if(OSTimeGet(&os_err) - last_time > T_1MIN)
						setup_socket_step = MODEM_STATE_RESET;
				}
	         	break;
	    	case MODEM_STATE_CHK_CREG:	
				if(modem_chk_reg_state()==0)//CGATT //3秒
				{
					InfoPrintf("-2-注册到基站 ok\r\n");
					setup_socket_step = MODEM_STATE_SHUT_LINK;
				}
				else
				{
					if(OSTimeGet(&os_err) - last_time > T_2MIN)
						setup_socket_step = MODEM_STATE_RESET;
					user_delay_ms(TIME_10ms);
				} 
                
				{
					static unsigned char ccid__flag=0;
					if(ccid__flag==0)
					{
						ccid__flag=modem_get_ccid();
					}
				}
	       		break;
			case MODEM_STATE_SHUT_LINK:
				net_state_init();
				modem_shut_link();
				setup_socket_step = MODEM_STATE_SET_APN;
				break;
			case MODEM_STATE_SET_APN:
				if(!modem_set_apn())
				{
					InfoPrintf("-3-连接APN ok\r\n");
					setup_socket_step = MODEM_STATE_NET_OPEN;
				}
				else
				{
					setup_socket_step = MODEM_STATE_INIT;
				}
				break;
	        case MODEM_STATE_NET_OPEN:
				if(!modem_setup_link())//setup gprs   //6秒
				{
					setup_socket_step = MODEM_STATE_CHK_NET;
					InfoPrintf("-4-连接internet ok\r\n");
				}
				else
				{
					setup_socket_step = MODEM_STATE_SHUT_LINK;
				}
	            break;		
			case MODEM_STATE_CHK_NET://get ip
				if(modem_check_link()==0) 		//4秒
				{
					InfoPrintf("-5-get ip ok\r\n");
					modem_ctl.tcp_link_state = 1;
					setup_socket_step = MODEM_STATE_SOCKET_CREATE;
				}
				else
				{
					modem_ctl.tcp_link_state = 0;
					setup_socket_step = MODEM_STATE_SHUT_LINK;
				}
				break;
			case MODEM_STATE_SOCKET_CREATE://connect  
				if(modem_setup_connect(get_remote_dn(),get_remote_port())==0) //6秒
				{
					InfoPrintf("-6-socket created ok\r\n");
					modem_ctl.socket_link_state = 1;
					modem_set_add_iphead();
					modem_post_socket_state_to_app(1);
					setup_socket_step = MODEM_STATE_GPRS;
				}
				else
				{
					modem_ctl.socket_link_state = 0;
					setup_socket_step = MODEM_STATE_CHK_NET;
				}
	            break;	
	   		case MODEM_STATE_GPRS:
				if((modem_ctl.socket_link_state==1)&& (modem_ctl.tcp_link_state==1))
				{
					//check tcp link and socket state
					//...
				}	
				else
				{
					setup_socket_step = MODEM_STATE_SHUT_LINK;
				}
	        	break;
			case MODEM_EXIT_GPRS:
				modem_setup_link();
				setup_socket_step = MODEM_STATE_SHUT_LINK;
			case MODEM_STATE_SLEEP:
				break;
	     	default:
	      		InfoPrintf("error setup_socket_step!!-%d-\r\n",setup_socket_step);	
	       		break;			
	 	}
	}
	else
	{//quit socket
	
		setup_socket_step=MODEM_STATE_OFF;
		modem_shut_link();
	}
}

void modem_manage_task_msg_handle(TASK_MSG_T *p_msg)
{
	//OS_ERR os_err;
	
	switch (p_msg->msg_src_id)
	{
		case DEV_MANAGE_TASK_ID:
			switch (p_msg->msg_father_type)
			{
				case FATHER_MSG_T_TASK_POWER_CTRL:
					switch (p_msg->msg_son_type)
					{
						case POWER_CTRL_PWR_ON:
							InfoPrintf("modem rx ctl msg:pwr on\r\n");					
							modem_open_module();
							user_delay_ms(TIME_10ms);
							InfoPrintf("===========================\r\n");
							if(modem_check_mod_ready()!=0)//max wait time 11S
							{
								InfoPrintf("power up:2g module power up ok\r\n");
							}
							else
							{
								InfoPrintf("power up:2g module power up failed !!!\r\n");
							}
							
							modem_ctl.rx_gprs_state=0;
							modem_ctl.socket_ctl = SOCKET_CTL_SETUP_AND_KEEP;
							break;

						case POWER_CTRL_SLEEP:
							InfoPrintf("modem rx ctl msg: sleep\r\n");
							break;

						case POWER_CTRL_WAKEUP:
							InfoPrintf("modem rx ctl msg: wakeup\r\n");
							break;

						case POWER_CTRL_PWR_OFF:
							InfoPrintf("modem rx ctl msg: power off\r\n");
							break;
							
						default:
							break;
					}
					break;
					
				default:
					break;
			}
			break;

		default:
			break;		
	}
}

/*
 *****************************************************************************************
 *function name:  modem_receive_task
 * Description: 接收模块的底层数据，对不同的数据进行分发到不同的任务
 * Arguments  : 读取模块的串口数据
 *
 * Returns : 
 *			
 *******************************************************************************************
 */
void modem_receive_task(void*p_data)
{
	OS_ERR os_err; 
	uint32_t last_time=0;
	p_data = p_data;

	OSQCreate(&modem_receive_urc_q,"modem_receive_urc_q",10,&os_err);
	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf("create modem_receive_urc q fail\r\n");
	}

	OSQCreate(&modem_send_at_rsp_q,"modem_send_at_rsp_q",2,&os_err);
	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf("create modem_send_at_rsp q fail\r\n");
	}

	OSQCreate(&modem_send_gprs_at_rsp_q,"modem_send_gprs_at_rsp_q",1,&os_err);
	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf("create modem_send_gprs_at_rsp q fail\r\n");
	}

	OSQCreate(&modem_receive_gprs_data_q,"rx_gprs_data q",5,&os_err);
	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf("create modem_recv_remote_data q fail\r\n");
	}

	//InfoPrintf("modem_receive_task() start ok\r\n");
	while(1)
	{	
		if(modem_ctl.socket_ctl != SOCKET_CTL_CLOSED)
		{
			at_pro_2g_module_data(&modem_ctl.rx_gprs_state,&modem_ctl.waiting_cipsend_ack);
		}
		else
		{
			//InfoPrintf("socket_ctl==SOCKET_CTL_CLOSED ...\r\n");
		}
		user_delay_ms(TIME_10ms);
		if(user_get_os_time()-last_time>T_1S)
		{
			last_time=user_get_os_time();
			InfoPrintf("modem_receive_task loop...\r\n");
		}
	}
}


/*
 *****************************************************************************************
 *function name:  modem_manage_task
 * Description: 维护模块的状态切换，保持 IP，查询信号强度，发送gprs数据。开关机，休眠管理
 * Arguments  : 接收消息，根据不同消息执行不同功能。
 *
 * Returns : 
 *			
 *******************************************************************************************
 */
//void modem_manage_task(void*p_data)
void ModemManageTask(void*p_data)
{
    OS_ERR os_err;
	TASK_MSG_T* task_msg;
	uint8_t *urc_msg; 
	uint16_t msg_size;
	uint32_t last_time=0;
	//uint8_t ret=0;
	
	p_data = p_data;
	
	modem_manage_init();
	modem_power_down();
	
	if(modem_send_net_data_mutex.Type != OS_OBJ_TYPE_MUTEX)
	{
		OSMutexCreate(&modem_send_net_data_mutex,"modem send net data mutex",&os_err);
		if(os_err!=OS_ERR_NONE)
		{
			InfoPrintf("creat modem_send_net_data_mutex failed !!!\r\n");
		}
	}

	if(modem_uart_tx_ringbuf_mutex.Type != OS_OBJ_TYPE_MUTEX)
	{
		OSMutexCreate(&modem_uart_tx_ringbuf_mutex,"modem_uart_tx_ringbuf_mutex",&os_err);
		if(os_err!=OS_ERR_NONE)
		{
			InfoPrintf("creat modem_uart_tx_ringbuf_mutex failed !!!\r\n");
		}
	}
	
	#if 1
	OSTaskCreate(&modem_receive_task_Tcb,
				"modem_receive_task",
				(OS_TASK_PTR)    modem_receive_task,
				(void          *)NULL,
				 MODEM_RECV_TASK_PRIO,
				(CPU_STK 	*)  modem_receive_task_Stk,
				(CPU_STK_SIZE)  MODEM_RECEIV_TASK_STK_SIZE/10,/* limit*/
				(CPU_STK_SIZE)  MODEM_RECEIV_TASK_STK_SIZE,
				(OS_MSG_QTY)    MODEM_RECV_TASK_MSG_QTY,
				(OS_TICK )      MODEM_RECV_TASK_TIME_QUANTA,
				(void  *)       NULL,
				(OS_OPT)        (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				&os_err);	

	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf("creat modem_receive_task Tcb failed !!!\r\n");
	}
	#endif

    OSTmrCreate(&modem_led_control_tmr,"modem_led_control_tmr",T_10MS,T_10MS,OS_OPT_TMR_PERIODIC,modem_led_control,NULL,&os_err);
	OSTmrStart(&modem_led_control_tmr,&os_err);

	while(1)
	{
		urc_msg = modem_pend_receive_urc_q(T_10MS*2,&msg_size);
		if(urc_msg != NULL)
		{
			modem_urc_handle(urc_msg);
		}
		else if(os_err != OS_ERR_TIMEOUT)
		{
			user_delay_ms(TIME_10ms);
		}
		
		task_msg = OSTaskQPend(0,OS_OPT_PEND_NON_BLOCKING,&msg_size,NULL,&os_err);
		if(NULL != task_msg)
		{
			modem_manage_task_msg_handle(task_msg);
			
			user_free(task_msg,__FUNCTION__);
		}
		//socket control ----------------------------------------------------
		if(modem_ctl.socket_ctl == SOCKET_CTL_CLOSED)
		{
			//do nothing		
		}
		else if(modem_ctl.socket_ctl == SOCKET_CTL_SETUP_AND_KEEP)
		{
			modem_internet_control(1);// max 6 second
		}
		else if(modem_ctl.socket_ctl == SOCKET_CTL_QUIT)
		{
			modem_internet_control(0);
			modem_ctl.socket_ctl = SOCKET_CTL_CLOSED;
		}

		//fault detect ----------------------------------------------------

		//InfoPrintf(".");
		OSTimeDly(T_10MS,OS_OPT_TIME_DLY,&os_err);
		if(user_get_os_time()-last_time>T_1S)
		{
			last_time=user_get_os_time();
			InfoPrintf("ModemManageTask loop...\r\n");
		}
	}
}

#endif

