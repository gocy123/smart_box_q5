#include "user_system_cfg.h"

#ifdef USE_PROTOCOL_SGMW

#include <stdint.h>

#include "os.h"
#include "os_cfg_app.h"	//TCB,STACK,PRIORITY,�������Ϣ�Ľṹ�嶨��ͺ�
#include "timer_define.h"

#include "aes.h"
#include "rsa_main.h"
#include "modem_sim800_app.h"
#include "sgmw_protocol_tx.h"
#include "sgmw_protocol_rx.h"
#include "user_debug_app.h"
#include "sgmw_telecom_prot_manage.h"
#include "user_gps_app.h"
#include "user_rtc_app.h"


extern OS_Q 	pd_rep_info_data_Q;
extern OS_Q 	pd_rep_info_data_save_Q;


char *jhn_test_str="0000202134312545656786654rbfwgejutyityutyrterwwwert";

extern int get_report_period(void);

#if ENABLE_RM_DBG
extern 	uint8_t *tbc_report_buf;
#endif
extern uint8_t aes_key_public[];

sgwm_md5_t 		md5_t;
extern uint8_t 	FileCopyBuffer[1024];
extern uint32_t	upgrade_tbc_add;
extern uint32_t	SD_save_time; //SD������ʱ��
extern uint8_t 	SD_save_status; //�Ƿ񱣴�SD�� 0������ 1����
extern uint8_t 	report_TBC_type; //tbc�ش���ʽ 1 DBC 2 TBC

extern uint8_t     	get_already_login_flag(void);

#if 1
#define ______EXTERNAL__FUNCTION________________________________________

uint32_t  	SD_save_time 	= 0; //SD������ʱ��0�Զ�
uint8_t 	SD_save_status 	= 1; //�Ƿ񱣴�SD�� 0������ 1����
uint8_t 	report_TBC_type = 2; //tbc�ش���ʽ 1 DBC 2 TBC


uint8_t sgmw_tm_barcode[SGMW_TM_BARCODE_LEN+1]="7684401602394839519G5300133";
uint8_t sgmw_tm_sim[SGMW_SIM_SN_LEN+1]="1064812345678";
uint8_t sgmw_tm_ccid[SGMW_CCID_SN_LEN+1]="898602b2211530000179";//898602b2211530000179
uint8_t sgmw_tm_vin[SGMW_VIN_LEN+1]="LK012345678912345";


char 	*rd_barcode(void){return (char *)sgmw_tm_barcode;}
uint8_t  get_mount_flag(void){return 0;}
char 	*get_sim_ccid (void){return (char *)sgmw_tm_ccid;}

uint16_t get_main_power_voltage(void)
{
	return 12345;//mV
}

uint8_t  get_no_bat_state(void)
{//�Ƿ��ѶϿ����õ��: 1:�ǣ�0:δ�Ͽ�
	return 0;
}

uint16_t get_battery_voltage(void)
{
	return 6789;//mV
}

uint32_t WriteToAppArea(uint32_t *FlashAddr,unsigned char *buf,uint32_t len)
{
	return 0;
}



int get_upgradefile_md5(uint8_t *ret_md5,uint32_t addr_t,uint32_t file_len)
{
	return 0;
}

uint32_t tbc_get_report_buf_size(void)
{
	return 0;
}
void tbc_clear_report_data_buf(void)
{

}
void tbc_clear_report_update_flag(void)
{

}
uint8_t get_acc_status(void)
{
	return 1;
}

int get_default_tbc_name(uint8_t * file_name_t)
{
	return 0;
}
char * get_version(void)
{
	static char buf[]="ver_xxxxxxxxxxxx";
	return buf;
}

uint32_t read_file_exist_flag(uint8_t i)
{
	return 0;
}
void  get_tbc_file_list(uint8_t *buf,uint16_t *len)
{

}

unsigned int get_heartbeat_period(void)
{
	return 0;
}
int Get_Emmc_Info(uint8_t pdrv, uint8_t cmd, void *pBuf)
{
	return 0;
}


void feed_wdt(void)
{

}
void read_tbc_file_name(uint8_t *pbuf,uint8_t i)
{

}
uint32_t  read_tbc_file_total(uint8_t i)
{
	return 0;
}
int write_multi_tbcfile_param(uint32_t f_type,uint8_t *name,uint32_t f_name_len,\
							  uint32_t t_add,uint32_t f_file_len,uint8_t *t_file_md5,\
							  uint32_t default_f,uint32_t t_total,uint32_t f_exist_flag,\
							  uint8_t i)
{
	return 0;
}


void set_sd_save_status(uint8_t SD_save_status )
{
	//eeprom_write(SD_SAVE_STATUS_ADDR, (uint8_t*)(&SD_save_status),1);
}

void set_rep_tbc_type(uint8_t report_TBC_type )
{
	//eeprom_write(REP_TBC_TYPE_ADDR, (uint8_t*)(&report_TBC_type),1);
}

uint32_t get_sd_save_time(void)
{
	uint32_t temp=0;
	//eeprom_read(SD_SAVE_TIME_ADDR, (uint8_t*)(&temp),4);
	return temp;
}

uint8_t get_sd_save_status(void )
{
	uint8_t temp=0;
	//eeprom_read(SD_SAVE_STATUS_ADDR, (uint8_t*)(&temp),1);
	return temp;
}

uint8_t get_rep_tbc_type(void )
{
	uint8_t temp=0;
	//eeprom_read(REP_TBC_TYPE_ADDR, (uint8_t*)(&temp),1);
	return temp;
}

void set_sd_save_time(uint32_t  SD_save_time )
{
	//eeprom_write(SD_SAVE_TIME_ADDR, (uint8_t*)(&SD_save_time),4);
}


void  get_dbc_md5(uint8_t *buf,uint8_t *md5_t,uint8_t len)
{
   uint8_t i;
   for(i=0;i<len;i++)
   {
      *(md5_t+i) = *(buf+i);
   }
}

int sgmw_get_dev_id(void)
{
	return 20908888;
}

uint8_t GetTermFault(void)  //��ȡ�ն˹��� =1��ʾ�й��ϣ�0��ʾ��
{
	return 0;
}

int get_report_period(void)
{
	return 50;//N*100ms ,������λ��100ms  zgc ,���δ���1ms  ????
}

int get_gprs_send_fun(void)
{
	return 1;
	//return gprs_enable;
}

#endif

#define ____function_start____________________________________________________________

//uint16_t �ߵ��ֽ�ת��
unsigned short htons(unsigned short n)
{
  return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

/************************************************************
������get_check_sum ���������У���
����:*p_data ��Ҫ����У��͵�Դ���ݣ�lenԴ�����ֽ���
���:
����ֵ:���У���
����:liaoxc 2015-11-10
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

__no_init uint16_t sgmw_serial_num;

/************************************************************
������get_serial_num ����ȡ�ϴ����ݵ���ˮ��
����:
���:
����ֵ:Э����ˮ��
����:liaoxc 2015-11-16
***************************************************************/
uint16_t get_serial_num(void)
{
	uint16_t tmp;
	//InfoPrintf("��ˮ��=%d\r\n",sgmw_serial_num);
	tmp = sgmw_serial_num;
	sgmw_serial_num++;
	
	return tmp;
}


/************************************************************
������reset_serial_num ����ȡ�ϴ����ݵ���ˮ��
����:
���:
����ֵ:Э����ˮ��
����:liaoxc 2015-11-16
***************************************************************/
void reset_serial_num(void)
{
	sgmw_serial_num = 1;
}

#define ____escape__DeEscape____________________________________________
/************************************************************
������check_escape_bytes_amount ��������Ҫת������ݸ���
����:*p��Ҫ��Դ���ݣ�lenԴ�����ֽ���
���:
����ֵ:��Ҫת����ֽ���
����:zgc 2015-11-10
***************************************************************/
int check_escape_bytes_amount(uint8_t *p,int len)
{
	int i,j=0;
	for(i=0;i<len;i++)
	{
		if(*p == SGWM_PROTOCL_ESCAPE_CODE || *p == SGMW_PROTOCOL_FIX)
		{
			j++;
		}
		p++;
	}

	return j;
}

/************************************************************
ת�崦�����壺��0x7Eת��0x7D 0x02��0x7Eת��0x7D 0x01
����:*Src δת���ͷβ��Э�������
���:*Dst ת���Ĵ�ͷβ��Э�������
����ֵ:ת�������ݳ���
����:liaoxc 2015-11-10

***************************************************************/
uint16_t sgwm_escape_package(uint8_t*Dst,uint8_t *Src,uint16_t SrcLen)
{
	uint16_t len=1;
	uint16_t i=0;
	
	*Dst++ = *Src++; //head
	for(i=SrcLen;i > 2;i--)//�ӵڶ�������head���ַ���ʼ
	{
		if(*Src == SGMW_PROTOCOL_FIX)
		{
			
			*Dst++ = SGWM_PROTOCL_ESCAPE_CODE;
			*Dst++ = 0x02;
			Src++;
			len += 2;
		}
		else if(*Src == SGWM_PROTOCL_ESCAPE_CODE)
		{
			
			*Dst++ = SGWM_PROTOCL_ESCAPE_CODE;
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


/*
��ת�崦�����壺��0x7D 0x01ת��0x7D��0x7D 0x02ת��0x7E
����:*Src δת���ͷβ��Э�������
���:*Dst ת���Ĵ�ͷβ��Э�������
����ֵ:��ת�������ݳ���
����:liaoxc 2015-11-10
*/
uint16_t sgmw_DeEscape_package(uint8_t*Dst,uint8_t *Src,uint16_t SrcLen)
{
	uint16_t len=1;
	uint16_t i=0;
	
	*Dst++ = *Src++;//head
	i=2;
	while(i<SrcLen)//�ӵڶ�������head���ַ���ʼ
	{
		if(*Src != SGWM_PROTOCL_ESCAPE_CODE)
		{
			*Dst++ = *Src++;
			i++;
		}
		else
		{
			if(*(Src+1) == 0x02)
			{
				*Dst = SGMW_PROTOCOL_FIX;
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

static void sgmw_protocol_format_time_for_rep(sgwm_protocol_time_format_t *time,MSG_DATE_TIME_T normal_time)
{
	time->year_ge=normal_time.year%10;
	time->year_shi=normal_time.year/10;
    time->re_report = 0;
		
	time->month_ge=normal_time.mon%10;
	time->month_shi=normal_time.mon/10;
		
	time->day_ge=normal_time.day%10;
	time->day_shi=normal_time.day/10;
		
	time->hour_ge=normal_time.hour%10;
	time->hour_shi=normal_time.hour/10;

	time->minute_ge=normal_time.min%10;
	time->minute_shi=normal_time.min/10;

	time->second_ge=normal_time.sec%10;
	time->second_shi=normal_time.sec/10;

	time->sec_mili_bai= (normal_time.msec%1000)/100;
	time->sec_mili_qian= 0;
 														
	time->sec_mili_ge= normal_time.msec%10;
	time->sec_mili_shi=(normal_time.msec%100)/10;
}

/************************************
������:sgmw_protocol_format_pwr_voltage_for_rep
����	:����Դ��
����:weicheng 2015.11.20 
*************************************/
static void sgmw_protocol_format_pwr_voltage_for_rep(sgwm_power_voltage_t *power_vol_t)
{
	uint32_t bat_volt =0;
	
	bat_volt = get_main_power_voltage();
	//InfoPrintf("bat_volt=%d\r\n",bat_volt);
	
	power_vol_t->stor_bat_shi = bat_volt/10000;
	power_vol_t->stor_bat_ge = (bat_volt%10000)/1000;
	power_vol_t->stor_bat_shifen = (bat_volt%1000)/100;
	power_vol_t->stor_bat_baifen = (bat_volt%100)/10;
	
	if(get_no_bat_state()==1)
	{
		bat_volt =0x00; 
	}
	else
	{
		bat_volt = get_battery_voltage();
	}
	power_vol_t->lith_bat_ge = (bat_volt%10000)/1000;
	power_vol_t->lith_bat_shifen = (bat_volt%1000)/100;
	power_vol_t->lith_bat_baifen = (bat_volt%100)/10;
	power_vol_t->lith_bat_qianfen = (bat_volt%10);
}

/***************************************
������:sgmw_protocol_format_gsm_gps_sta_for_rep
����	:���GSM��GPS״̬
****************************************/
static void sgmw_protocol_format_gsm_gps_sta_for_rep(sgwm_gsm_gps_status_t *gsm_gps_status,gps_location_info_T *gps_location_t)
{
    uint8_t gsm_sig_val=0;

	//gps
    if(gps_check_lacation_state()==1)
    {
        gsm_gps_status->gps_valid = 1;
	}
	else
	{
		gsm_gps_status->gps_valid = 0;
	}

	//can	zgc   �������� �붨������� ��һ��
	#if 0
	if(tbc_get_report_update_flag()&&(NULL != tbc_report_data_buf_ok()))//can��ʶ����can����	
	{		
		if(!read_default_tbc_file_md5(md5_t.md5_buf))
		{
			gsm_gps_status->can_rec_valid = 1;//��	
		}
	}	
	else	
	#endif	
	{		
		gsm_gps_status->can_rec_valid = 0;	
	}
	
	//terminal fault
	if(GetTermFault()) //=1 have fault
	{
		gsm_gps_status->dev_breakdown_valid = 1;
	}
	else
	{
		gsm_gps_status->dev_breakdown_valid = 0;
	} 
	
	//reserve
	gsm_gps_status->reserved = 0;

	//gps satellite number
    gsm_gps_status->gps_num_ge = Get_GPS_Number()%10;	//gps���Ǹ���
	gsm_gps_status->gps_num_shi= Get_GPS_Number()/10;

	//gsm signal quality
	gsm_sig_val = modem_get_sig_val();	
	gsm_gps_status->gsm_signal_ge = gsm_sig_val%10; 	//GSM�ź�ǿ��
	gsm_gps_status->gsm_signal_shi = gsm_sig_val/10;
}

/*************************************
������	:longitude_for_report
����	:����ת����Э���ʽ
����	:weicheng 2015.11.23
**************************************/
static void gps_longitude_for_rep(sgwm_protocol_gps_format_t *gps,gps_location_info_T *locat)
{
	//$GNRMC,004505.000,A,2241.001098,N,11416.656622,E,0.028,129.792,201015,,E,A*3F
	uint8_t len,i,poit;
	
	len = strlen((char const*)locat->longitude);

	if(len == 0)
	{	
		return;
	}
	
	for(i=0;i<len;i++)
	{
		if(locat->longitude[i] == '.') 
		{
			poit = i;
			break;
		}
	}
	
	switch(poit)
	{
		case 5:
			gps->lng_du_shi = locat->longitude[1]-'0';
			gps->lng_du_bai = locat->longitude[0]-'0';
			if(locat->longitude_EW == 'E')
			{
				gps->lng_ew = 0;
			}
			else if(locat->longitude_EW == 'W')
			{
				gps->lng_ew = 1;
			}

			gps->lng_fen_shi = locat->longitude[3]-'0';
			gps->lng_du_ge = locat->longitude[2]-'0';

			gps->lng_fen_shifen = locat->longitude[6]-'0';
			gps->lng_fen_ge = locat->longitude[4]-'0';

			gps->lng_fen_qianfen = locat->longitude[8]-'0';
			gps->lng_fen_baifen = locat->longitude[7]-'0';

			gps->lng_fen_shiwanfen = locat->longitude[10]-'0';
			gps->lng_fen_wanfen = locat->longitude[9]-'0'; //�ֵ����
			break;
			
		case 4:
			gps->lng_du_shi = locat->longitude[0]-'0';
			gps->lng_du_bai = 0;
			if(locat->longitude_EW == 'E')
			{
				gps->lng_ew = 0;
			}
			else if(locat->longitude_EW == 'W')
			{
				gps->lng_ew = 1;
			}

			gps->lng_fen_shi = locat->longitude[2]-'0';
			gps->lng_du_ge = locat->longitude[1]-'0';

			gps->lng_fen_shifen = locat->longitude[5]-'0';
			gps->lng_fen_ge = locat->longitude[3]-'0';

			gps->lng_fen_qianfen = locat->longitude[7]-'0';
			gps->lng_fen_baifen = locat->longitude[6]-'0';

			gps->lng_fen_shiwanfen = locat->longitude[9]-'0';
			gps->lng_fen_wanfen = locat->longitude[8]-'0'; //�ֵ����
			break;	
			
		default:
			break; 
	}
}

/*************************************
������	:latitude_for_report
����	:γ��ת����Э���ʽ
����	:weicheng 2015.11.23
**************************************/
static void gps_latitude_for_rep(sgwm_protocol_gps_format_t *gps,gps_location_info_T *locat)
{
	//$GNRMC,004505.000,A,2241.001098,N,11416.656622,E,0.028,129.792,201015,,E,A*3F
	uint8_t len,i,poit=0;
	len = strlen((char const*)locat->latitude);
	if(len == 0)return;
	for(i=0;i<len;i++)
	{
		if(locat->latitude[i] == '.') 
		{
			poit = i;
			break;
		}
	}
	switch(poit)
	{
		case 4:
			gps->lat_du_shi = locat->latitude[0]-'0';
			gps->lat_du_bai = 0;
			if(locat->latitude_NS == 'N')
			{
				gps->lat_ns = 1;
			}
			else if(locat->latitude_NS == 'S')
			{
				gps->lat_ns = 0;
			}

			gps->lat_fen_shi = locat->latitude[2]-'0';
			gps->lat_du_ge = locat->latitude[1]-'0';

			gps->lat_fen_shifen = locat->latitude[5]-'0';
			gps->lat_fen_ge = locat->latitude[3]-'0';

			gps->lat_fen_qianfen = locat->latitude[7]-'0';
			gps->lat_fen_baifen = locat->latitude[6]-'0';

			gps->lat_fen_shiwanfen = locat->latitude[9]-'0';
			gps->lat_fen_wanfen = locat->latitude[8]-'0';//�ֵ����	   
			break;
		case 3:
			gps->lat_du_shi = 0;
			gps->lat_du_bai = 0;
			if(locat->latitude_NS == 'N')
			{
				gps->lat_ns = 1;
			}
			else if(locat->latitude_NS == 'S')
			{
				gps->lat_ns = 0;
			}

			gps->lat_fen_shi = locat->latitude[1]-'0';
			gps->lat_du_ge = locat->latitude[0]-'0';

			gps->lat_fen_shifen = locat->latitude[4]-'0';
			gps->lat_fen_ge = locat->latitude[2]-'0';

			gps->lat_fen_qianfen = locat->latitude[6]-'0';
			gps->lat_fen_baifen = locat->latitude[5]-'0';

			gps->lat_fen_shiwanfen = locat->latitude[8]-'0';
			gps->lat_fen_wanfen = locat->latitude[7]-'0';//�ֵ����	 
			break;
			
		default:
			break;
	}	 	 
}

/**************************************
������	:speed_for_report
����	:�ٶ�ת����Э���ʽ
����	:weicheng 2015.11.23
**************************************/
static void gps_speed_for_rep(sgwm_protocol_gps_format_t *gps,gps_location_info_T *locat)
{
	//$GNRMC,004505.000,A,2241.001098,N,11416.656622,E,0.028,129.792,201015,,E,A*3F
   uint8_t len,i,poit=0;
   len = strlen((char const*)locat->speed);
   if(len == 0)return;
   if(len == 1)
   {
      poit = 0;
   }
   else
   {
      for(i=0;i<len;i++)
      {
		  if(locat->speed[i] == '.') 
		  {
			 poit = i;
			 break;
		  }
     }
   }
   switch(poit)
   {
        case 0:
              gps->speed_shi = 0;
			  gps->speed_bai = 0;

			  gps->speed_shifen = 0;
			  gps->speed_ge = 0;

              break;
		case 1:
              gps->speed_shi = 0;
			  gps->speed_bai = 0;

			  gps->speed_shifen = locat->speed[2]-'0';
			  gps->speed_ge = locat->speed[0]-'0';
		      break;
	   case 2:
              gps->speed_shi = locat->speed[0]-'0';
			  gps->speed_bai = 0;

			  gps->speed_shifen = locat->speed[3]-'0';
			  gps->speed_ge = locat->speed[1]-'0';
			  break;
	   case 3:
              gps->speed_shi = locat->speed[1]-'0';
			  gps->speed_bai = locat->speed[0]-'0';

			  gps->speed_shifen = locat->speed[4]-'0';
			  gps->speed_ge = locat->speed[2]-'0';
		      break;
	   default:break;

   }

}
/**************************************
������	:speed_direction_for_report
����	:�Ƕ�ת����Э���ʽ
����	:weicheng 2015.11.23
**************************************/
static void gps_speed_direction_for_rep(sgwm_protocol_gps_format_t *gps,gps_location_info_T *locat)
{
	//$GNRMC,004505.000,A,2241.001098,N,11416.656622,E,0.028,129.792,201015,,E,A*3F

	   uint8_t len,i,poit=0;
	   len = strlen((char const*)locat->speed_direction);
	   if(len == 0)return;
	   if(len == 1)
	   {
          poit = 0;
	   }
	   else
	   {
          for(i=0;i<len;i++)
	      {
			  if(locat->speed_direction[i] == '.') 
			  {
				 poit = i;
				 break;
			  }
	     }
	   }
	   
	   switch(poit)
	   {
           case 0:
		   	      gps->dir_shi = 0;
				  gps->dir_bai = 0;

				  gps->dir_shifen = 0;
				  gps->dir_ge = 0;
                  break;
		   case 1:
                  gps->dir_shi = 0;
				  gps->dir_bai = 0;

				  gps->dir_shifen = locat->speed_direction[2]-'0';
				  gps->dir_ge = locat->speed_direction[0]-'0';
			      break;
		   case 2:
                  gps->dir_shi = locat->speed_direction[0]-'0';
				  gps->dir_bai = 0;

				  gps->dir_shifen = locat->speed_direction[3]-'0';
				  gps->dir_ge = locat->speed_direction[1]-'0';
				  break;
		   case 3:
                  gps->dir_shi = locat->speed_direction[1]-'0';
				  gps->dir_bai = locat->speed_direction[0]-'0';

				  gps->dir_shifen = locat->speed_direction[4]-'0';
				  gps->dir_ge = locat->speed_direction[2]-'0';
				  break;
		   default:break;
	
	   }


}
/**************************************
������	:altitude_for_report
����	:���θ߶�ת����Э���ʽ
����	:weicheng 2015.11.23
**************************************/
static void gps_altitude_for_rep(sgwm_protocol_gps_format_t *gps,gps_location_info_T *locat)
{
	//$GNRMC,004505.000,A,2241.001098,N,11416.656622,E,0.028,129.792,201015,,E,A*3F


	   uint8_t len,i,poit=0;
	   len = strlen((char const*)locat->altitude);
	   if(len == 0)return;
	   if(len == 1)
	   {
	      poit = 0;
	   }
	   else
	   {
	      for(i=0;i<len;i++)
	      {
			  if(locat->altitude[i] == '.') 
			  {
				 poit = i;
				 break;
			  }
	     }
	   }
	   switch(poit)
	   {
           case 0:
                 gps->high_bai = 0;
				 gps->high_qian = 0;
				 gps->high_lev = 0;

				 gps->high_ge = 0;
				 gps->high_shi = 0;
				 break;
		   case 1:
                 gps->high_bai = 0;
				 gps->high_qian = 0;
				 gps->high_lev = 0;


				 gps->high_ge = locat->altitude[0]-'0';
				 gps->high_shi = 0;
			      break;
		   case 2:
                 gps->high_bai = 0;
				 gps->high_qian = 0;
				 gps->high_lev = 0;


				 gps->high_ge = locat->altitude[1]-'0';
				 gps->high_shi = locat->altitude[0]-'0';
				  break;
		   case 3:

                  gps->high_bai = locat->altitude[0]-'0';
				 gps->high_qian = 0;
				 gps->high_lev = 0;


				 gps->high_ge = locat->altitude[2]-'0';
				 gps->high_shi = locat->altitude[1]-'0';
				  break;
		  case 4:

		         gps->high_bai = locat->altitude[1]-'0';
				 gps->high_qian = locat->altitude[0]-'0';
				 gps->high_lev = 0;


				 gps->high_ge = locat->altitude[3]-'0';
				 gps->high_shi = locat->altitude[2]-'0';
				 break;
		   default:break;
	   }
}

/**************************************
������	:mileage_for_report
����	:���ת����Э���ʽ
����	:weicheng 2015.11.23
**************************************/
static void  gps_mileage_for_rep(sgwm_protocol_gps_format_t *gps)
{
    gps->odo_qianwan = 0;
	gps->odo_yi = 0;

	gps->odo_shiwan = 0;
	gps->odo_baiwan = 0;

	gps->odo_qian = 0;
	gps->odo_wan = 0;

	gps->odo_shi = 0;
	gps->odo_bai = 0;

    gps->odo_shifen = 0;
	gps->odo_ge = 0;

}

/***************************************
������:sgmw_protocol_format_gps_info_for_rep
����	:��䶨λ���ݱ�
����:weicheng 2015.11.20
***************************************/
static void sgmw_protocol_format_gps_info_for_rep(sgwm_protocol_gps_format_t *gps,gps_location_info_T *locat)
{
    //$GNRMC,004505.000,A,2241.001098,N,11416.656622,E,0.028,129.792,201015,,E,A*3F
    //����
    gps_longitude_for_rep(gps,locat);
	//γ��
    gps_latitude_for_rep(gps,locat);
	//�߶�altitude
	gps_altitude_for_rep(gps,locat);
	//����
	gps_speed_direction_for_rep(gps,locat);
    //�ٶ�
    gps_speed_for_rep(gps,locat);
    //���
	gps_mileage_for_rep(gps);
}


/*
	discription: pack the info data area of the periodic report frame,
	             not inclue HEAD ,CRC,TAIL !!!!!!!!!!
	
	input:
			msg_time: year.month.day,hour.min.sec.msec

	return:
			NULL  : packed failed
			others: pack ok 
			
	important note: the return ptr must be free later.
*/
static uint8_t *sgmw_pack_pd_info_area(MSG_DATE_TIME_T msg_time ,uint16_t *info_data_len)
{
	uint16_t malloc_len=0;
	uint8_t *malloc_ptr;	//��Ϣ��RAM��
	//uint16_t ptrMsg_len=0;	//��Ϣ��RAM������
	uint8_t *ptr;
	//uint8_t *reserve_ptr_use; 	//just for secretly debug	
	//uint32_t can_data_len=0;
	//int i;
	
	gps_location_info_T 		location={0};
	sgwm_protocol_time_format_t protocol_time={0};
	sgwm_power_voltage_t 		t_power_voltage={0};
	sgwm_gsm_gps_status_t 		t_gsm_gps_status={0};
	sgwm_protocol_gps_format_t	t_gps={0};

	//(1)--------------malloc------------------------------------------------------
	#if 0
	if(tbc_get_report_update_flag())
	{
		can_data_len = tbc_get_report_buf_size();
	}
	#endif

	//--A--date time
		malloc_len += sizeof(sgwm_protocol_time_format_t);
	//--B--voltage
	if(1)
	{
		malloc_len += (1+sizeof(sgwm_power_voltage_t));
	}
	//--C--gsm gps status
	if(1)
	{
		malloc_len += (1+sizeof(sgwm_gsm_gps_status_t));
	}
	//--D--gps location info
	if(gps_check_lacation_state()==1)
	{
		malloc_len += (1+sizeof(sgwm_protocol_gps_format_t));
	}	
	//--E--can data 
	#if 0
	if((0 != tbc_get_report_update_flag()) && (NULL != tbc_report_data_buf_ok()))
	{
		malloc_len += (1+sizeof(sgwm_md5_t)+can_data_len);
	}
	#endif

	//calculate end, then malloc
	malloc_ptr = (uint8_t *)user_malloc(malloc_len,__FUNCTION__);//����
	if(malloc_ptr == NULL)
	{
		//InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
		return NULL;
	}

	//(2)--------------��Э������---------------------------------------------------
	ptr=malloc_ptr; 
	
	//--A--���� & ʱ��
	sgmw_protocol_format_time_for_rep(&protocol_time,msg_time);	
	memcpy(ptr,(uint8_t *)&protocol_time,sizeof(sgwm_protocol_time_format_t));
	ptr += sizeof(sgwm_protocol_time_format_t);

	//--B--��ѹ��־�����ѹ�� (��������Ż�,����ѹ���ϴ��ϱ�û�б仯,�򱾴β����ϱ���ѹ,��ʡ(1+4)�ֽ�)
	*ptr++ = 0x04; //4:-----------------------------------------------------voltage valid flag
	sgmw_protocol_format_pwr_voltage_for_rep(&t_power_voltage);
	memcpy(ptr,(uint8_t *)&t_power_voltage,sizeof(sgwm_power_voltage_t)); 
	ptr += sizeof(sgwm_power_voltage_t);
		
	//--C--״̬��־����gsm��gps״̬
	*ptr++ = 0x03; // 3:----------------------------------------------------gsm,gps flag
	//reserve_ptr_use = ptr;
	sgmw_protocol_format_gsm_gps_sta_for_rep(&t_gsm_gps_status,NULL);
	memcpy(ptr,(uint8_t *)&t_gsm_gps_status,sizeof(sgwm_gsm_gps_status_t)); 
	ptr += sizeof(sgwm_gsm_gps_status_t);
	
	//--D--��λ��ʶ���λ����
	if(gps_check_lacation_state()==1)
	{
		*ptr++ =0x01;//1:---------------------------------------------------gps valid flag
		gps_get_location_info(&location);
		sgmw_protocol_format_gps_info_for_rep(&t_gps,&location);
		memcpy(ptr,(uint8_t *)&t_gps,sizeof(sgwm_protocol_gps_format_t));
		ptr += sizeof(sgwm_protocol_gps_format_t);
	}	

	//--E--CAN������Ч��ʶ����CAN����
	//if((0 != tbc_get_report_update_flag())&&(NULL != tbc_report_data_buf_ok()))
	if(0) //zgc ,not implement can data pack
	{//can data valid
		#if 0
		if(!read_default_tbc_file_md5(md5_t.md5_buf))
		{//md5 valid
			*ptr++ =0x02;//2:-----------------------------------------------can valid flag
			memcpy(ptr,(uint8_t *)md5_t.md5_buf,sizeof(sgwm_md5_t));//md5
			ptr += sizeof(sgwm_md5_t);
			memcpy(ptr,tbc_read_report_data(),can_data_len); zgc ,not implement can data pack
	
			#if 0
			InfoPrintf("���TBC %d bytes:\r\n",can_data_len);
			for(i=0;i<can_data_len;i++){InfoPrintf("%02X,",ptr[i]);
			OSTimeDly(1,OS_OPT_TIME_PERIODIC,&os_err);if(i%10==0&&i!=0){InfoPrintf("\r\n");}
			}OSTimeDly(12,OS_OPT_TIME_PERIODIC,&os_err);InfoPrintf("\r\n");
			#endif
			
			tbc_clear_report_data_buf();
			ptr += can_data_len;
		}
		else
		{
			InfoPrintf("sgmw_report()��ȡMD5ֵʧ��--------------\r\n");
			*reserve_ptr_use |= 0x80;// |0x80    //2016.4.21			
		}

		tbc_clear_report_update_flag();
		tbc_clear_all_valid_flag();
		#endif
	}
	else
	{
		#if 0
		//InfoPrintf("sgmw_report()���β����CAN����--%d--0x%X--\r\n",
		if(tbc_get_report_update_flag()==0)
		{
			*reserve_ptr_use |= 0x40;//|0x40//2016.4.21
		}

		if(tbc_report_data_buf_ok()==NULL)
		{
			*reserve_ptr_use |= 0x20;//|0x20//2016.4.21
		}
		if(get_acc_status()==0)//0 ACC off
		{
			*reserve_ptr_use |= 0x10;//|0x20//2016.5.6
		}
		#endif
	}

	*info_data_len=malloc_len;

	#if 0
	InfoPrintf("PD PACK,INFO LEN=%d:",malloc_len);for(i=0;i<malloc_len;i++)InfoPrintf("%02X,",malloc_ptr[i]);
	InfoPrintf("--\r\n");
	#endif
	
	return malloc_ptr;
}


int sgmw_pack_and_post_pd_rep_info_data_q(void)
{
	OS_ERR os_err=OS_ERR_NONE;
	uint16_t pd_rep_info_len=0;
	uint8_t *ptr=NULL;
	BEIJING_TIME_T  rtcTime={0};
	MSG_DATE_TIME_T msg_time={0};

	int ret=-1;

	if(get_rtc_init_flag()==0)
	{
		return -1;
	}

	get_current_time(&rtcTime);
	#if 0
	rtcTime.year=16;
	rtcTime.month=10;
	rtcTime.day=12;
	rtcTime.hour=21;
	rtcTime.minute=33;
	rtcTime.second=01;
	#endif

	rtcTime.year%=2000;
	
	if((rtcTime.year > 40)||(rtcTime.year < 16))
	{
		InfoPrintf("RTCʱ����� !!!%d\r\n",rtcTime.year);
		
		return -1;
	}
	
	msg_time.year=rtcTime.year;
	msg_time.mon=rtcTime.month;
	msg_time.day=rtcTime.day;
	msg_time.hour=rtcTime.hour;
	msg_time.min=rtcTime.minute;
	msg_time.sec=rtcTime.second;
	
	ptr=sgmw_pack_pd_info_area(msg_time,&pd_rep_info_len);
	if(ptr!=NULL)
	{
		//pack ok		
		OSQPost(&pd_rep_info_data_Q,ptr,pd_rep_info_len,OS_OPT_POST_FIFO,&os_err);
		if(os_err == OS_ERR_NONE)
		{
			ret=0;
			//InfoPrintf("post pd_rep_info_data_Q ok\r\n");
		}
		else
		{
			InfoPrintf("post pd_rep_info_data_Q failed -%d- \r\n",os_err);
		}

		if(ret!=0)
		{
			#if ENABLE_ITV
			OSQPost(&pd_rep_info_data_save_Q,ptr,pd_rep_info_len,OS_OPT_POST_FIFO,&os_err);
			if(os_err == OS_ERR_NONE)
			{
				ret=0;
			}
			#endif
			
			if(ret!=0)
			{
				//����������,�޷����㻺��
				user_free(ptr,__FUNCTION__);
				InfoPrintf("post pd_rep_info_data_save_Q failed -%d- \r\n",os_err);
			}
		}
	}
	else
	{
		InfoPrintf("pack pd package failed \r\n");
	}

	return os_err;
}


#define _____pack_protocol_frame________________________________________________

/*
	brief: pack one total air protocol frame , encrypt it, escape it, 
	       and then return it`s pointer
*/
static uint8_t *sgmw_pack_one_protocol_frame(	uint16_t  msg_id,
												uint8_t  *pdu,
												uint16_t  pdu_len,
												uint16_t *fram_len,
												uint16_t *serial
							  				)
{
	uint16_t package_len=0;//total len
    uint16_t buf_len=0;
	uint8_t *p_dst=NULL;
	uint8_t check_sum;
	sgwm_frame_t *p_protc;
	uint8_t remainder_num=0;//����
	uint8_t i;
	uint16_t  decrypt_info_data_len=0;//������Ϣ�峤��
	uint16_t  encrypt_info_data_len=0;//������Ϣ�峤��
	uint16_t  packed_info_data_len;
	uint8_t aes_encrypt_flag=0;

	if(pdu_len > (16384-15))
	{
		return NULL;
	}

	#if PROTOCOL_V1_0_2_3	
	if(  (msg_id == UP_REQ_RSA_KEY_ID)
	   ||(msg_id == UP_REP_AES_KEY_ID)       		
	  )
	{
		aes_encrypt_flag=0;
	}
	else
	{
		aes_encrypt_flag=1;
	}
	#endif
	
	if(1==aes_encrypt_flag)
	{
		decrypt_info_data_len = pdu_len;
		//InfoPrintf("������Ϣ�峤��%d \r\n",decrypt_info_data_len);
		remainder_num = pdu_len % 16;
		encrypt_info_data_len=pdu_len ;
		if(remainder_num!=0)
		{
			encrypt_info_data_len += (16-remainder_num);
		}
		
		//InfoPrintf("������Ϣ�峤��%d \r\n",encrypt_info_data_len);
		
		//sizeof(sgwm_frame_t)==8;
		//          7E + HEAD + (encrypt data )  + 1(CRC) + 7E
		package_len = 1+6 + encrypt_info_data_len + 1 + 1;	
	}
	else
	{
		decrypt_info_data_len = pdu_len;
		//InfoPrintf("�������,��Ϣ�峤��%d \r\n",decrypt_info_data_len);
		
		//sizeof(sgwm_frame_t)==8;
		//           7E + HEAD+(info data)+ 1(CRC) + 7E
		package_len = 1 + 6 + pdu_len +1 + 1;
	}
	
	//InfoPrintf("���а�����(ת��ǰ)-%d-\r\n",package_len);	
	p_protc = (sgwm_frame_t *)user_malloc(package_len,"sgmw_fram_pack1");//����
	if(p_protc == NULL)
	{
		InfoPrintf("%s,malloc 1  fail\r\n",__FUNCTION__);
		return NULL;
	}

	p_protc->prefix = SGMW_PROTOCOL_FIX;
	p_protc->st_head.msg_id = msg_id;
	p_protc->st_head.st_msg_attr.multi_pack_flag = 0;
	if(1==aes_encrypt_flag)
	{
		p_protc->st_head.st_msg_attr.aes_flag = 1;
	}
	else
	{
		p_protc->st_head.st_msg_attr.aes_flag = 0;
	}
	
	//info data len value write into frame head
	p_protc->st_head.st_msg_attr.pdu_len = pdu_len;
	p_protc->st_head.msg_type=htons(p_protc->st_head.msg_type);
	
	//the real info data bytes amount packed into info_data area of the package
	if((1==aes_encrypt_flag)&&(0 != remainder_num))
	{
		packed_info_data_len = encrypt_info_data_len;	
	}
	else
	{	
		packed_info_data_len=decrypt_info_data_len;
	}

	if(serial!=NULL)
	{
		p_protc->st_head.serial = get_serial_num();
    	*serial = p_protc->st_head.serial;
	}
	else
	{
		p_protc->st_head.serial = 0;
	}
	
    if(1==aes_encrypt_flag)
	{		
		memcpy(p_protc->pdu,pdu,decrypt_info_data_len);  	//������Ϣ��
		if(remainder_num != 0)
		{
			for(i=0;i<(16-remainder_num);i++)				//����
			{
				p_protc->pdu[decrypt_info_data_len+i] = 0x00;
			}		
		}
		AesEncryptData(p_protc->pdu,packed_info_data_len);
	}
	else
	{
		memcpy(p_protc->pdu,pdu,packed_info_data_len);  	//������Ϣ��
	}

	check_sum = get_check_sum((uint8_t *)&p_protc->st_head,package_len - 3);//-fix- pdu
	memset(p_protc->pdu + packed_info_data_len,check_sum,1); //check_byte
	memset(p_protc->pdu + packed_info_data_len + 1,SGMW_PROTOCOL_FIX,1);//��β0X7E

	buf_len = check_escape_bytes_amount((uint8_t *)&p_protc->st_head,(package_len - 2)) + package_len;
	//InfoPrintf("ת��󳤶� %d \r\n",buf_len);
	p_dst = (uint8_t *)user_malloc(buf_len,__FUNCTION__);
	if(p_dst == NULL)
	{
		InfoPrintf("%s,malloc 2 fail\r\n",__FUNCTION__);
		user_free(p_protc,__FUNCTION__);
		return NULL;
	}

	*fram_len = sgwm_escape_package(p_dst,(uint8_t *)p_protc,package_len);

	user_free(p_protc,__FUNCTION__);

	return p_dst;
}


#define _____pack_app_frame___________________________________________


static int sgmw_wait_report_ack_q(uint16_t tx_serial)
{
	remote_result_q_t *p_msg;
	uint16_t pend_msg_len;
	uint8_t noAck_errAck_cnt=0;
	OS_ERR os_err;
	int ret=-2;
		
	while(1)
	{
		if(noAck_errAck_cnt < 2)
		{
			p_msg = (remote_result_q_t *)OSQPend(&rx_server_ack_q,T_5S,OS_OPT_PEND_BLOCKING,&pend_msg_len,NULL,&os_err);		
			if(NULL != p_msg)
			{
				if(p_msg->serial == tx_serial) 
				{
					if(p_msg->result==0)
					{
						//InfoPrintf("report,ack ok,S=0x%04X\r\n",tx_serial);
					}
					else
					{
						InfoPrintf("gprs rep,rx ack,S:%04X,R:%04X,but result=%u,take it as right rep...\r\n",tx_serial,p_msg->serial, p_msg->result);
					}
					
					ret = 0;
					noAck_errAck_cnt=0;
					//�����ˮ�Ŷ���,��ֱ�ӷ��ر��ζ�ʱ�ϱ�OK������ƽ̨������result
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
				noAck_errAck_cnt++;
				continue;
			}
		}
		else
		{
			InfoPrintf("gprs rep,ack fail,re_create socket!!!!\r\n"); 
			noAck_errAck_cnt=0;
			modem_ReCreat_socket();
		}

		break;
	}

	return ret;
}

/*
	input:	
			info_data_ptr: point to the info_data RAM
			info_data_len: info_data length
			
	return:
			   0  : successful,    report and received ACK ok.
			others: report failed, need to save ITV
*/
int sgmw_pack_and_rep_pd_frame(uint8_t *info_data_ptr,uint16_t info_data_len)
{
    OS_ERR os_err;
	int ret = -1;
	uint16_t serial;
	uint8_t *package_ptr=NULL;
	uint16_t package_len;
	uint32_t temp;
    //uint32_t i;

	temp=OSTimeGet(&os_err);

	package_ptr = sgmw_pack_one_protocol_frame(UP_REPORT_MSG_ID,(uint8_t*)(info_data_ptr),info_data_len,&package_len,&serial);
	if(package_ptr == NULL)
	{
		return -1;
	}

	ret=-2;

	#if 0
	InfoPrintf("\r\n\r\n��ʱ�ϱ�(%d):",package_len);
	for(i=0;i<package_len;i++)InfoPrintf("%02X,",package_ptr[i]);InfoPrintf("\r\n");
	#endif

	if(modem_send_net_data(package_ptr,(uint32_t)package_len)>0)
	{
		ret=sgmw_wait_report_ack_q(serial);
	}
	else
	{
	   InfoPrintf("pd rep,modem send failed,S=0x%04X !!!!!!!!!\r\n",serial);
	} 

	user_free(package_ptr,__FUNCTION__);
	if(ret==0)
	{
		InfoPrintf("pd rep ok,  S=0x%04X used   time=%d ms\r\n",serial,OSTimeGet(&os_err)-temp);
	}
	else
	{
		InfoPrintf("pd rep fail,S=0x%04X used  time=%d ms !!!!\r\n",serial,OSTimeGet(&os_err)-temp);
	}
	printf_time_now();

	return ret;
}

int sgmw_pack_and_rep_req_rsa_key(void)
{
    OS_ERR os_err;
	int ret = -1;
	uint16_t serial;	
	uint8_t *package_ptr=NULL;
	uint16_t package_len;
	uint32_t temp;

	temp=OSTimeGet(&os_err);

	package_ptr = sgmw_pack_one_protocol_frame(UP_REQ_RSA_KEY_ID,NULL,0,&package_len,&serial);
	if(package_ptr == NULL)
	{
		return -1;
	}

	ret=-2;

	if(modem_send_net_data(package_ptr,(uint32_t)package_len)>0)
	{		
		ret=sgmw_wait_report_ack_q(serial);
	}
	else
	{
	   InfoPrintf("req rsa key rp,modem send fail,S=0x%04X !!!!!!!!!\r\n",serial);
	} 

	user_free(package_ptr,__FUNCTION__);
	if(ret==0)
	{
		InfoPrintf("req RSA key ok, used   time=%d ms\r\n",OSTimeGet(&os_err)-temp);
	}
	else
	{
		InfoPrintf("req RSA key fail,used  time=%d ms !!!!\r\n",OSTimeGet(&os_err)-temp);
	}

	return ret;
}


/*
	return:
			0: ok
			1: fail
*/
int sgmw_pack_and_rep_aes_key(uint8_t *aes_key,uint8_t key_len)
{
	//CPU_SR_ALLOC();
    OS_ERR os_err;
	int ret = -1;
	uint16_t serial;	
	uint8_t *package_ptr=NULL;
	uint16_t package_len;
	uint32_t temp;
	//uint8_t i;
	uint8_t aes_key_str[34]={0};
	uint8_t aes_key_rsa_en_hex[128+32]={0};
	uint8_t aes_key_rsa_en_len;

	unsigned char *ptr; 

	hex_to_string(aes_key,16,aes_key_str,(uint16_t *) &temp);

	InfoPrintf("AES KEY(string):%s\r\n",aes_key_str);

	ptr=rsa_encrypt_data(aes_key_str,RSA_E_exponent_str,RSA_N_modulus_str);
	if(ptr!=NULL)
	{
		//InfoPrintf("RSA��Կ����AES��Կ: %s---------\r\n", ptr);
		
	}
	
	string_to_hex(ptr,aes_key_rsa_en_hex,(uint16_t *)&aes_key_rsa_en_len);
	
	#if 0
	//CPU_CRITICAL_ENTER();
	InfoPrintf("AES KEY RSA DATA(%d):",aes_key_rsa_en_len);
	for(i=0;i<aes_key_rsa_en_len;i++)InfoPrintf("%02X,",aes_key_rsa_en_hex[i]);
	InfoPrintf("\r\n");
	InfoPrintf("----\r\n");
	//CPU_CRITICAL_EXIT();
	#endif
	
	temp=OSTimeGet(&os_err);

	package_ptr = sgmw_pack_one_protocol_frame(UP_REP_AES_KEY_ID,aes_key_rsa_en_hex,128,&package_len,&serial);
	if(package_ptr == NULL)
	{
		return -1;
	}

	ret=-2;
	
	#if 0
	//CPU_CRITICAL_ENTER();
	InfoPrintf("AES��Կ�ϱ�(RSA����)(%d bytes):",package_len);
	for(i=0;i<package_len;i++)
	{
		InfoPrintf("%02X-",package_ptr[i]);
	}
	InfoPrintf("\r\n");
	//CPU_CRITICAL_EXIT();
	#endif
	
	if(modem_send_net_data(package_ptr,(uint32_t)package_len)>0)
	{		
		ret=sgmw_wait_report_ack_q(serial);
	}
	else
	{
	   InfoPrintf("AES key rp,modem send fail,S=0x%04X !!!!!!!!!\r\n",serial);
	} 

	user_free(package_ptr,__FUNCTION__);
	if(ret==0)
	{
		InfoPrintf("AES key rp ok, used   time=%d ms\r\n",OSTimeGet(&os_err)-temp);
	}
	else
	{
		InfoPrintf("AES key rp fail,used  time=%d ms !!!!\r\n",OSTimeGet(&os_err)-temp);
	}

	return ret;
}


/****************************** zgc
������:sgmw_pack_and_rep_login_frame
����	:�ͻ���Ȩ��
����ֵ:-1ʧ��
*******************************/
int sgmw_pack_and_rep_login_frame(void)
{
	//CPU_SR_ALLOC();
	//uint16_t i;
	const uint8_t authorize_key[16]={0x0c,0x5f,0xab,0x49,0xf4,0xa0,0xc4,0x85,0xce,0x71,0xca,0xe1,0x98,0x56,0xa0,0x25};
	//OS_ERR os_err;
	int ret = -1;
	uint8_t *p_fram;
	sgwm_login_msg_info_data_cfg_t login_msg_info_data_config={0};
	uint8_t *login_msg_info_data_ptr;
	uint16_t login_msg_info_data_len;
	uint16_t offset_len;
	uint32_t temp;

	uint16_t login_msg_size=0;
    uint16_t serial=0;


	//----------------------------------------------------------------
	//config login msg info data`s content flag byte
	//if(already_login_info_flag==0)
	{
		login_msg_info_data_config.ct_flag_bit.TM_barcode_flag=1;
		login_msg_info_data_config.ct_flag_bit.SIM_sn_flag=1;
		login_msg_info_data_config.ct_flag_bit.CCID_sn_flag=1;
		login_msg_info_data_config.ct_flag_bit.Vin_sn_flag=1;
	}
	
	//calculate info_data length
	login_msg_info_data_len =SGMW_DEV_ID_LEN;					//dev_id len
	login_msg_info_data_len+=SGMW_AUTHORIZE_TYPE_LEN;			//terminal 
	login_msg_info_data_len+=SGMW_AUTHORIZE_KEY_LEN;			//authorize code
	
	#if 1 //PROTOCOL_V1_0_2_3
	login_msg_info_data_len+=SGMW_AUTHORIZE_CONTENT_FLAG_LEN;	//content flag
	
	if(login_msg_info_data_config.ct_flag_bit.TM_barcode_flag==1)
	{
		login_msg_info_data_len+=(1+SGMW_TM_BARCODE_LEN);
	}
	if(login_msg_info_data_config.ct_flag_bit.SIM_sn_flag==1)
	{
		login_msg_info_data_len+=(1+SGMW_SIM_SN_LEN);
	}
	if(login_msg_info_data_config.ct_flag_bit.CCID_sn_flag==1)
	{
		login_msg_info_data_len+=(1+SGMW_CCID_SN_LEN);
	}
	if(login_msg_info_data_config.ct_flag_bit.Vin_sn_flag==1)
	{
		login_msg_info_data_len+=(1+SGMW_VIN_LEN);
	}
	#endif
	
    login_msg_info_data_ptr = (uint8_t *)user_malloc(login_msg_info_data_len,__FUNCTION__);
	if(login_msg_info_data_ptr==NULL)
	{
		InfoPrintf("login:���䳤��--%d-----ʧ��-\r\n",login_msg_info_data_len);
		return -1;
	}
	//InfoPrintf("login:���䳤��--%d----�ɹ�--\r\n",login_msg_info_data_len);
	
	//start...
	offset_len=0;
	
	//dev id
	temp=sgmw_get_dev_id();
	//InfoPrintf("�豸ID,ptr=%d\r\n",offset_len);
	memcpy(&login_msg_info_data_ptr[offset_len],(uint8_t *)&temp,4);
	offset_len+=SGMW_DEV_ID_LEN;

	//type
	login_msg_info_data_ptr[offset_len]=1;
	offset_len+=SGMW_AUTHORIZE_TYPE_LEN;

	//authorize code
	memcpy(&login_msg_info_data_ptr[offset_len],authorize_key,16);
	offset_len+=SGMW_AUTHORIZE_KEY_LEN;
	
	#if 1 //PROTOCOL_V1_0_2_3
	//login msg content flag byte
	login_msg_info_data_ptr[offset_len]=*((uint8_t*)&login_msg_info_data_config);
	offset_len+=SGMW_AUTHORIZE_CONTENT_FLAG_LEN;
	
	//barcode
	if(login_msg_info_data_config.ct_flag_bit.TM_barcode_flag==1)
	{
		temp=SGMW_TM_BARCODE_LEN;
		memcpy(&login_msg_info_data_ptr[offset_len],(uint8_t *)&temp,1);
		offset_len+=1;
		//InfoPrintf("����,ptr=%d\r\n",offset_len);
		
		memcpy(&login_msg_info_data_ptr[offset_len],rd_barcode(),SGMW_TM_BARCODE_LEN);
		offset_len+=SGMW_TM_BARCODE_LEN;
	}
	//sim 
	if(login_msg_info_data_config.ct_flag_bit.SIM_sn_flag==1)
	{
		temp=SGMW_SIM_SN_LEN;
		memcpy(&login_msg_info_data_ptr[offset_len],(uint8_t *)&temp,1);
		offset_len+=1;
		//InfoPrintf("SIM����,ptr=%d\r\n",offset_len);
		memcpy(&login_msg_info_data_ptr[offset_len],sgmw_tm_sim,SGMW_SIM_SN_LEN);
		offset_len+=SGMW_SIM_SN_LEN;
	}
	//ccid
	if(login_msg_info_data_config.ct_flag_bit.CCID_sn_flag==1)
	{
		temp=SGMW_CCID_SN_LEN;
		memcpy(&login_msg_info_data_ptr[offset_len],(uint8_t *)&temp,1);
		offset_len+=1;
		//InfoPrintf("CCID,ptr=%d\r\n",offset_len);
		memcpy(&login_msg_info_data_ptr[offset_len],get_sim_ccid(),SGMW_CCID_SN_LEN);
		offset_len+=SGMW_CCID_SN_LEN;
	}
	//vin
	if(login_msg_info_data_config.ct_flag_bit.Vin_sn_flag==1)
	{
		temp=SGMW_VIN_LEN;
		memcpy(&login_msg_info_data_ptr[offset_len],(uint8_t *)&temp,1);
		offset_len+=1;
		
		InfoPrintf("VIN,ptr=%d\r\n",offset_len);
		memcpy(&login_msg_info_data_ptr[offset_len],sgmw_tm_vin,SGMW_VIN_LEN);
		
		offset_len+=SGMW_VIN_LEN;
	}
	#endif
	if(login_msg_info_data_len!=offset_len)
	{
		InfoPrintf("���ȴ���------\r\n");
	}
	
	#if 0
	InfoPrintf("login:��Ϣ�峤��--%d---...���...\r\n",login_msg_info_data_len);
	for(i=0;i<login_msg_info_data_len;i++)
	{
		InfoPrintf("%02X,",login_msg_info_data_ptr[i]);
	}
	InfoPrintf("-------\r\n");
	#endif
	
	//malloc ram
	p_fram = sgmw_pack_one_protocol_frame(UP_LOGIN_MSG_ID,login_msg_info_data_ptr,
                            login_msg_info_data_len,&login_msg_size,&serial);
	if(p_fram == NULL)
	{		
		user_free(login_msg_info_data_ptr,__FUNCTION__);
		return -1;
	}

	#if 0
	InfoPrintf("��Ȩ��(%d):",login_msg_size);
	for(i=0;i<login_msg_size;i++)
	{
		InfoPrintf("%02X,",p_fram[i]);
	}
	InfoPrintf("\r\n");
	#endif

	ret = -2;
	//0000202134312545656786654rbfwgejutyityutyrterwwwert
	if(modem_send_net_data(p_fram,login_msg_size)>0)
	//if(modem_send_net_data(jhn_test_str,strlen(jhn_test_str))>0)
	{
		ret=sgmw_wait_report_ack_q(serial);
	}
	else
	{
		InfoPrintf("login,modem send fail !!!\r\n");
	}
	
	user_free(login_msg_info_data_ptr,__FUNCTION__);
	user_free(p_fram,__FUNCTION__);	

	if(ret==0)
	{
		InfoPrintf("sgmw:login ok,-----ID:%d-----\r\n",sgmw_get_dev_id());
	}

	return ret;
}

int sgmw_pack_and_rep_logout_frame(void)
{

    return 0;
}

/*
	input:
			info_data_ptr: packed info data, not include frame HEAD,CRC,TAIL
			info_data_len: length
	return:
				0 : save ok
			others: save failed
*/
int sgmw_save_itv(uint8_t *info_data_ptr,uint32_t info_data_len)
{
	return 0;
}


/*
	return:
			0		: report ok
			others	: report failed 
*/
int sgmw_pack_and_rep_itv_frame(uint8_t *data, uint16_t data_len)
{
	OS_ERR os_err;
    uint16_t package_len=0;
	uint8_t *package_ptr;
	//int i;
    uint16_t serial;
	int ret = -1;
	uint32_t temp;

	temp=OSTimeGet(&os_err);
	data[0] |= 0x80;//������־
	package_ptr = sgmw_pack_one_protocol_frame(UP_REPORT_MSG_ID,data,data_len,&package_len,&serial);
	if(package_ptr == NULL)
	{
		return -1;
	}

	ret=-2;
	if(modem_send_net_data(package_ptr,package_len)>0)
	{
		ret=sgmw_wait_report_ack_q(serial);
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
	return ret;
}



void sgmw_read_and_rep_itv(void)
{
	//read itv data
	//...
	
	//pack and rep
	sgmw_pack_and_rep_itv_frame(NULL,0);
}

#define ____general_air_communication__________________________________________

/***************************************
������:normal_ack_to_server
����	:ͨ��Ӧ���
����	:weicheng 2015.11.24
****************************************/
int  normal_ack_to_server(uint16_t serial,uint16_t msg_id,uint8_t result)
{
	//	OS_ERR os_err;
	uint16_t fram_len;
	uint16_t len;
	uint8_t *p_fram;
	uint8_t *ptrMsg;
	uint16_t ptrMsg_len=0;
	uint8_t *ptr;
    sgwm_general_ack_t ack_pack_t={0};
	uint16_t serial_t;
	int ret = -1;
        
	len = sizeof(sgwm_general_ack_t);//������Ϣ���ڴ�ռ䳤��
	ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
	if(ptrMsg == NULL)
	{
		InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
		return -1;
	}

	ack_pack_t.serial = serial;  //Ӧ����ˮ��
	ack_pack_t.msg_id = msg_id;  //Ӧ��ID
	ack_pack_t.result = result;  //���
	
    ptr = ptrMsg;
    memcpy(ptr,(uint8_t *)&ack_pack_t,sizeof(sgwm_general_ack_t));
    ptr += sizeof(sgwm_general_ack_t);
	
	ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
	p_fram = sgmw_pack_one_protocol_frame(GENERAL_ACK_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
    
    user_free(ptrMsg,__FUNCTION__);

	if(p_fram == NULL)
	{
		return -1;
	}
    //send====
    if(modem_send_net_data(p_fram,fram_len)>0)
	{
		//DebugPrintf(REMOTE_DBG,"normal_ack_to_server, ack ok\r\n");
		ret = 0;	
	}
	else
	{
	   //DebugPrintf(REMOTE_DBG,"normal_ack_to_server, ack failed !!!!!!!!!\r\n");
	   //SAVE
	} 
		
    user_free(p_fram,__FUNCTION__);
     return ret;
}
/***************************************
������:query_default_dbcfile_ack
����	:�鿴Ĭ��DBC�ļ�Ӧ��
����	:weicheng 2015.11.24
****************************************/
int  query_default_dbcfile_ack(uint16_t serial,uint8_t *file_name,uint8_t file_len)
{
	//	OS_ERR os_err;
	uint16_t fram_len;
	uint16_t len;
	uint8_t *p_fram;
	uint8_t *ptrMsg;
	uint16_t ptrMsg_len=0;
	uint8_t *ptr;
	uint8_t i;
	uint16_t serial_t;
    int ret = -1;
	
	len = file_len+2;//������Ϣ���ڴ�ռ䳤��
	ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
	if(ptrMsg == NULL)
	{
		InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
		return -1;
	}

    ptr = ptrMsg;
	
	*ptr++ = serial;  //Ӧ����ˮ��
	*ptr++ = serial>>8;

	for(i=0;i<file_len;i++) //�ļ���
	{
       *ptr++ = *(file_name+i);
	}
			
	ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
	p_fram = sgmw_pack_one_protocol_frame(UP_GET_DEFAULT_DBC_ACK_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
    user_free(ptrMsg,__FUNCTION__);
	if(p_fram == NULL)
	{
		return -1;
	}
    //send====
	if(modem_send_net_data(p_fram,fram_len)>0)
	{
		
		//DebugPrintf(REMOTE_DBG,"query_default_dbcfile_ack, ack ok\r\n");
		ret = 0;		
	}
	else
	{
	    //DebugPrintf(REMOTE_DBG,"query_default_dbcfile_ack, ack failed !!!!!!!!!\r\n");
	} 
	
    user_free(p_fram,__FUNCTION__);
    return ret;

}

/****************************************
������:query_dbcfile_list_ack
����	:�鿴DBC�ļ��б�Ӧ��
����:weicheng 2015.11.25
*****************************************/
int  query_dbcfile_list_ack(uint16_t serial,uint8_t *file_list,uint16_t file_list_len)
{
//	OS_ERR os_err;
	uint16_t fram_len;
	uint16_t len=0;
	uint8_t *p_fram;
	uint8_t *ptrMsg;
	uint16_t ptrMsg_len=0;
	uint8_t *ptr;
	uint16_t serial_t;
	int ret = -1;
    uint16_t i;
            
	len = file_list_len+2;//������Ϣ���ڴ�ռ䳤��
	ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
	if(ptrMsg == NULL)
	{
		InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
		return -1;
	}

	ptr = ptrMsg;
	
	*ptr++ = serial;  //Ӧ����ˮ��
	*ptr++ = serial>>8;

	for(i=0;i<file_list_len;i++) //
	{
	   *ptr++ = *(file_list+i);
	}
	
	ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
	p_fram = sgmw_pack_one_protocol_frame(UP_GET_DBC_LIST_ACK_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
	user_free(ptrMsg,__FUNCTION__);
	if(p_fram == NULL)
	{
		return -1;
	}
	//send====
	if(modem_send_net_data(p_fram,fram_len)>0)
	{
		
		InfoPrintf("query_dbcfile_list_ack, ack ok\r\n");
		ret = 0;		
	}
	else
	{
	   InfoPrintf("query_dbcfile_list_ack, ack failed !!!!!!!!!\r\n");
	} 
	
	user_free(p_fram,__FUNCTION__);
	return ret;
}

/***************************************
������:device_system_version_ack
����	:��ϵͳ�汾��Ӧ��
����	:weicheng 2015.11.26
****************************************/

int  device_system_version_ack(uint16_t serial,const char *version)
{
	#define VER_LEN_C 49//(32+1+32)
	#define SPACE_LEN_C (60)
//	OS_ERR os_err;
	uint16_t fram_len;
	uint16_t len=0;
	uint8_t *p_fram;
	uint8_t *ptrMsg;
	uint16_t ptrMsg_len=0;
	uint8_t *ptr;	
	#if ENABLE_RM_DBG
	//uint8_t *ptr_dbg=NULL;
	#endif
	uint16_t i;
	uint16_t serial_t;
	uint16_t version_len;
    int ret = -1;
	
	version_len = VER_LEN_C;//strlen((const char *)version);//������Ϣ���ڴ�ռ䳤��
	
	#if ENABLE_RM_DBG
	version_len+=(RM_DBG_BUF_LEN+SPACE_LEN_C);
	#endif
	
	len = version_len + 2;
	
	InfoPrintf("ver string len:%d\r\n",len);

	ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
	if(ptrMsg == NULL)
	{
		InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
		return -1;
	}
	
	#if ENABLE_RM_DBG
	memset(ptrMsg,' ', len);
	#endif	

    ptr = ptrMsg;
	
	*ptr++ = serial;  //Ӧ����ˮ��
	*ptr++ = serial>>8;

	#if ENABLE_RM_DBG
	InfoPrintf("ʵ�ʰ汾����:%d\r\n",(version_len-(RM_DBG_BUF_LEN+SPACE_LEN_C)));
	for(i=0;i<(version_len-(RM_DBG_BUF_LEN+SPACE_LEN_C));i++) //�汾��
	{
       *ptr++ = *(version+i);
	}
	#else
	for(i=0;i<version_len;i++) //�汾��
	{
       *ptr++ = *(version+i);
	}
	#endif

	#if ENABLE_RM_DBG
	ptr+= SPACE_LEN_C;
	ret=read_rm_dbg((ptrMsg+VER_LEN_C+SPACE_LEN_C+2),RM_DBG_BUF_LEN);
	ptr+=ret;
	InfoPrintf("��������:%d\r\n",ret);
	//for(i=VER_LEN_C+2+SPACE_LEN_C;i<(VER_LEN_C+2+SPACE_LEN_C+ret);i++)InfoPrintf("%c",ptrMsg[i]);
	//InfoPrintf("���\r\n");
	#endif
			
	ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
	p_fram = sgmw_pack_one_protocol_frame(UP_GET_VER_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
    user_free(ptrMsg,__FUNCTION__);
	if(p_fram == NULL)
	{
		InfoPrintf("report ver failed !!!\r\n",ret);
		return -1;
	}
    //send====
	if(modem_send_net_data(p_fram,fram_len)>0)
	{
		//DebugPrintf(REMOTE_DBG,"device_system_version_ack, ack ok\r\n");
		ret = 0;		
	}
	else
	{
	   //DebugPrintf(REMOTE_DBG,"device_system_version_ack, ack failed !!!!!!!!!\r\n");
	} 

	
    user_free(p_fram,__FUNCTION__);
    return ret;

}


/***************************************
������:postback_frequency_ack
����	:�鿴�ش�Ƶ��Ӧ��
����	:weicheng 2015.11.26
****************************************/
int  postback_frequency_ack(uint16_t serial,uint32_t postback_frq)
{
	//	OS_ERR os_err;
	uint16_t fram_len;
	uint16_t len=0;
	uint8_t *p_fram;
	uint8_t *ptrMsg;
	uint16_t ptrMsg_len=0;
	uint8_t *ptr;
	uint16_t serial_t;
     int ret = -1;
	
	len = 4 + 2;//������Ϣ���ڴ�ռ䳤��
	ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
	if(ptrMsg == NULL)
	{
		InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
		return -1;
	}
    ptr = ptrMsg;
	
	*ptr++ = serial;  //Ӧ����ˮ��
	*ptr++ = serial>>8;
    //ptr += 2;

    memcpy(ptr,&postback_frq,4);//�ش�Ƶ��
    ptr += 4;
			
	ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
	p_fram = sgmw_pack_one_protocol_frame(UP_GET_REPORT_PERIOD_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
    user_free(ptrMsg,__FUNCTION__);
	if(p_fram == NULL)
	{
		return -1;
	}
    //send====
	if(modem_send_net_data(p_fram,fram_len)>0)
	{
		
		//DebugPrintf(REMOTE_DBG,"postback_frequency_ack, ack ok\r\n");
		ret = 0;		
	}
	else
	{
	   //DebugPrintf(REMOTE_DBG,"postback_frequency_ack, ack failed !!!!!!!!!\r\n");
	} 
	

    user_free(p_fram,__FUNCTION__);
    return ret;

}

/***************************************
������:heartbeat_frequency_ack
����	:�鿴����Ƶ��Ӧ��
����	:weicheng 2015.11.26
****************************************/
int  heartbeat_frequency_ack(uint16_t serial,uint32_t heartbeat_frq)
{
//	OS_ERR os_err;
	uint16_t fram_len;
	uint16_t len=0;
	uint8_t *p_fram;
	uint8_t *ptrMsg;
	uint16_t ptrMsg_len=0;
	uint8_t *ptr;
	uint16_t serial_t;
    int ret = -1;
	
	len = 4 + 2;//������Ϣ���ڴ�ռ䳤��
	ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
	if(ptrMsg == NULL)
	{
		InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
		return -1;
	}

    ptr = ptrMsg;
	
	*ptr++ = serial;  //Ӧ����ˮ��
	*ptr++ = serial>>8;

    memcpy(ptr,&heartbeat_frq,4);//����Ƶ��
    ptr += 4;
			
	ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
	p_fram = sgmw_pack_one_protocol_frame(UP_GET_HEARTBEAT_ACK_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
    user_free(ptrMsg,__FUNCTION__);
	if(p_fram == NULL)
	{
		return -1;
	}
    //send====
	if(modem_send_net_data(p_fram,fram_len)>0)
	{
		
		//DebugPrintf(REMOTE_DBG,"heartbeat_frequency_ack, ack ok\r\n");
		ret = 0;		
	}
	else
	{
	   //DebugPrintf(REMOTE_DBG,"heartbeat_frequency_ack, ack failed !!!!!!!!!\r\n");
	} 	

	
    user_free(p_fram,__FUNCTION__);
    return ret;

}
/***************************************
������:postback_type_ack
����	:�ش���ʽӦ��
����	:weicheng 2015.11.26
****************************************/
int  postback_type_ack(uint16_t serial,uint8_t postback_type)
{
	//	OS_ERR os_err;
		uint16_t fram_len;
		uint16_t len=0;
		uint8_t *p_fram;
		uint8_t *ptrMsg;
		uint16_t ptrMsg_len=0;
		uint8_t *ptr;
		uint16_t serial_t;
	    int ret = -1;
		
		len = 1 + 2;//������Ϣ���ڴ�ռ䳤��
		ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
		if(ptrMsg == NULL)
		{
			InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
			return -1;
		}
	    ptr = ptrMsg;
		
		*ptr++ = serial;  //Ӧ����ˮ��
		*ptr++ = serial>>8;

        *ptr++ = postback_type;//�ش���ʽ
        			
		ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
		p_fram = sgmw_pack_one_protocol_frame(UP_GET_USE_FILE_TYPE_ACK_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
        user_free(ptrMsg,__FUNCTION__);
		if(p_fram == NULL)
		{
			return -1;
		}
        //send====
		if(modem_send_net_data(p_fram,fram_len)>0)
		{
			
			//DebugPrintf(REMOTE_DBG,"postback_type_ack, ack ok\r\n");
			ret = 0;		
		}
		else
		{
		   //DebugPrintf(REMOTE_DBG,"postback_type_ack, ack failed !!!!!!!!!\r\n");
		} 	

    
        user_free(p_fram,__FUNCTION__);
        return ret;

}

/***************************************
������:sd_save_status_ack
����	:�鿴�Ƿ񱣴�SD��Ӧ��
����	:weicheng 2015.11.26
****************************************/
int  sd_save_status_ack(uint16_t serial,uint8_t sd_save_status)
{
	//	OS_ERR os_err;
		uint16_t fram_len;
		uint16_t len=0;
		uint8_t *p_fram;
		uint8_t *ptrMsg;
		uint16_t ptrMsg_len=0;
		uint8_t *ptr;
		uint16_t serial_t;
	    int ret = -1;
		
		len = 1 + 2;//������Ϣ���ڴ�ռ䳤��
		ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
		if(ptrMsg == NULL)
		{
			InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
			return -1;
		}
	    ptr = ptrMsg;
		
		*ptr++ = serial;  //Ӧ����ˮ��
		*ptr++ = serial>>8;

        *ptr++ = sd_save_status;//SD���Ƿ񱣴�״̬
        			
		ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
		p_fram = sgmw_pack_one_protocol_frame(UP_GET_SAVE_DATA_EN_ACK_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
        user_free(ptrMsg,__FUNCTION__);
		if(p_fram == NULL)
		{
			return -1;
		}
        //send====
		if(modem_send_net_data(p_fram,fram_len)>0)
		{
			
			//DebugPrintf(REMOTE_DBG,"sd_save_status_ack, ack ok\r\n");
			ret = 0;		
		}
		else
		{
		  // DebugPrintf(REMOTE_DBG,"sd_save_status_ack, ack failed !!!!!!!!!\r\n");
		} 	
		
        user_free(p_fram,__FUNCTION__);
        return ret;

}
/***************************************
������:sd_save_time_ack
����	:SD������ʱ��Ӧ��
����	:weicheng 2015.11.26
****************************************/
int  sd_save_time_ack(uint16_t serial,uint32_t sd_save_time)
{
	//	OS_ERR os_err;
		uint16_t fram_len;
		uint16_t len=0;
		uint8_t *p_fram;
		uint8_t *ptrMsg;
		uint16_t ptrMsg_len=0;
		uint8_t *ptr;
		uint16_t serial_t;
	    int ret = -1;
		
		len = 4 + 2;//������Ϣ���ڴ�ռ䳤��
		ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
		if(ptrMsg == NULL)
		{
			InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
			return -1;
		}
	    ptr = ptrMsg;
		
		*ptr++ = serial;  //Ӧ����ˮ��
		*ptr++ = serial>>8;

        memcpy(ptr,&sd_save_time,4);//Ӧ���� ʱ����
        ptr += 4;
				
		ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
		p_fram = sgmw_pack_one_protocol_frame(UP_GET_SAVE_DATA_TIME_ACK_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
        user_free(ptrMsg,__FUNCTION__);
		if(p_fram == NULL)
		{
			return -1;
		}
        //send====
		if(modem_send_net_data(p_fram,fram_len)>0)
		{
			
			//DebugPrintf(REMOTE_DBG,"sd_save_time_ack, ack ok\r\n");
			ret = 0;		
		}
		else
		{
		   //DebugPrintf(REMOTE_DBG,"sd_save_time_ack, ack failed !!!!!!!!!\r\n");
		} 	
		
        user_free(p_fram,__FUNCTION__);
        return ret;

}
/***************************************
������:sd_exist_status_ack
����	:�鿴SD��״̬Ӧ��
����	:weicheng 2015.11.26
****************************************/
int  sd_exist_status_ack(uint16_t serial,uint8_t sd_exist_status)
{
	//	OS_ERR os_err;
		uint16_t fram_len;
		uint16_t len=0;
		uint8_t *p_fram;
		uint8_t *ptrMsg;
		uint16_t ptrMsg_len=0;
		uint8_t *ptr;
		uint16_t serial_t;
	    int ret = -1;
		
		len = 1 + 2;//������Ϣ���ڴ�ռ䳤��
		ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
		if(ptrMsg == NULL)
		{
			InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
			return -1;
		}

	    ptr = ptrMsg;
		
		*ptr++ = serial;  //Ӧ����ˮ��
	    *ptr++ = serial>>8;

        *ptr++ = sd_exist_status;//SD��״̬
        			
		ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
		p_fram = sgmw_pack_one_protocol_frame(UP_GET_SD_STATE_ACK_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
        user_free(ptrMsg,__FUNCTION__);
		if(p_fram == NULL)
		{
			return -1;
		}
        //send====
   		if(modem_send_net_data(p_fram,fram_len)>0)
		{
			
			//DebugPrintf(REMOTE_DBG,"sd_exist_status_ack, ack ok\r\n");
			ret = 0;		
		}
		else
		{
		   //DebugPrintf(REMOTE_DBG,"sd_exist_status_ack, ack failed !!!!!!!!!\r\n");
		} 	
		
        user_free(p_fram,__FUNCTION__);
        return ret;

}
/***************************************
������:sd_total_size_ack
����	:�鿴SD���ܴ�С��Ӧ��
����	:weicheng 2015.11.26
****************************************/
int  sd_total_size_ack(uint16_t serial,uint32_t sd_total_size)
{
	//	OS_ERR os_err;
		uint16_t fram_len;
		uint16_t len=0;
		uint8_t *p_fram;
		uint8_t *ptrMsg;
		uint16_t ptrMsg_len=0;
		uint8_t *ptr;
		uint16_t serial_t;
	    int ret = -1;
		
		len = 4 + 2;//������Ϣ���ڴ�ռ䳤��
		ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
		if(ptrMsg == NULL)
		{
			InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
			return -1;
		}
	    ptr = ptrMsg;
		
		*ptr++ = serial;  //Ӧ����ˮ��
	    *ptr++ = serial>>8;

        memcpy(ptr,&sd_total_size,4);//SD���ܴ�С
        ptr += 4;
				
		ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
		p_fram = sgmw_pack_one_protocol_frame(UP_GET_SD_VOL_ACK_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
        user_free(ptrMsg,__FUNCTION__);
		if(p_fram == NULL)
		{
			return -1;
		}
        //send====
        if(modem_send_net_data(p_fram,fram_len)>0)
		{
			
			//DebugPrintf(REMOTE_DBG,"sd_total_size_ack, ack ok\r\n");
			ret = 0;		
		}
		else
		{
		   //DebugPrintf(REMOTE_DBG,"sd_total_size_ack, ack failed !!!!!!!!!\r\n");
		}  
		
        user_free(p_fram,__FUNCTION__);
        return ret;

}
/***************************************
������:sd_used_size_ack
����	:�鿴SD����ʹ�ô�С��Ӧ��
����	:weicheng 2015.11.26
****************************************/
int  sd_used_size_ack(uint16_t serial,uint32_t sd_used_size)
{
	//	OS_ERR os_err;
		uint16_t fram_len;
		uint16_t len=0;
		uint8_t *p_fram;
		uint8_t *ptrMsg;
		uint16_t ptrMsg_len=0;
		uint8_t *ptr;
		uint16_t serial_t;
	    int ret = -1;
		
		len = 4 + 2;//������Ϣ���ڴ�ռ䳤��
		ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
		if(ptrMsg == NULL)
		{
			InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
			return -1;
		}

	    ptr = ptrMsg;
		
		*ptr++ = serial;  //Ӧ����ˮ��
	    *ptr++ = serial>>8;

        memcpy(ptr,&sd_used_size,4);//SD����ʹ�ô�С
        ptr += 4;
				
		ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
		p_fram = sgmw_pack_one_protocol_frame(UP_GET_SD_USED_ACK_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
        user_free(ptrMsg,__FUNCTION__);
		if(p_fram == NULL)
		{
			return -1;
		}
        //send====
	    if(modem_send_net_data(p_fram,fram_len)>0)
		{
			
			//DebugPrintf(REMOTE_DBG,"sd_used_size_ack,ack ok\r\n");
			ret = 0;		
		}
		else
		{
		   //DebugPrintf(REMOTE_DBG,"sd_used_size_ack, ack failed !!!!!!!!!\r\n");
		}  

        user_free(p_fram,__FUNCTION__);
        return ret;

}

/***************************************
������:ask_current_ip_port_ack
����	:ѯ�ʵ�ǰIP/�����Ͷ˿ڵ�Ӧ��
����	:weicheng 2015.11.26
****************************************/
int  ask_current_ip_port_ack(uint16_t serial,uint16_t port,uint8_t *ip_string)
{
	//	OS_ERR os_err;
		uint16_t fram_len;
		uint16_t len=0;
		uint8_t *p_fram;
		uint8_t *ptrMsg;
		uint16_t ptrMsg_len=0;
		uint8_t *ptr;
		uint8_t i;
		uint16_t serial_t;
		uint8_t ip_string_len;
	     int ret = -1;
		
		ip_string_len = strlen((const char *)ip_string);//������Ϣ���ڴ�ռ䳤��
		len = ip_string_len + 2 + 2;
		ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
		if(ptrMsg == NULL)
		{
			InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
			return -1;
		}
	    ptr = ptrMsg;
		
		*ptr++ = serial;  //Ӧ����ˮ��
	    *ptr++ = serial>>8;

		*ptr++ = port;  //�˿�
        *ptr++ = port>>8; 

		for(i=0;i<ip_string_len;i++) //IP/����120.025.12.78  /  tx.tepth.cn
		{
           *ptr++ = *(ip_string+i);
		}
				
		ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
		p_fram = sgmw_pack_one_protocol_frame(UP_GET_REMOTE_NET_PARAM_ACK_MSG_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
        user_free(ptrMsg,__FUNCTION__);
		if(p_fram == NULL)
		{
			return -1;
		}
        //send====
        if(modem_send_net_data(p_fram,fram_len)>0)
		{
			
			//DebugPrintf(REMOTE_DBG,"ask_current_ip_port_ack, ack ok\r\n");
			ret = 0;		
		}
		else
		{
		   //DebugPrintf(REMOTE_DBG,"ask_current_ip_port_ack, ack failed !!!!!!!!!\r\n");
		}  
		
        user_free(p_fram,__FUNCTION__);
        return ret;

}

/************************************
������:ack_server_query_param
����	:��������ѯ�ն˲���Ӧ��
����	:weicheng  2015.11.27
*************************************/
uint8_t file_list_str[200]={0};
void ack_server_query_param(uint16_t rec_serial,uint16_t rec_msg_id)
{
	//DebugPrintf(REMOTE_DBG,"qry param:s:%02X,id:%02X\r\n",rec_serial,rec_msg_id);
	//FRESULT ret;
	//u16 pLen=0,
    uint16_t tolal_plen=0;
	//uint8_t file_number=0;	
	uint8_t str_copy[32]={0};
	//INIRESULT ret;
	uint32 sd_used_size= 0;
	uint32 sd_total_size = 0;
	
	switch(rec_msg_id)
	{
		case DN_GET_DEFAULT_DBC_MSG_ID: //�鿴Ĭ��DBC�ļ�
			//��ȡĬ��DBC�ļ����ļ������� 
			if(!get_default_tbc_name(str_copy))	
			{
				query_default_dbcfile_ack(rec_serial,(uint8_t *)str_copy,strlen((const char *)str_copy));
			}
			else
			{
				str_copy[0]='\0';
				query_default_dbcfile_ack(rec_serial,(uint8_t *)str_copy,0); 
			}
			break;
		
		case DN_GET_DBC_LIST_MSG_ID: //�鿴DBC�ļ��б�
			get_tbc_file_list(file_list_str,&tolal_plen);
			query_dbcfile_list_ack(rec_serial,file_list_str,tolal_plen);
			break;
		
		case DN_GET_VER_MSG_ID: //�鿴��ϵͳ�汾��
			device_system_version_ack(rec_serial,(char*)get_version());
			break;
		
		case DN_GET_REPORT_PERIOD_MSG_ID: //�鿴�ش�Ƶ��
			postback_frequency_ack(rec_serial,get_report_period());
			break;
		
		case DN_GET_HEARTBEAT_MSG_ID: //�鿴����Ƶ��
			heartbeat_frequency_ack(rec_serial,get_heartbeat_period());
			break;

		case DN_GET_USE_FILE_TYPE_MSG_ID: //�鿴�ش���ʽ
			postback_type_ack(rec_serial,get_rep_tbc_type());                              
			break;
			
		case DN_GET_SAVE_DATA_EN_MSG_ID: //�鿴�Ƿ񱣴�SD��
			sd_save_status_ack(rec_serial,get_sd_save_status());//0�����棬1����
			break;					

		case DN_GET_SAVE_DATA_TIME_MSG_ID: //�鿴SD������ʱ��
			sd_save_time_ack(rec_serial,get_sd_save_time());//0�Զ���>1��λ��
			break;
			
		case DN_GET_SD_STATE_MSG_ID: //�鿴D��״̬
			sd_exist_status_ack(rec_serial,get_mount_flag());//0δ����SD����1�Ѽ���SD��
			break;
			
		case DN_GET_SD_VOL_MSG_ID: //�鿴SD���ܴ�С
			//Get_Emmc_Info(1,GET_TOTAL_VOLUME,&sd_total_size);
			sd_total_size_ack(rec_serial,sd_total_size/(1024*1024));
			break;
			
		case DN_GET_SD_USED_MSG_ID: //�鿴D�����ô�С
			//Get_Emmc_Info(1,GET_USED_VOLUME,&sd_used_size);
			sd_used_size_ack(rec_serial,sd_used_size/(1024*1024));
			break;
			
		case DN_GET_REMOTE_NET_PARAM_MSG_ID: //�鿴��ǰIP�˿ں�
			//ask_current_ip_port_ack(rec_serial,get_remote_port(),(uint8_t *)get_remote_dn());
			break;
			
		default:
			break;
	}
}
/************************************
������:ack_server_set_param
����	:��������ѯ�ն�
����	:weicheng  2015.11.27
*************************************/
void ack_server_set_param(uint16_t rec_serial,uint16_t rec_msg_id,uint8_t result)
{
	switch(rec_msg_id)
	{
		case DN_DOWN_DBC_FILE_MSG_ID:// ƽ̨�·�DBC�ļ�

		case DN_SET_DEFAULT_DBC_MSG_ID: //ƽ̨�޸�Ĭ�ϵ�DBC�ļ�

		case DN_DEL_DBC_FILE_MSG_ID: //ɾ��DBC�ļ�

		case DN_SET_REPORT_PERIOD_MSG_ID: //�޸Ļش�Ƶ��

		case DN_SET_HEARTBEAT_MSG_ID: //�޸�����Ƶ��

		case DN_SET_USE_FILE_TYPE_MSG_ID: //�޸Ļش���ʽ

		case DN_SET_SAVE_DATA_EN_MSG_ID: //�޸��Ƿ񱣴�SD��

		case DN_SET_SAVE_DATA_TIME_MSG_ID: //�޸�D������ʱ��

		case DN_SET_REMOTE_NET_PARAM_MSG_ID: //�޸ĵ�ǰIP/�����˿ں�

		case NEW_UPDATE_PACKEGE_ID: //
			normal_ack_to_server(rec_serial,rec_msg_id,result);
			break;	
			
		default:
		InfoPrintf("invalid sever_set_ack msg !!!\r\n");
		break;
	}
}



#define ____request_update_______________________________________________________
/**************************************
������     :request_update_package
����      :�ն���ƽ̨����������
����      :weicheng 2015.12.21 һ
***************************************/
int  request_update_package(uint8_t *md5,uint8_t serial)
{
        uint16_t fram_len;
		uint16_t len=0;
		uint8_t *p_fram;
		uint8_t *ptrMsg;
		uint16_t ptrMsg_len=0;
		uint8_t *ptr;
		uint8_t i;
		uint16_t serial_t;
	    int ret = -1;
		//OS_ERR os_err;

		InfoPrintf("req UPD PACK:%u\r\n",serial);
		/*if(serial == 1)
		{
			InfoPrintf("delay 200ms-----start-----\r\n");
			OSTimeDly (200,OS_OPT_TIME_DLY,&os_err);
			InfoPrintf("delay 200ms-----end-----\r\n");
		}*/
		//������Ϣ���ڴ�ռ䳤��
		len = 32;
		ptrMsg = (uint8_t *)user_malloc(len,__FUNCTION__);//����
		if(ptrMsg == NULL)
		{
			InfoPrintf("%s,malloc fail\r\n",__FUNCTION__);
			return -1;
		}
	    ptr = ptrMsg;		
		for(i=0;i<16;i++) //�����ļ�MD5
		{
           *ptr++ = *(md5+i);
		}
		*ptr++ = serial;//��������
		
		ptrMsg_len = ptr-ptrMsg;//��Ϣ�峤��
		p_fram = sgmw_pack_one_protocol_frame(REQUEST_UPDATE_PACKEGE_ID,(uint8_t*)ptrMsg,ptrMsg_len,&fram_len,&serial_t);
        user_free(ptrMsg,__FUNCTION__);
		if(p_fram == NULL)
		{
			return -1;
		}
        //send====
        if(modem_send_net_data(p_fram,fram_len)>0)
		{
			
			//DebugPrintf(REMOTE_DBG,"request_update_package, ack ok\r\n");
			ret = 0;		
		}
		else
		{
		  // DebugPrintf(REMOTE_DBG,"request_update_package, ack failed !!!!!!!!!\r\n");
		}  
		
        user_free(p_fram,__FUNCTION__);
        return ret;
}

/**********************************
����ֵ: 0�ɹ�����0ʧ��
************************************/
int sgmw_rec_dbc_tbc_file(unsigned char mult_pack_f,
	unsigned char *pdu,unsigned short len)
{
#if 0
  int ret = 0;
	static uint8_t i,j;
	int ack_result=0;
	unsigned char total_pack=1;
	unsigned char curr_pack =1;
	unsigned char first_pack=0;
	unsigned char *pdata=NULL;
    static uint32_t file_len_t = 0;
    uint32_t upgrade_tbc_pack_buf_len_temp=0;
    static uint32_t  upgrade_tbc_pack_buf_len=0;  //buffer 1024�����е����ݳ���
    static uint32_t tbc_add_temp = 0x0000B000;
	static uint32_t file_total_temp=0;
	uint8_t file_same_flag = 0;
	uint8_t file_name_str[32]={0};
	uint32_t addr_temp = UPDATE_TBC_FILE_BASE_14;
	uint8_t *ptr=NULL;
	uint8_t last_tbc_pack_flag = 0; //  ���ս���
	static uint32_t save_tbc_add_temp = UPDATE_TBC_FILE_BASE_14;//tbc�ݴ��ַ
    uint8_t file_num=0;
	OS_ERR os_err;
	uint32_t counter_t=0;

	if(!mult_pack_f)//û�зְ�
	{
		first_pack = 1;
		pdata = pdu;
	}
	else //�зְ�
	{
		pdata = pdu + 4;
		total_pack = pdu[0];
		curr_pack = pdu[2];
		if(1 == curr_pack)
		{
			
            first_pack = 1;          
		}
	}
    InfoPrintf("rx TBC:total pack:%u,curr pack:%u\r\n",total_pack,curr_pack);
	if(first_pack)
    {		
		
		//ɾ��14����
		feed_wdt();
		__disable_irq();
        
		ret = Chip_IAP_PreSectorForReadWrite(UPDATE_TBC_STR_SECTOR_14,UPDATE_TBC_STR_SECTOR_14);
		ret = Chip_IAP_EraseSector(UPDATE_TBC_STR_SECTOR_14,UPDATE_TBC_STR_SECTOR_14);
		__enable_irq();
		feed_wdt();
		
		//�ļ�����
		memcpy((void*)&tbc_file_flag_t.file_len,pdata+1,2);
		
		//��ȡ����
	    tbc_file_flag_t.file_type = *pdata;//�ļ���ʶ
	    
	    memset(FileCopyBuffer,0xFF, sizeof(FileCopyBuffer)); //��0XFF
	    save_tbc_add_temp = UPDATE_TBC_FILE_BASE_14;

		file_len_t = 0;
        file_len_t += len;//�ļ��ĳ����ۼ�
        upgrade_tbc_pack_buf_len_temp=0;
        upgrade_tbc_pack_buf_len=0;  //buffer 1024�����е����ݳ���
                    

		if(!mult_pack_f)//�ļ����ְ�
		{						
			//��ȡ�������ļ�						
			if(tbc_file_flag_t.file_len <= 1024)
			{
				memcpy(FileCopyBuffer,pdata+3,tbc_file_flag_t.file_len);
				ret = WriteToAppArea(&save_tbc_add_temp,FileCopyBuffer,1024); 			
				if(ret != 0)
				{
					InfoPrintf("write tbc pack failed\r\n"); 
					return 1;
				}
				
			}

			//�ļ�������
			tbc_file_flag_t.file_name_len = len-3 - tbc_file_flag_t.file_len;

			//��ȡ�ļ���
			rec_file_t.file_name = pdata +3 + tbc_file_flag_t.file_len;//�ļ�����ʼ��ַ
			*(pdata +3+ tbc_file_flag_t.file_len + tbc_file_flag_t.file_name_len) = '\0';			 
			memset(tbc_file_flag_t.file_name,0x00,sizeof(tbc_file_flag_t.file_name));
			memcpy((void*)&tbc_file_flag_t.file_name,rec_file_t.file_name,tbc_file_flag_t.file_name_len);

			last_tbc_pack_flag = 1; //���ս���
			                                                  			
		}
		else //�ļ��ְ��ĵ�һ��
		{		
           
			upgrade_tbc_pack_buf_len_temp = upgrade_tbc_pack_buf_len;
            upgrade_tbc_pack_buf_len_temp += len-3;//����ﵽ1024��д�롣
            if(upgrade_tbc_pack_buf_len_temp < 1024)
            {           
                memcpy(FileCopyBuffer+upgrade_tbc_pack_buf_len,pdata+3,len-3);                  
                upgrade_tbc_pack_buf_len += len-3;
            }
            else
            {
                return 1;
            }        
		}
		
	}
	else
	{//mult and 2,3,4...             
        file_len_t += len;
        upgrade_tbc_pack_buf_len_temp = upgrade_tbc_pack_buf_len;
        upgrade_tbc_pack_buf_len_temp += len;//����ﵽ1024��д�롣
            
		if(curr_pack < total_pack)
		{
			
            if(file_len_t > 4096) return 1;
            if(upgrade_tbc_pack_buf_len_temp >= 1024)
            { 
                //дtbc���ļ�����
                
                memcpy(FileCopyBuffer+upgrade_tbc_pack_buf_len,pdata,1024-upgrade_tbc_pack_buf_len);
                pdata += 1024-upgrade_tbc_pack_buf_len;
			
			   	ret = WriteToAppArea(&save_tbc_add_temp,FileCopyBuffer,1024);              
                if(ret != 0)
                {
                    InfoPrintf("write tbc pack failed\r\n");
                    return 1;                             
                }
                          
                memset(FileCopyBuffer,0xff, sizeof(FileCopyBuffer));
                memcpy(FileCopyBuffer,pdata,(len - (1024-upgrade_tbc_pack_buf_len)));
                upgrade_tbc_pack_buf_len = len - (1024-upgrade_tbc_pack_buf_len);               
            }
            else
            {
                memcpy(FileCopyBuffer+upgrade_tbc_pack_buf_len,pdata,len);                  
                upgrade_tbc_pack_buf_len += len;
            }
                      
		}
		else //���յ����һ��
		{	
			
            if(file_len_t > 4096) return 1;
            if(upgrade_tbc_pack_buf_len_temp >= 1024)
            { 
                //дtbc���ļ�����               
                memcpy(FileCopyBuffer+upgrade_tbc_pack_buf_len,pdata,1024-upgrade_tbc_pack_buf_len);
                pdata += 1024-upgrade_tbc_pack_buf_len;
                
                ret = WriteToAppArea(&save_tbc_add_temp,FileCopyBuffer,1024);              
                if(ret != 0)
                {
                    InfoPrintf("write tbc pack failed\r\n");
                    return 1;                        
                }
                          
                memset(FileCopyBuffer,0xff, sizeof(FileCopyBuffer));
                memcpy(FileCopyBuffer,pdata,(len - (1024-upgrade_tbc_pack_buf_len)));
                upgrade_tbc_pack_buf_len = len - (1024-upgrade_tbc_pack_buf_len);               
            }
            else
            {
                memcpy(FileCopyBuffer+upgrade_tbc_pack_buf_len,pdata,len);                  
                upgrade_tbc_pack_buf_len += len;
            }
			
            #if 1
			counter_t = OSTimeGet (&os_err);
			InfoPrintf("clock tick д���ݰ�--start-- %d----\r\n",counter_t);
			#endif
			
            //дtbc���ļ�����                           
            ret = WriteToAppArea(&save_tbc_add_temp,FileCopyBuffer,1024);              
            if(ret != 0)
            {
                InfoPrintf("write tbc pack failed\r\n");
                return 1;                        
            }
			
			#if 1
			counter_t = OSTimeGet (&os_err);
			InfoPrintf("clock tick д���ݰ�--end-- %d----\r\n",counter_t);
			#endif
			
			//�ļ�������
			tbc_file_flag_t.file_name_len = file_len_t - 3 -tbc_file_flag_t.file_len;
			memset((void *)&tbc_file_flag_t.file_name,0,sizeof(tbc_file_flag_t.file_name));
			//�ļ���
			tbc_add_temp = UPDATE_TBC_FILE_BASE_14; 
			memcpy((void *)&tbc_file_flag_t.file_name,(const void *)(tbc_add_temp + tbc_file_flag_t.file_len),tbc_file_flag_t.file_name_len);

			last_tbc_pack_flag = 1; //���ս���
			                       
		}
	   
	}   
//--2016.4.27��
	if(last_tbc_pack_flag == 1) //����tbc�ļ�����
	{
		//�Ƚ��ļ���
		//�жϿյ�����
		for(i=0;i<MAX_TBC_FLAG_N;i++)
		{
			if(read_file_exist_flag(i) == 1)
			{
				read_tbc_file_name(file_name_str,i);
				ret = strcmp((const char *)tbc_file_flag_t.file_name,(const char *)file_name_str);
				if(ret == 0)//������ͬ
				{
					file_same_flag = 1;
					break;
				}					
			}
		}
		
		if(file_same_flag == 0)	//û��ͬ��	
		{
			//�жϿյ�����
			for(i=0;i<MAX_TBC_FLAG_N;i++)
			{
				if(read_file_exist_flag(i) != 1)
				{
					break;
				}
			}
			if(i == MAX_TBC_FLAG_N) //������
			{				
				InfoPrintf("TBC files number full!!!!!!!!!\r\n");
                return 1; 
			}
		}

		//�����洢��
		switch(i)
		{
			case 0:
				ret = Chip_IAP_PreSectorForReadWrite(UPDATE_TBC_STR_SECTOR_11,UPDATE_TBC_STR_SECTOR_11);
				ret = Chip_IAP_EraseSector(UPDATE_TBC_STR_SECTOR_11,UPDATE_TBC_STR_SECTOR_11);	
				upgrade_tbc_add = UPDATE_TBC_FILE_BASE_11;  //tbc  д�����ʼ��ַ
				tbc_add_temp = UPDATE_TBC_FILE_BASE_11;  //tbc  д�����ʼ��ַ
				
				break;
			case 1:
				ret = Chip_IAP_PreSectorForReadWrite(UPDATE_TBC_STR_SECTOR_12,UPDATE_TBC_STR_SECTOR_12);
				ret = Chip_IAP_EraseSector(UPDATE_TBC_STR_SECTOR_12,UPDATE_TBC_STR_SECTOR_12);
				upgrade_tbc_add = UPDATE_TBC_FILE_BASE_12;  //tbc  д�����ʼ��ַ			
				tbc_add_temp = UPDATE_TBC_FILE_BASE_12;  //tbc  д�����ʼ��ַ
				break;
			case 2:
				ret = Chip_IAP_PreSectorForReadWrite(UPDATE_TBC_STR_SECTOR_13,UPDATE_TBC_STR_SECTOR_13);
				ret = Chip_IAP_EraseSector(UPDATE_TBC_STR_SECTOR_13,UPDATE_TBC_STR_SECTOR_13);
				upgrade_tbc_add = UPDATE_TBC_FILE_BASE_13;  //tbc  д�����ʼ��ַ
				tbc_add_temp = UPDATE_TBC_FILE_BASE_13;  //tbc  д�����ʼ��ַ
				break;
			default:;
			
		}
        if(ret != 0)
        {
            InfoPrintf("EraseSector TBC upgrade area failed\r\n");
            
            return 1;
        }
		//�����ڵ��ļ�����
		for(j=0;j<MAX_TBC_FLAG_N;j++)
		{
			if(read_file_exist_flag(j)== 1)
			{
               file_num++;
			}
		}
		
		//��ȡ�ļ�����Ŀ
		file_total_temp = read_tbc_file_total(0);
		if(file_num != file_total_temp) //��ͬ�Ѵ��ڱ�־����Ϊ׼
		{
			file_total_temp = file_num;			
			InfoPrintf("�ļ���������ڱ�־������������ʼ��tbc�ļ�����!!!!!!!!!!!!\r\n");
			
		}			
		
		//���ļ����Ƶ���Ӧ����
		ptr = (uint8_t*)user_malloc(1024,__FUNCTION__);    
	    if(ptr == NULL)
	    { 
			InfoPrintf("user_malloc TBC transfer failed!!!!!!!!!!!!\r\n");
			return 1;
	    }				
		#if 1
		counter_t = OSTimeGet (&os_err);
		InfoPrintf("clock tick ����--start-- %d----\r\n",counter_t);
		#endif
		while(addr_temp < UPDATE_TBC_FILE_BASE_15)
		{
			memcpy(ptr,(void*)addr_temp,1024);
			
			ret = WriteToAppArea(&upgrade_tbc_add,(unsigned char *)ptr,1024); 
			if(ret != 0)
			{
				InfoPrintf("TBC transfer failed!!!!!!!!!!!!\r\n");
				user_free(ptr,__FUNCTION__); 
				return 1;
			}			
	        addr_temp += 1024;
			//ÿдһ���ӳ�50ms
			OSTimeDly(50, OS_OPT_TIME_DLY, &os_err);
		}						
		user_free(ptr,__FUNCTION__);
		
		#if 1
		counter_t = OSTimeGet (&os_err);
		InfoPrintf("clock tick ����--end-- %d----\r\n",counter_t);
		#endif
													       
        //��ȡ�ļ����ͱ�־������       
		//��ȡ�ļ����Ȳ�����       		
		//�ļ�������        		
        //д���ļ���
        //����MD5ֵ������          
        memset(md5_tmp_t,0,sizeof(md5_tmp_t));//Ҫ����
        
        ret = get_upgradefile_md5(md5_tmp_t,tbc_add_temp,tbc_file_flag_t.file_len);
        if(ret != 0)
        {
            InfoPrintf("count tbc_file_md5 failed\r\n"); 
			return 1;
        }
        
        //�ļ�����ʼ��ַ       
        //=�Ƿ�ΪĬ��tbc        		
		//д���б�־           
		//��ȡ�ļ�����Ŀ		
		if(file_same_flag == 0)
		{
			file_total_temp +=1;
		}
		//д�������
		ret = write_multi_tbcfile_param(tbc_file_flag_t.file_type,tbc_file_flag_t.file_name,\
										tbc_file_flag_t.file_name_len,tbc_add_temp,tbc_file_flag_t.file_len,\
										md5_tmp_t,1,file_total_temp,1,i);
		if(ret != 0)
        {
            InfoPrintf("write_multi_tbcfile_param failed---------\r\n"); 
			return 1;
        }
		
		post_update_param_msg(	&tcb_can_manage_task,
								REMOTE_RECV_TASK_ID,
								FATHER_MSG_T_REMOTE_UPDATE_PARAM,
								RMT_UPD_TBC_DFT
							 );
	}
//--2016.4.27��
	
	return ack_result;
    #endif
    return 0;
}

#define ____TASK___________AREA__________________________________________

OS_TMR 	period_report_tmr;
#define PD_REP_INFO_DATA_Q_MAX (6)
OS_Q pd_rep_info_data_Q={0};
#define PD_REP_INFO_DATA_SAVE_Q_MAX (3)
OS_Q pd_rep_info_data_save_Q={0};


void pd_pack_info_data_tmr_callback(void*p_Tmr,void*p_Arg)
{
	//OS_ERR os_err;
	if(get_gprs_send_fun()==1)
	{
		//InfoPrintf("pd_pack_info_data_tmr_callback()--------------%d---\r\n",OSTimeGet(&os_err));
    	sgmw_pack_and_post_pd_rep_info_data_q();
	}
}

void create_and_start_pd_packing_timer(void)
{
	OS_ERR os_err;
	//create the timer
	OSTmrCreate(&period_report_tmr,
				"period report tmr",
				10,//N*100ms ,������λ��100ms  zgc
				get_report_period(), 
				OS_OPT_TMR_PERIODIC,
				pd_pack_info_data_tmr_callback,
				NULL,
				&os_err);
	
	OSTmrStart(&period_report_tmr, &os_err);
}

void change_report_period(uint32_t preriod)
{
	//�ù���������汾��UCOS��Ҫdelete tmr,�����´��� ????
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
			if(sgmw_save_itv((uint8_t *)ptr,rep_msg_len)!=0)
			{
				InfoPrintf("sgmw_save_itv(xxx,%d)failed C !!\r\n",rep_msg_len);
			}
			else
			{
				InfoPrintf("sgmw_save_itv(xxx,%d) ok (Q full)\r\n",rep_msg_len);
			}
			
			user_free(ptr,__FUNCTION__);
		}
		
		OSSchedRoundRobinYield(&os_err);
	}
}


void remote_report_task(void*p_para)
{
	OS_ERR os_err;
    //uint32_t t_wait=0;
	//int i;
	int ret;
	TASK_MSG_T *p_msg;
	uint16_t msg_size;
	uint8_t  *rep_msg=NULL;
	uint16_t rep_msg_len=0; 
	p_para = p_para;

	
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
		}

		//---------------------------------------------------------------------------------------
		rep_msg=(uint8_t *)OSQPend(&pd_rep_info_data_Q,1,OS_OPT_PEND_BLOCKING,&rep_msg_len,NULL,&os_err);
		if(rep_msg != NULL)
		{
			if(get_already_login_flag()==1)
			{
				ret=sgmw_pack_and_rep_pd_frame((uint8_t *)rep_msg,rep_msg_len);
				if(ret==0)
				{
					//InfoPrintf("sgmw_pack_and_rep_pd_frame() ok\r\n");
				}
				else
				{
					InfoPrintf("sgmw_pack_and_rep_pd_frame() failed!!!\r\n");
					#if ENABLE_ITV
					if(sgmw_save_itv((uint8_t *)rep_msg,rep_msg_len)!=0)
					{
						InfoPrintf("sgmw_save_itv(xxx,%d)failed (net fault) !!\r\n",rep_msg_len);
					}
					else
					{
						InfoPrintf("sgmw_save_itv(xxx,%d)ok (net fault) !!\r\n",rep_msg_len);
					}
					#endif
				}	
			}
			else
			{
				InfoPrintf("sgmw_save_itv(xxx,%d)failed -- (no login) !!\r\n",rep_msg_len);
			
				//save itv
				#if ENABLE_ITV
				if(sgmw_save_itv((uint8_t *)&rep_msg[2],rep_msg_len)!=0)
				{
					InfoPrintf("sgmw_save_itv(xxx,%d)failed (no login) !!\r\n",rep_msg_len);
				}
				else
				{
					InfoPrintf("sgmw_save_itv(xxx,%d)ok (no login) !!\r\n",rep_msg_len);
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

		OSTimeDly(T_10MS,OS_OPT_TIME_DLY,&os_err);
	}
}


#endif

