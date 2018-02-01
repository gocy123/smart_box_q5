#include "user_system_cfg.h"

#ifdef  USE_PROTOCOL_JT808
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "os.h"
#include "os_cfg_app.h"	//TCB,STACK,PRIORITY,任务间消息的结构体定义和宏
#include "heads.h"


//#include "aes.h"
//#include "rsa_main.h"
#ifdef USE_SIM800_MODEM
#include "modem_sim800_app.h"
#endif
#ifdef USE_SIM6320_MODEM
#include "modem_sim6320_app.h"
#endif

#ifdef USE_MC20_MODEM
#include "modem_MC20_app.h"
#include "modem_MC20_AT_pro.h"
#endif


#include "user_debug_app.h"
#include "jt808_telecom_prot_manage.h"
//#include "user_gps_app.h"
//#include "user_rtc_app.h"



#include "jt808_protocol_tx.h"
#include "jt808_protocol_rx.h"
#include "jt808_pt_misc.h"

#ifndef ONENET_IP

#if (CAR_LICENSE_NUM_LEN==6)
uint8_t card[CAR_LICENSE_NUM_LEN]="zgcdef";
#else
uint8_t card[CAR_LICENSE_NUM_LEN]="粤B24680";
#endif

#endif

#define PD_REP_INFO_DATA_Q_MAX (6)
#define PD_REP_INFO_DATA_SAVE_Q_MAX (3)
OS_Q pd_rep_info_data_Q={0};
OS_Q pd_rep_info_data_save_Q={0};

OS_TMR 	period_report_tmr;

extern OS_Q 	pd_rep_info_data_Q;
extern OS_Q 	pd_rep_info_data_save_Q;
extern uint8_t 	aes_key_public[];
extern uint8_t	get_already_login_flag(void);


int get_report_period(void)
{
	//return 120;//N*100ms ,基本单位是100ms  zgc ,但滴答是1ms  ????
	return 20;//N*100ms ,基本单位是100ms  zgc ,但滴答是1ms  ????
}

int get_gprs_send_fun(void)
{
	return 1;
	//return gprs_enable;
}


#define ____function_start____________________________________________________________

//uint16_t 高低字节转换
unsigned short htons(unsigned short n)
{
  return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

/************************************************************
函数：get_check_sum ，计算异或校验和
输入:*p_data 需要计算校验和的源数据，len源数据字节数
输出:
返回值:异或校验和
作者:liaoxc 2015-11-10
***************************************************************/
uint8_t get_check_sum(uint8_t *p_data,uint16_t len)
{
	uint8_t chk_sum;

	chk_sum = *p_data++;
	len--;
	
	while(len--)
	{
		chk_sum ^=  *p_data++;
	}

	return chk_sum;
}

#define ____flow_serial_number___________________________________

__no_init uint16_t jt808_serial_num;

/************************************************************
函数：get_serial_num ，获取上传数据的流水号
输入:
输出:
返回值:协议流水号
作者:liaoxc 2015-11-16
***************************************************************/
uint16_t get_serial_num(void)
{
	uint16_t tmp;
	//InfoPrintf("流水号=%d\r\n",sgmw_serial_num);
	tmp = jt808_serial_num;
	jt808_serial_num++;
	
	return tmp;
}

uint8_t *get_phone_num(void)
{
	//这个就是EXLIVE的设备ID号,EXLIVE APP 添加设备那里对应的"设备ID"
	
	//APP添加设备界面的"SIM卡号"无意义(对终端代码无任何关联作用),
	//只是在APP的地图界面显示而已
	//static uint8_t phone_num[6]={0x02,0x88,0x20,0x16,0x08,0x05};
	//static uint8_t phone_num[6]={0x03,0x88,0x20,0x16,0x12,0x31}; 
	static uint8_t phone_num[6]={0x12,0x34,0x56,0x78,0x90,0x88}; //mobi
	//static uint8_t phone_num[6]={0x01,0x87,0x63,0x49,0x07,0x65};
	return phone_num;
}


/************************************************************
函数：reset_serial_num ，获取上传数据的流水号
输入:
输出:
返回值:协议流水号
作者:liaoxc 2015-11-16
***************************************************************/
void reset_serial_num(void)
{
	jt808_serial_num = 1;
}

#define ____escape__DeEscape____________________________________________
/************************************************************
函数：check_escape_bytes_amount ，计算需要转义的数据个数
输入:*p需要的源数据，len源数据字节数
输出:
返回值:需要转义的字节数
作者:zgc 2015-11-10
***************************************************************/
int check_escape_bytes_amount(uint8_t *p,int len)
{
	int i,j=0;
	for(i=0;i<len;i++)
	{
		if(*p == JT808_PROTOCL_ESCAPE_CODE || *p == JT808_PROTOCOL_FIX)
		{
			j++;
		}
		p++;
	}

	return j;
}

/************************************************************
转义处理函数ｅ：把0x7E转成0x7D 0x02，0x7E转成0x7D 0x01
输入:*Src 未转义带头尾的协议包数据
输出:*Dst 转义后的带头尾的协议包数据
返回值:转义后的数据长度
作者:liaoxc 2015-11-10
***************************************************************/
#if 1
uint16_t jt808_escape_package(uint8_t*Dst,uint16_t DstLen,uint8_t *Src,uint16_t SrcLen)
{
	uint16_t len=0;
	uint16_t i=0;

//  7e xx xx xx xx xx xx.... 7e
	
	*Dst++ = *Src++; //head
	len++;
	
	for(i=1; i<SrcLen-1; i++)//从第二个不是head的字符开始
	{
		if(len < (DstLen-1))
		{
			if(*Src == JT808_PROTOCOL_FIX)
			{
				
				*Dst++ = JT808_PROTOCL_ESCAPE_CODE;
				*Dst++ = 0x02;
				Src++;
				len += 2;
			}
			else if(*Src == JT808_PROTOCL_ESCAPE_CODE)
			{
				
				*Dst++ = JT808_PROTOCL_ESCAPE_CODE;
				*Dst++ = 0x01;
				Src++;
				len += 2;
			}
			else
			{	
				*Dst++ = *Src++;
				len++;
			}
		}
		else
		{
			return 0;
		}
	}
	
	*Dst = *Src;//tail
	len ++;

	return len;
}
#else
uint16_t jt808_escape_package(uint8_t*Dst,uint16_t DstLen,uint8_t *Src,uint16_t SrcLen)
{
	uint16_t len=1;
	uint16_t i=0;
	
	*Dst++ = *Src++; //head
	for(i=SrcLen;i > 2;i--)//从第二个不是head的字符开始
	{
		if(len > (DstLen-1)-1)
		{
			//return 0;
		}
	
		if(*Src == JT808_PROTOCOL_FIX)
		{
			
			*Dst++ = JT808_PROTOCL_ESCAPE_CODE;
			*Dst++ = 0x02;
			Src++;
			len += 2;
		}
		else if(*Src == JT808_PROTOCL_ESCAPE_CODE)
		{
			
			*Dst++ = JT808_PROTOCL_ESCAPE_CODE;
			*Dst++ = 0x01;
			Src++;
			len += 2;
		}
		else
		{	
			*Dst++ = *Src++;
			len++;
		}
	}
	
	*Dst = *Src;//tail
	len ++;

	return len;
}

#endif

/*
反转义处理函数ｅ：把0x7D 0x01转成0x7D，0x7D 0x02转成0x7E
输入:*Src 未转义带头尾的协议包数据
输出:*Dst 转义后的带头尾的协议包数据
返回值:反转义后的数据长度
作者:liaoxc 2015-11-10 */
uint16_t jt808_DeEscape_package(uint8_t*Dst,uint8_t *Src,uint16_t SrcLen)
{
	uint16_t len=1;
	uint16_t i=0;
	
	*Dst++ = *Src++;//head
	i=2;
	while(i<SrcLen)//从第二个不是head的字符开始
	{
		if(*Src != JT808_PROTOCL_ESCAPE_CODE)
		{
			*Dst++ = *Src++;
			i++;
		}
		else
		{
			if(*(Src+1) == 0x02)
			{
				*Dst = JT808_PROTOCOL_FIX;
			}
			else if(*(Src+1) == 0x01)
			{
				*Dst = *Src;
			}
			else
			{
				*Dst++ = *Src;
				*Dst = *(Src+1);

			}
			Src += 2;
			i += 2;
			Dst++;
			
		}			
		len++;
	}
	
	*Dst = *Src;//tail
	len ++;
	
	return len;
}


#define ____wr_air_protocol_frame__________________________________________________

static uint8_t *jt808_pack_gps_info_area(MSG_DATE_TIME_T msg_time ,uint16_t *info_data_len)
{
	uint16_t malloc_len=0;
	GPS_INFO_REP_T *malloc_jt808_gps_info_data=NULL;	//信息体RAM区
	//uint8_t *ptr;
	gps_location_info_T gps_location_data={0};
	uint8_t i;
    //uint8_t j;
	//BEIJING_TIME_T current_time={0};
	//int ret;
	//uint8_t temp_len;
	uint32_t temp_32;

	float f_lati;
	float f_longi;

	uint16_t unint_lati;
	uint16_t unint_longi;

	
	uint16_t unint_lati_du;
	uint16_t unint_longi_du;
	
	float    float_lati_fen;
	float    float_longi_fen;
	
	float 	 float_lati_miao;
	float    float_longi_miao;

	uint32_t u32_lati;
	uint32_t u32_longi;
	
	malloc_len=sizeof(GPS_INFO_REP_T);
	*info_data_len=malloc_len;
	malloc_jt808_gps_info_data=(GPS_INFO_REP_T *)user_malloc(malloc_len,__FUNCTION__);
	if(malloc_jt808_gps_info_data==NULL)
	{
		return NULL;
	}

	//alarm DWORD
	malloc_jt808_gps_info_data->alarm_flags=0;

	//status DWORD
	malloc_jt808_gps_info_data->status.acc_sta=1;//缺函数
	malloc_jt808_gps_info_data->status.gps_location_valid=gps_check_lacation_state();
	malloc_jt808_gps_info_data->status.latitude_type=gps_get_south_north();//1:south lati,0:north lati
	malloc_jt808_gps_info_data->status.longitude_type=gps_get_east_west();//1:west longi,0:east longi
	malloc_jt808_gps_info_data->status.bussiness_operate_sta=1;//1:verhicle is in bussiness operate sta,0:is in idle sta,
	malloc_jt808_gps_info_data->status.lati_longi_encrypt_sta=0;//1:encrypt,0:unenc

	malloc_jt808_gps_info_data->status.loading_level=3;//0:empty verhi,1:half loading,2:reserve,3:full loading level
	malloc_jt808_gps_info_data->status.oil_way_sta=1;//1:normal,0: oil-way broken(disconnect)
	malloc_jt808_gps_info_data->status.electrical_circuit_sta=1;//1:verhicle electrical circuit ok,0:circuit broken(disconnect)
	malloc_jt808_gps_info_data->status.door_lock_sta=0;//1:door locked ,0:door unlocked
	malloc_jt808_gps_info_data->status.front_door_open_sta=0;//1:open ,0:closed
	malloc_jt808_gps_info_data->status.middle_door_open_sta=0;//1:open ,0:closed
	malloc_jt808_gps_info_data->status.back_door_open_sta=0;//1:open ,0:closed

	malloc_jt808_gps_info_data->status.driver_door_open_sta=0;//1:open ,0:closed
	malloc_jt808_gps_info_data->status.undefine_door_open_sta=0;//1:open ,0:closed
	malloc_jt808_gps_info_data->status.use_gps_to_locate=1;//1:use,0:no use
	malloc_jt808_gps_info_data->status.use_beidou_to_locate=0;//1:use,0:no use
	malloc_jt808_gps_info_data->status.use_glonass_to_locate=0;//1:use,0:no use
	malloc_jt808_gps_info_data->status.use_galileo_to_locate=0;//1:use,0:no use

	if(malloc_jt808_gps_info_data->status.gps_location_valid==1)
	{	
		gps_get_location_info(&gps_location_data);
		f_lati=atof((char const *)(gps_location_data.latitude));
		f_longi=atof((char const *)(gps_location_data.longitude));
		
		unint_lati=(uint16_t)f_lati;
		unint_longi=(uint16_t)f_longi;
		
		//InfoPrintf("f_lati=%d\r\n",f_lati);
		//InfoPrintf("f_longi=%d\r\n",f_longi);

		//InfoPrintf("unint_lati=%d\r\n",unint_lati);
		//InfoPrintf("unint_longi=%d\r\n",unint_longi);

		unint_lati_du=unint_lati/100;
		unint_longi_du=unint_longi/100;
		
		float_lati_fen=unint_lati%100;
		float_longi_fen=unint_longi%100;	
		
		f_lati-=unint_lati;
		f_longi-=unint_longi;

		float_lati_miao=f_lati*60;
		float_longi_miao=f_longi*60;

		f_lati= float_lati_fen/60 + float_lati_miao/3600;
		f_longi= float_longi_fen/60 + float_longi_miao/3600;

		

		f_lati*=1000000;
		f_longi*=1000000;

		u32_lati=(uint32_t)f_lati+unint_lati_du*1000000;
		u32_longi=(uint32_t)f_longi+unint_longi_du*1000000;
		
		//InfoPrintf("%d%d%d%d%\r\n",f_lati,f_longi,unint_lati,unint_longi);
		//InfoPrintf("%d%d%d%d%\r\n",unint_lati_du,unint_lati_fen,unint_longi_fen,unint_longi_fen);

		//pack latitude
		malloc_jt808_gps_info_data->latitude=u32_lati;			

		//pack longitude
		malloc_jt808_gps_info_data->longitude=u32_longi;			

		//pack altitude
		for(i=0;i<GPS_ALTITUDELEN;i++)
		{
			if(gps_location_data.altitude[i]=='.')
			{
				gps_location_data.altitude[i]='\0';
				break;
			}
		}
		malloc_jt808_gps_info_data->altitude=atoi((char const *)(gps_location_data.altitude));	

		//pack gps speed
		for(i=1;i<=3;i++)//GPS_SPEEDLEN
		{
			if(gps_location_data.speed[i]=='.')
			{
				break;
			}
		}
		
		if(i<=3)
		{
			memmove(gps_location_data.speed+i,(gps_location_data.speed+i+1),(5-(i+1)));//GPS_SPEEDLEN
			gps_location_data.speed[4]='\0';
			
			temp_32=atoi((char const *)(gps_location_data.speed));
			temp_32=temp_32*1852; // km/h (放大1000倍)		
			if(i==1)
			{
				temp_32/=100000;
			}
			else if(i==2)
			{
				temp_32/=10000;
			}
			else if(i==3)
			{
				temp_32/=1000;
			}
			malloc_jt808_gps_info_data->speed = (uint16_t)temp_32;	
		}
		else
		{
			//GPS module output err string
		}

		//pack gps direction
		malloc_jt808_gps_info_data->direction=atoi((char const *)(gps_location_data.speed_direction));

		//pack date_time
		//ret=get_current_time(&current_time);
		
		malloc_jt808_gps_info_data->date_time[0]=((((uint8_t)(msg_time.year&0x3F)))/10)<<4;
		malloc_jt808_gps_info_data->date_time[0]|=(((uint8_t)(msg_time.year&0x3F)))%10;

		malloc_jt808_gps_info_data->date_time[1]=((((uint8_t)(msg_time.mon&0x0F)))/10)<<4;
		malloc_jt808_gps_info_data->date_time[1]|=(((uint8_t)(msg_time.mon&0x0F)))%10;

		malloc_jt808_gps_info_data->date_time[2]=((((uint8_t)(msg_time.day&0x1F)))/10)<<4;
		malloc_jt808_gps_info_data->date_time[2]|=((uint8_t)(msg_time.day&0x1F))%10;

		malloc_jt808_gps_info_data->date_time[3]=((((uint8_t)(msg_time.hour&0x1F)))/10)<<4;
		malloc_jt808_gps_info_data->date_time[3]|=(((uint8_t)(msg_time.hour&0x1F)))%10;

		malloc_jt808_gps_info_data->date_time[4]=((((uint8_t)(msg_time.min&0x3F)))/10)<<4;
		malloc_jt808_gps_info_data->date_time[4]|=(((uint8_t)(msg_time.min&0x3F)))%10;
		malloc_jt808_gps_info_data->date_time[5]=((((uint8_t)(msg_time.sec&0x3F)))/10)<<4;
		malloc_jt808_gps_info_data->date_time[5]|=(((uint8_t)(msg_time.sec&0x3F)))%10;		
	}
	
	//big end , little end switch
	malloc_jt808_gps_info_data->alarm_flags=high_low_switch_32(malloc_jt808_gps_info_data->alarm_flags);
	malloc_jt808_gps_info_data->uint32_status=high_low_switch_32(malloc_jt808_gps_info_data->uint32_status);
	malloc_jt808_gps_info_data->latitude=high_low_switch_32(malloc_jt808_gps_info_data->latitude);
	malloc_jt808_gps_info_data->longitude=high_low_switch_32(malloc_jt808_gps_info_data->longitude);

	malloc_jt808_gps_info_data->altitude=high_low_switch_16(malloc_jt808_gps_info_data->altitude);
	malloc_jt808_gps_info_data->speed=high_low_switch_16(malloc_jt808_gps_info_data->speed);
	malloc_jt808_gps_info_data->direction=high_low_switch_16(malloc_jt808_gps_info_data->direction);

	/*{
	uint8_t *tptr=(uint8_t *)malloc_jt808_gps_info_data;
	InfoPrintf("打包后,信息体:\r\n");
	for(i=0;i<malloc_len;i++)
	{
		InfoPrintf("%02X,",tptr[i]);
	}}
	InfoPrintf("\r\n");*/

		
	return (uint8_t *)malloc_jt808_gps_info_data;
}


int jt808_pack_and_post_gps_info_q(void)
{
	OS_ERR os_err=OS_ERR_NONE;
	uint16_t pd_rep_info_len=0;
	uint8_t *ptr=NULL;
	BEIJING_TIME_T  rtcTime={0};
	MSG_DATE_TIME_T msg_time={0};
 
	int ret=-1;

	if(get_rtc_init_flag()==0)
	{
		InfoPrintf("rtc not init !!!!can`t pack,return!!!!!\r\n");
		return -1; 
	}

	get_current_time(&rtcTime);
	rtcTime.year%=2000;
	if((rtcTime.year > 40)||(rtcTime.year < 16))
	{
		InfoPrintf("RTC时间错误 !!!%d\r\n",rtcTime.year);
		
		return -1;
	}
	
	msg_time.year=rtcTime.year;
	msg_time.mon=rtcTime.month;
	msg_time.day=rtcTime.day;
	msg_time.hour=rtcTime.hour;
	msg_time.min=rtcTime.minute;
	msg_time.sec=rtcTime.second;

	ptr=jt808_pack_gps_info_area(msg_time,&pd_rep_info_len);
	if(ptr!=NULL)
	{
		//pack ok	
		ret=-1;
		OSQPost(&pd_rep_info_data_Q,ptr,pd_rep_info_len,OS_OPT_POST_FIFO,&os_err);
		if(os_err == OS_ERR_NONE)
		{
			ret=0;
			InfoPrintf("post pd_rep_info_data Q ok\r\n");
		}
		else
		{
			InfoPrintf("post pd_rep_info_data Q failed -%d- \r\n",os_err);

			#if ENABLE_ITV
			OSQPost(&pd_rep_info_data_save_Q,ptr,pd_rep_info_len,OS_OPT_POST_FIFO,&os_err);
			if(os_err == OS_ERR_NONE)
			{
				ret=0;
			}
			#endif
			
			//数据量过快,无法满足缓冲
			user_free(ptr,__FUNCTION__);
			InfoPrintf("post pd_rep_info_data_save_Q failed -%d- \r\n",os_err);	
		}
	}
	else
	{
		InfoPrintf("pack pd package failed \r\n");
	}

	return ret;
}


#define _____pack_protocol_frame__big_endian_transmode_________________
//
//
//
//================大端传输模式==================
//
//
//


/*
	brief: pack one total air protocol frame , encrypt it, escape it, 
	       and then return it`s pointer
*/
static uint8_t *jt808_pack_one_protocol_frame(	uint16_t  msg_id,
												uint8_t  *pdu,
												uint16_t  pdu_len,
												uint16_t *fram_len,
												uint16_t *serial,
												uint8_t   multi_package_flag
							  				)
{
	uint16_t DeEscaped_malloc_len=0;
	uint16_t escaped_malloc_len=0;
	uint8_t *malloc_ptr=NULL;
	uint8_t *escaped_malloc_ptr=NULL;
	uint8_t *temp_ptr;
	uint16_t temp;
	//int i;

	if(multi_package_flag>1)
	{
		return NULL;
	}

	DeEscaped_malloc_len=1+12+pdu_len+1+1;
	if(multi_package_flag==1)
	{
		InfoPrintf("多包打包数据\r\n");
		DeEscaped_malloc_len+=4;//multi package
	}
	
	malloc_ptr=user_malloc(DeEscaped_malloc_len,__FUNCTION__);
	if(malloc_ptr!=NULL)
	{
		malloc_ptr[0]=JT808_PROTOCOL_FIX;
		
		malloc_ptr[1]=(uint8_t)(msg_id>>8);
		malloc_ptr[2]=(uint8_t)(msg_id);

		malloc_ptr[3]=(uint8_t)((multi_package_flag<<5)|(pdu_len>>8));
		malloc_ptr[4]=(uint8_t)pdu_len;

		temp_ptr=get_phone_num();
		memcpy(&malloc_ptr[5],temp_ptr,6);

		temp=get_serial_num();
		*serial=temp;
		malloc_ptr[11]=(uint8_t)(temp>>8);
		malloc_ptr[12]=(uint8_t)temp;

		memcpy(&malloc_ptr[13],pdu,pdu_len);
		
		malloc_ptr[13+pdu_len]=get_check_sum(&malloc_ptr[1],(DeEscaped_malloc_len-3));
		malloc_ptr[13+pdu_len+1]=JT808_PROTOCOL_FIX;
	}
	else
	{
		InfoPrintf("_malloc_fail_ 1_\r\n");
		return NULL;
	}

	#if 0
	InfoPrintf("注册包,未转义(%d):",DeEscaped_malloc_len);
	for(i=0;i<DeEscaped_malloc_len;i++)InfoPrintf("%02X,",malloc_ptr[i]);
	InfoPrintf("\r\n");
	#endif
		

	//escape operate
	temp=check_escape_bytes_amount(&malloc_ptr[1],(DeEscaped_malloc_len-3));
	escaped_malloc_len=DeEscaped_malloc_len+temp;
	escaped_malloc_ptr=user_malloc(escaped_malloc_len,__FUNCTION__);
	if(escaped_malloc_ptr!=NULL)
	{
		temp=jt808_escape_package(escaped_malloc_ptr,escaped_malloc_len,malloc_ptr,DeEscaped_malloc_len);	
		user_free(malloc_ptr,__FUNCTION__);
		if(temp!=0)
		{
			*fram_len=temp;

			#if 0
			InfoPrintf("注册包,已转义(%d):",temp);
			for(i=0;i<temp;i++)InfoPrintf("%02X,",escaped_malloc_ptr[i]);
			InfoPrintf("\r\n");
			#endif

			return escaped_malloc_ptr;
		}
		else
		{
			user_free(escaped_malloc_ptr,__FUNCTION__);	
		}
	}
	else
	{
		InfoPrintf("_malloc_fail_ 2_\r\n");
		user_free(malloc_ptr,__FUNCTION__);	
	}

	return NULL;
}


#define _____pack_app_frame___________________________________________

/*
	output : 
			result: the result value of the server ack

	return :
				0 : rx ack frame from server
			others: no rx ack frame from server
*/
static int jt808_wait_report_ack_q(uint16_t tx_serial,uint8_t *result)
{
	//#define jt808_wait_report_ack_q_print
	remote_result_q_t *p_msg;
	uint16_t pend_msg_len;
	uint8_t noAck_errAck_cnt=0;
	OS_ERR os_err;
	int ret=-2;
	uint8_t temp_result=0xFF;	
	while(1)
	{
		if(noAck_errAck_cnt < 2)
		{
			p_msg = (remote_result_q_t *)OSQPend(&rx_server_ack_q,T_5S,OS_OPT_PEND_BLOCKING,&pend_msg_len,NULL,&os_err);		
			if((NULL != p_msg)&&(pend_msg_len==sizeof(remote_result_q_t)))
			{
				if(p_msg->serial == tx_serial) 
				{
					if(p_msg->result==0)
					{
						#ifdef jt808_wait_report_ack_q_print
						InfoPrintf("report,ack ok,S=0x%04X\r\n",tx_serial);
						#endif
					}
					else
					{
						InfoPrintf("gprs rep,rx ack,S:%04X,R:%04X,but result=%u,take it as right rep...\r\n",tx_serial,p_msg->serial, p_msg->result);
					}
					
					temp_result=p_msg->result;
					//如果流水号对了,则直接返回本次定时上报OK，忽略平台解析的result
					ret = 0;
					
					user_free(p_msg,__FUNCTION__);
					break;
				}
				else
				{
					InfoPrintf("gprs rep,rx ack,S:%04X,R:%04X,result=%u,continue...\r\n",tx_serial,p_msg->serial, p_msg->result);
					noAck_errAck_cnt++;
					user_free(p_msg,__FUNCTION__);
					continue;
				}
			}
			else
			{
				InfoPrintf("gprs rep,no rx ack,S=0x%04X,continue wait...\r\n",tx_serial); 
				if(NULL != p_msg)
				{
					user_free(p_msg,__FUNCTION__);
				}	
				noAck_errAck_cnt++;
				continue;
			}
		}
		else
		{
			InfoPrintf("gprs rep,wait ack timeout,re_create socket!!!!!!!!!!!!!\r\n"); 
			modem_ReCreat_socket();
		}

		break;
	}
	
	*result=temp_result;//平台解析数据的合法性
	
	return ret; 		//网络发送是否成功
}

/*
	input:	
			info_data_ptr: point to the info_data RAM
			info_data_len: info_data length
			
	return:
			   0  : successful,    report and received ACK ok.
			others: report failed, need to save ITV
*/
int jt808_pack_and_rep_frame(uint8_t *info_data_ptr,uint16_t info_data_len)
{
	#define jt808_pack_and_rep_frame_print
    OS_ERR os_err;
	int ret = -1;
	uint16_t serial;
	uint8_t *package_ptr=NULL;
	uint16_t package_len;
	uint32_t temp;
	uint8_t result;
    uint32_t i;

	temp=OSTimeGet(&os_err);

	package_ptr = jt808_pack_one_protocol_frame(TM_GPS_INFO_REP_ID,
												(uint8_t*)(info_data_ptr),
												info_data_len,
												&package_len,
												&serial,
												0);
	
	if(package_ptr == NULL)
	{
		return -1;
	}

	ret=-2;

	#ifdef jt808_pack_and_rep_frame_print
	InfoPrintf("JT808上报(%d):",package_len);
	for(i=0;i<package_len;i++)InfoPrintf("%02X,",package_ptr[i]);InfoPrintf("\r\n");
	#endif

	if(modem_send_net_data(package_ptr,(uint32_t)package_len)>0)
	{
		#ifdef jt808_pack_and_rep_frame_print
		InfoPrintf("JT808上报发送完毕\r\n");InfoPrintf("等应答 !!!\r\n");
		#endif
		
		ret=jt808_wait_report_ack_q(serial,&result);
		if(result!=0)
		{
			InfoPrintf("pd rep,S=0x%04X,ack result value=%d,take is as ok !!!!\r\n",serial,result);
		}
	}
	else
	{
	   	InfoPrintf("pd rep,modem send failed,S=0x%04X !!!!!!!!!\r\n",serial);
	} 

	user_free(package_ptr,__FUNCTION__);

	
	if(ret==0)
	{
		#ifdef jt808_pack_and_rep_frame_print
		InfoPrintf("pd rep ok,  S=0x%04X used   time=%d ms\r\n",serial,OSTimeGet(&os_err)-temp);
		#endif
	}
	else
	{
		InfoPrintf("pd rep fail,S=0x%04X used  time=%d ms !!!!\r\n",serial,OSTimeGet(&os_err)-temp);
	}
	
	#ifdef jt808_pack_and_rep_frame_print
	printf_time_now();
	#endif

	return ret;
}

/*#pragma pack(1)
typedef struct wait_ack_{
	uint8_t  active_flag;
	union {
	uint32_t active_timestamp;
	uint8_t  active_timestamp_buf[4];
	};
	union {
	uint32_t timeout_timestamp;
	uint8_t  timeout_timestamp_buf[4];
	};
	union {
	uint16_t flow_num;
	uint8_t  flow_num_buf[2];
	};
	uint8_t  prot_package[1];
	
}WAIT_ACK_CACHE_T;
#pragma pack()

#pragma pack(1)
typedef struct node_{
	struct node_ *p_next;
	WAIT_ACK_CACHE_T data;
}PT_NODE_T;
#pragma pack()

PT_NODE_T *node_a=NULL,*node_b=NULL,node_c=NULL;*/

/*
	return :
	          0  : register ok
		  others :fail
*/
int jt808_pack_and_rep_register_frame(void)
{
	#define jt808_pack_and_rep_register_frame_print
	
	#define TM_TYPE_LEN (20) //注:是指字符串的长度，而不是整形数的位数
	#define TM_ID_LEN   (7)  //注:是指字符串的长度，而不是整形数的位数
	//车辆VIN(17位):LSVFA49J232037048
	
	uint16_t info_data_len;
	uint8_t  *p_fram;
	uint16_t packed_len;
	uint16_t sn;
	int ret;
	int temp_ret;
    uint8_t result;
#ifdef jt808_pack_and_rep_register_frame_print
	int i;
	uint8_t *ptr;
	uint32_t temp;
	OS_ERR os_err;
#endif
    
	REGISTER_INFO_T reg_info_data={0};
	uint32_t manufacturer_id;				//=88603;
	uint8_t  tm_type[TM_TYPE_LEN];			//="GTB1";
	uint8_t  tm_id[TM_ID_LEN];				//="2";

	memset(tm_type,0,TM_TYPE_LEN);
	memset(tm_id,0,TM_ID_LEN);
	
	manufacturer_id=88603;
	memcpy(tm_type,"GTB1",4);
	memcpy(tm_id,"123456789088",2);
	
	//信息体
	reg_info_data.province_id=0;								//省
	reg_info_data.town_id=0;   		 							//县
	
	reg_info_data.manufacturer[4]=(uint8_t)((manufacturer_id>>0)&0xFF);		//制造商ID
	reg_info_data.manufacturer[3]=(uint8_t)((manufacturer_id>>8)&0xFF);
	reg_info_data.manufacturer[2]=(uint8_t)((manufacturer_id>>16)&0xFF);
	reg_info_data.manufacturer[1]=0;
	reg_info_data.manufacturer[0]=0;
	
	tm_type[TM_TYPE_LEN-1]=0x00;								//强制终端类型的最后一个字节为0x00
	memcpy(reg_info_data.tm_type,tm_type,TM_TYPE_LEN);			//终端类型  (20byte)
	
	memcpy(reg_info_data.tm_id,tm_id,TM_ID_LEN);				//终端ID     (7byte)

	#ifndef ONENET_IP 
		//EXLIVE
		reg_info_data.license_plate_color=0x01; 					//车牌颜色
		memcpy(reg_info_data.license_num,card,CAR_LICENSE_NUM_LEN);
	#else
		//onenet
		reg_info_data.license_plate_color=0; 					//车牌颜色
		//使用ONENET时，不能在消息头里面携带车牌号码，
		//并且车牌颜色为填0
		//memcpy(reg_info_data.license_num,card,6);	
	#endif
	
	info_data_len= sizeof(REGISTER_INFO_T);
	
	#ifdef jt808_pack_and_rep_register_frame_print
		ptr=(uint8_t *)(&reg_info_data);
		InfoPrintf("\r\n\r\n\r\n注册信息体A(%d):",info_data_len);
		for(i=0;i<info_data_len;i++)InfoPrintf("%02X,",ptr[i]);InfoPrintf("\r\n");
		InfoPrintf("注册信息体B(%d):",info_data_len);
		for(i=0;i<info_data_len;i++)InfoPrintf("%c",ptr[i]);InfoPrintf("\r\n");
	#endif

	//打包协议包
	ret=-1;
	p_fram=jt808_pack_one_protocol_frame( TM_REGISTER_ID,
									  	  (uint8_t *)(&reg_info_data),
									   	  info_data_len,
									      &packed_len,
									      &sn,
									      0
							  		    );
	//发送协议包
	if(p_fram!=NULL)
	{
		#ifdef jt808_pack_and_rep_register_frame_print
		InfoPrintf("注册包,将发送(%d):\r\n",packed_len);
		for(i=0;i<packed_len;i++)InfoPrintf("%02X ",p_fram[i]);InfoPrintf("\r\n");
		#endif	
		
		if(modem_send_net_data(p_fram,packed_len)>0)
		{
			#ifdef jt808_pack_and_rep_register_frame_print
			InfoPrintf("JT808上报发送完毕\r\n");InfoPrintf("等应答 !!!\r\n");
			#endif
			
			temp_ret=jt808_wait_report_ack_q(sn,&result);
			if(temp_ret==0)//rx ack frame from the server
			{
				if(result==0)//ack result value is ok
				{
					ret=0;
				}
				else
				{
					InfoPrintf("register,modem send ok,but ack result value is %d !!!\r\n",result);
				}
			}
			else
			{
				InfoPrintf("register,modem send ok,but no rx ack frame!!!\r\n");
			}
		}
		else
		{
			InfoPrintf("register,modem send fail !!!\r\n");
		}

		user_free(p_fram,__FUNCTION__);
	}

	return ret;
}

/*
	return:
			   0: login ok
		  others: fail
*/
int jt808_pack_and_rep_login_frame(void)
{
	#define jt808_pack_and_rep_login_frame_print
	
	uint8_t  *p_fram;
	uint16_t packed_len;
	uint16_t sn;
	int ret,temp_ret;
	
	int i;
	uint8_t result;
	
	uint16_t info_data_len;
	uint8_t *info_data_ptr;


	info_data_len=strlen((char const *)(get_authorize_code()));
	info_data_ptr=get_authorize_code();


	

	ret=-1;
	p_fram=jt808_pack_one_protocol_frame( TM_GET_AUTHORISED_ID,
										  info_data_ptr,
										  info_data_len,
										  &packed_len,
										  &sn,
										  0
										);
	//发送协议包
	if(p_fram!=NULL)
	{
		#ifdef jt808_pack_and_rep_login_frame_print
		InfoPrintf("\r\n\r\n\r\n鉴权包,将发送(%d):\r\n",packed_len);
		for(i=0;i<packed_len;i++)InfoPrintf("%02X,",p_fram[i]);InfoPrintf("\r\n");
		#endif	
		
		if(modem_send_net_data(p_fram,packed_len)>0)
		{
			#ifdef jt808_pack_and_rep_login_frame_print
			InfoPrintf("JT808上报发送完毕\r\n");InfoPrintf("等应答 !!!\r\n");
			#endif
			
			temp_ret=jt808_wait_report_ack_q(sn,&result);
			if(temp_ret==0)//rx ack frame from the server
			{
				if(result==0)//ack result value is ok
				{
					ret=0;
				}
				else
				{
					InfoPrintf("login,modem send ok,but ack result value is %d !!!\r\n",result);
				}
			}
			else
			{
				InfoPrintf("login,modem send ok,but no rx ack frame!!!\r\n");
			}
		}
		else
		{
			InfoPrintf("login,modem send fail !!!\r\n");
		}

		user_free(p_fram,__FUNCTION__);
	}

	
	return ret;
}



/*
	input:
			info_data_ptr: packed info data, not include frame HEAD,CRC,TAIL
			info_data_len: length
	return:
				0 : save ok
			others: save failed
*/
int jt808_save_itv(uint8_t *info_data_ptr,uint32_t info_data_len)
{
	return 0;
}


/*
	return:
			0		: report ok
			others	: report failed 
*/
int jt808_pack_and_rep_itv_frame(uint8_t *data, uint16_t data_len)
{
	int ret = -1;
    #if 0    
    OS_ERR os_err;
    uint16_t package_len=0;
	uint8_t *package_ptr=NULL;
	//int i;
    uint16_t serial;

	uint32_t temp;
	uint8_t result;

	temp=OSTimeGet(&os_err);
	data[0] |= 0x80;//补报标志
	//package_ptr = sgmw_pack_one_protocol_frame(UP_REPORT_MSG_ID,data,data_len,&package_len,&serial);
	if(package_ptr == NULL)
	{
		return -1;
	}

	ret=-2;
	if(modem_send_net_data(package_ptr,package_len)>0)
	{
		ret=jt808_wait_report_ack_q(serial,&result);
	}
	else
	{
	   InfoPrintf("ITV rep,modem send failed,S=0x%04X !!!!!!!!!\r\n",serial);
	} 

	if(ret==0)
	{
		InfoPrintf("ITV rep ok, S=0x%04X used   time=%d ms\r\n",serial,OSTimeGet(&os_err)-temp); 
	}
	else
	{	
		InfoPrintf("ITV rep fail, S=0x%04X used time=%d ms!!!!\r\n",serial,OSTimeGet(&os_err)-temp); 
	}

	user_free(package_ptr,__FUNCTION__);
#endif
	return ret;
}



void jt808_read_and_rep_itv(void)
{
	//read itv data
	//...
	
	//pack and rep
	jt808_pack_and_rep_itv_frame(NULL,0);
}

#define ____general_air_communication__________________________________________

#define ____TASK___________AREA__________________________________________


void pd_pack_info_data_tmr_callback(void*p_Tmr,void*p_Arg)
{
	OS_ERR os_err;
	InfoPrintf("pd_pack_info_data_tmr_callback()---%d---\r\n",OSTimeGet(&os_err));
	jt808_pack_and_post_gps_info_q();	
}

void create_and_start_pd_packing_timer(void)
{
	OS_ERR os_err;
	//create the timer
	OSTmrCreate(&period_report_tmr,
				"period report tmr",
				10,//N*100ms ,基本单位是100ms  zgc
				get_report_period(), 
				OS_OPT_TMR_PERIODIC,
				pd_pack_info_data_tmr_callback,
				NULL,
				&os_err);
	
	OSTmrStart(&period_report_tmr, &os_err);
}

void change_report_period(uint32_t preriod)
{
	//该功能在这个版本的UCOS需要delete tmr,在重新创建 ????
	if(period_report_tmr.Period != preriod)
	{
		//period_report_tmr.Period  = preriod;
	}
}

void  remote_report_task_msg_handle(TASK_MSG_T *p_msg)
{
	//OS_ERR os_err;
	//uint8_t error_flag=0;
	#if 0
	if(p_msg->msg_type == CTRL_MSG_TYPE)
	{
		if(PWR_CTRL_ON == p_msg->msg_son_type)
		{
			InfoPrintf("period_report task rx ctl msg: pwr on\r\n");
		}
		else if(PWR_CTRL_SLEEP == p_msg->msg_son_type)
		{
			InfoPrintf("period_report task rx ctl msg: sleep\r\n");
			OSTaskSuspend(NULL,&os_err);
		}
		else if(PWR_CTRL_WAKEUP == p_msg->msg_son_type )
		{
			InfoPrintf("period_report task rx ctl msg: wake up\r\n");
		}
		else if(PWR_CTRL_STANDBY == p_msg->msg_son_type )
		{
			InfoPrintf("period_report task rx ctl msg: standby\r\n");
		}
		else
		{
			error_flag=2;
		}
	}
	else if(p_msg->msg_type == PWR_STATE_MSG_TYPE)
	{
		InfoPrintf("period_report task rx msg:ACC=%u\r\n",p_msg->msg_son_type);
	}	
	else
	{
		error_flag=5;
	}

	if(error_flag!=0)
	{
		InfoPrintf("period report task rx err msg:---%d---!!!\r\n",error_flag);
	}
	#endif
}


void period_report_save_task(void*p_para)
{
	OS_ERR os_err;
	//int i;
	
	uint8_t *ptr=NULL;
	uint16_t rep_msg_len;
	
	OSQCreate(&pd_rep_info_data_save_Q,"pd_rep_info_data_save Q",PD_REP_INFO_DATA_SAVE_Q_MAX,&os_err);
	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf("create pd_rep_info_data_save Q fail\r\n");
	}
	
	p_para = p_para;
	while(1)
	{	
		ptr=(uint8_t *)OSQPend(&pd_rep_info_data_save_Q,T_10MS*3,OS_OPT_PEND_BLOCKING,&rep_msg_len,NULL,&os_err);
		if(ptr != NULL)
		{
			if(jt808_save_itv((uint8_t *)ptr,rep_msg_len)!=0)
			{
				InfoPrintf("jt808_save_itv(xxx,%d)failed C !!\r\n",rep_msg_len);
			}
			else
			{
				InfoPrintf("jt808_save_itv(xxx,%d) ok (Q full)\r\n",rep_msg_len);
			}
			
			user_free(ptr,__FUNCTION__);
		}
		
		OSSchedRoundRobinYield(&os_err);
	}
}


void remote_report_task(void*p_para)
{
	OS_ERR os_err;
   // uint32_t t_wait=0;
	//int i;
	int ret;
	TASK_MSG_T *p_msg;
	uint16_t msg_size;
	uint8_t  *rep_msg=NULL;
	uint16_t rep_msg_len=0; 
	p_para = p_para;
	
	/*BEIJING_TIME_T temp_time;
	temp_time.year=16;
	temp_time.month=10;
	temp_time.day=21;
	temp_time.hour=15;
	temp_time.minute=46;
	temp_time.second=2;
	write_rtc(temp_time);*/

	#if 1
	OSTaskCreate(&period_report_save_task_Tcb,
				"tcb_period_report_save task",
				(OS_TASK_PTR)    period_report_save_task,
				(void          *)NULL,
				PERIOD_REP_SAVE_TASK_PRIO,
				(CPU_STK 	*)  period_rep_save_task_stk,
				(CPU_STK_SIZE)  PERIOD_REP_SAVE_TASK_STK_SIZE/10,/* limit*/
				(CPU_STK_SIZE)  PERIOD_REP_SAVE_TASK_STK_SIZE,
				(OS_MSG_QTY)    PD_REPORT_SAVE_TASK_MSG_QTY,
				(OS_TICK )      PD_REPORT_SAVE_TASK_TIME_QUANTA,
				(void  *)       NULL,
				(OS_OPT)        (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				&os_err);	

	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf("creat period_report_save_task failed !!!\r\n");
	}
	#endif
	
	OSQCreate(&pd_rep_info_data_Q,"pd_rep_info_data Q",PD_REP_INFO_DATA_Q_MAX,&os_err);
	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf("create pd_rep_info_data Q fail\r\n");
	}

	//create the timer
	create_and_start_pd_packing_timer();
	
	while(1)
	{	
		//----------------------------------------------------------------------------------------
		p_msg = (TASK_MSG_T*)OSTaskQPend(1,OS_OPT_PEND_NON_BLOCKING,&msg_size,NULL,&os_err);	
        if(NULL != p_msg)
		{
			remote_report_task_msg_handle(p_msg);	
			user_free(p_msg,__FUNCTION__);
		}

		//---------------------------------------------------------------------------------------
		rep_msg=(uint8_t *)OSQPend(&pd_rep_info_data_Q,1,OS_OPT_PEND_BLOCKING,&rep_msg_len,NULL,&os_err);
		if(rep_msg != NULL)
		{
			if(get_already_login_flag()==1)
			{
				ret=jt808_pack_and_rep_frame((uint8_t *)rep_msg,rep_msg_len);
				if(ret==0)
				{
					InfoPrintf("jt808_pack_and_rep_frame() ok\r\n");
				}
				else
				{
					InfoPrintf("jt808_pack_and_rep_frame() failed!!!\r\n");
					#if ENABLE_ITV
					if(jt808_save_itv((uint8_t *)rep_msg,rep_msg_len)!=0)
					{
						InfoPrintf("save_itv(xxx,%d)failed (net fault) !!\r\n",rep_msg_len);
					}
					else
					{
						InfoPrintf("save_itv(xxx,%d)ok (net fault) !!\r\n",rep_msg_len);
					}
					#endif
				}	
			}
			else
			{
				InfoPrintf("save_itv(xxx,%d)failed -- (no login) !!\r\n",rep_msg_len);
			
				//save itv
				#if ENABLE_ITV
				if(jt808_save_itv((uint8_t *)&rep_msg[2],rep_msg_len)!=0)
				{
					InfoPrintf("save_itv(xxx,%d)failed (no login) !!\r\n",rep_msg_len);
				}
				else
				{
					InfoPrintf("save_itv(xxx,%d)ok (no login) !!\r\n",rep_msg_len);
				}
				#endif
			}

			user_free(rep_msg,__FUNCTION__);
		}
        
        if(get_already_login_flag()==1)
		{
            //req_upgrade();
			#if ENABLE_ITV
			if((OSTimeGet(&os_err) - t_wait) > T_1S)
			{
				t_wait = OSTimeGet(&os_err);
				if(get_gprs_send_fun() == 1)
				{
				   //sgmw_read_and_rep_itv();
				}
			}	
			#endif
		}

		user_delay_ms(TIME_10ms);
	}
}

#endif


