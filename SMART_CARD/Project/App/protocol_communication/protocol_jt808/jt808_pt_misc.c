#include "user_system_cfg.h"

#ifdef USE_PROTOCOL_JT808

#include <stdint.h>
#include <string.h>
#include <math.h>

#include "os.h"
#include "os_cfg_app.h"

#include "user_debug_app.h"

#include "jt808_pt_misc.h"
#include "jt808_protocol_tx.h"

extern 	int post_report_result(uint16_t serial,uint16_t cmd_id,uint8_t result);
#ifdef MOBI_IP
uint8_t authorize_code[13]="123456789088\0";
#else
uint8_t authorize_code[49]="52EAD799A38944C081606191D538DB7B0001215A3782096-\0";
#endif

uint8_t register_ok_flag=0;

uint8_t get_register_ok_flag(void)
{
	return register_ok_flag;
}

uint8_t *get_authorize_code(void)
{
	return authorize_code;
}


void jt808_com_ack_pro(uint8_t *buf,uint16_t len)
{
	uint16_t ack_sn;
	uint16_t ack_id;
	uint8_t  result;
	
	ack_sn= *(uint16_t *)(buf);
	ack_sn=high_low_switch_16(ack_sn);
	
	ack_id= *(uint16_t *)(buf+2);
	ack_id=high_low_switch_16(ack_id);
	
	result= *(buf+4);
	
	InfoPrintf("应答流水号:%04X--",ack_sn);
	InfoPrintf("应答ID:%04X--",ack_id);
	InfoPrintf("应答结果:%d\r\n",result);

	post_report_result(ack_sn,ack_id,result);
}



/*void jt808_ack_reg_pro(uint8_t *buf,uint16_t len)
{
	uint16_t ack_sn;
	uint8_t  result;
	int i=0;
	uint8_t authorize_len;
	
	ack_sn= *(uint16_t *)(buf);
	ack_sn=high_low_switch_16(ack_sn);
	
	result= *(buf+2);
	
	InfoPrintf("应答流水号:%04X\r\n",ack_sn);
	InfoPrintf("应答结果:%d\r\n",result);
	
	authorize_len=len-3;
	
	memcpy(authorize_code,&buf[i+3],48);

	
	#if 1
	InfoPrintf("鉴权码 char (%d):",authorize_len);
	for(i=0;i<authorize_len;i++)
	{
		InfoPrintf("%c",authorize_code[i]);
	}
	InfoPrintf("\r\n");
		
	InfoPrintf("鉴权码 HEX(%d):",authorize_len);
	for(i=0;i<authorize_len;i++)
	{
		InfoPrintf("%02X",authorize_code[i]);
	}
	InfoPrintf("\r\n");
	#endif

	
	if(authorize_len>0)
	{
		register_ok_flag=1;
	}

	post_report_result(ack_sn,TM_REGISTER_ID,result);
}*/

void jt808_ack_reg_pro(uint8_t *buf,uint16_t len)
{
	uint16_t ack_sn;
	uint8_t  result;
	int i=0;
	uint8_t authorize_len;
	
	ack_sn= *(uint16_t *)(buf);
	ack_sn=high_low_switch_16(ack_sn);
	
	result= *(buf+2);
	
	InfoPrintf("应答流水号:%04X\r\n",ack_sn);
	InfoPrintf("应答结果:%d\r\n",result);
	
	authorize_len=len-3;
	if(authorize_len>0)
	{
		register_ok_flag=1;
		memset(authorize_code,0,48);
		memcpy(authorize_code,&buf[i+3],48);
	}

	#if 1
	InfoPrintf("鉴权码 char (%d):",authorize_len);
	for(i=0;i<authorize_len;i++)
	{
		InfoPrintf("%c",authorize_code[i]);
	}
	InfoPrintf("\r\n");
		
	InfoPrintf("鉴权码 HEX(%d):",authorize_len);
	for(i=0;i<authorize_len;i++)
	{
		InfoPrintf("%02X",authorize_code[i]);
	}
	InfoPrintf("\r\n");
	#endif
	

	post_report_result(ack_sn,TM_REGISTER_ID,result);
}




















#endif


