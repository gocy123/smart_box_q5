//static head file
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

//porting head files
#include "heads.h"

#include "user_debug_app.h"
#include "os.h"
#include "os_cfg_app.h"



#define GPS_CFG_NUM 6
const unsigned char gpsbuf[GPS_CFG_NUM][14]={
/*1*/	{0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x02,0x2E},//GPVTG
/*2*/	{0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0xFF,0x1F},//GPGSA
/*3*/	{0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x24},//GPGSV
/*4*/	{0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x01,0x00,0x00,0x00,0x00,0xFE,0x1A},//GPGLL
/*5*/	{0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x08,0x00,0x00,0x00,0x00,0x05,0x3D},//GPZDA
/*6*/	{0xB5,0x62,0x06,0x08,0x06,0x00,0xF4,0x01,0x01,0x00,0x01,0x00,0x0B,0x77} //GPRMC 2Hz report
/*7*/	//{0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A} //GPRMC 5Hz report

/*8*/	//{0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0xFD,0x15} //GPGGA
};

OS_SEM gps_lock;

#define READ_FROM_LINK_BUF_LEN    (128)
#define EXTRACTED_FRAME_BUF_LEN   (155)
static uint8_t read_from_link_buf[READ_FROM_LINK_BUF_LEN]={0};
static uint8_t extracted_frame_buf[EXTRACTED_FRAME_BUF_LEN]={0};
static gps_location_info_T gps_location_info={0};
//static gps_time_t gps_time={0};


/*
	input:
			On_Off:
					0: power off
					1: power on
	return:
			0: success
			1: error para
*/
static void gps_power_ctl(uint8_t On_Off)
{
	//modem PWRKEY
	/*PINSEL_ConfigPin(1, 26, 0);
	GPIO_SetDir(1, (1<<26), GPIO_DIRECTION_OUTPUT);

	if(On_Off==0)
	{
		GPIO_OutputValue(1, (1<<26), 0);	//power off 
	}
	else if(On_Off==1)
	{
		GPIO_OutputValue(1, (1<<26), 1);	//power on 
	}*/
}

/*
	input:
			en_reset:
					0: normal
					1: reset gps module 
					2: low power dissipation
*/
static void gps_reset_ctl(uint8_t en_reset)
{
	/*PINSEL_ConfigPin(4, 14, 0);
	GPIO_SetDir(1, (4<<14), GPIO_DIRECTION_OUTPUT);
	
	if(en_reset==0)
	{
		GPIO_OutputValue(4, (1<<14), 1);
	}
	else if(en_reset==1)
	{
		GPIO_OutputValue(4, (1<<14), 0);
	}
	else if(en_reset==2)
	{
		//Chip_IOCON_PinMuxSet(LPC_IOCON, 4, 14,IOCON_FUNC0 | IOCON_OPENDRAIN_EN);
		//Chip_GPIO_SetPinDIR(LPC_GPIO,4,14,1);
		//Chip_GPIO_SetPinOutHigh(LPC_GPIO,4,14);
	}*/
}

/*
	brief:config the GPS moudule itself, such as: gps frame`s report freq ...
	return: 
			0: success
			1: error
*/
static void gps_config_module(void)
{
	//OS_ERR os_err;
	int i;
	for (i=0;i<GPS_CFG_NUM;i++)
	{
		//uart_write(GPS_UART_PORT, (unsigned char*)&gpsbuf[i][0], 14 );
		uart2_send_data((unsigned char*)&gpsbuf[i][0], 14);
		user_delay_ms(200);
	}
}


static void gps_init_ant_ck_pin(void)
{
	//OS_ERR err;
	
	/*PINSEL_ConfigPin(1, 28, 0);
	GPIO_SetDir(1, (1<<28), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(1, (1<<28), 1);

	user_delay_ms(200);
	
	PINSEL_ConfigPin(1, 28, 0);
	GPIO_SetDir(0, (1<<28), GPIO_DIRECTION_OUTPUT);*/
}



void gps_open_module(void)
{
	user_uart2_open();
	//gps_power_ctl(1);
	//gps_reset_ctl(0);
	//user_delay_ms(TIME_10ms);
	//gps_config_module();
	//gps_init_ant_ck_pin();
}

void gps_close_module(void)
{
	user_uart2_close();	
	gps_power_ctl(0);
}

#if 0

//GPS纠偏算法 20161027
/*
	brief: get location info from the gps frame
	
	input:
			*buf: pointer of the frame that according to gps NMEA-0183 format data
	return:
			0		: content of the frame is corrcet
			others	: content error
*/
static int parse_gps_frame_data(uint8_t *buf)
{
	OS_ERR os_err;
	int n;
	uint32_t temp;
	RTC rtc_time,rd_rtc;
	static uint8_t calibrat_time_cnt=0;
	gps_location_info_T temp_gps_location_info={0};
	static float last_lati=182.0;
	static float last_longi=182.0;
	       float curr_lati=0;
	       float curr_longi=0;
	static uint8_t longi_shift_move_cnt=0;
	static uint8_t lati_shift_move_cnt=0;

	if(strncmp((char const *)buf, "$GPRMC", 6)==0)
	{
		DbgRmPrintf(GPS_RM,"%s",buf);
		OSSemPend(&gps_lock,10000,OS_OPT_PEND_BLOCKING,NULL,&os_err);
		
		//-----------------------------------------------------------------
		
		//$GNRMC,004505.000,A,2241.001098,N,11416.656622,E,0.028,129.792,201015,,E,A*3F
		//                               time      va   lati  NS    longi  EW    sp   sp-dr  date
		n=sscanf((char const*)buf,"%*[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]",
				&temp_gps_location_info.time[0],
				&temp_gps_location_info.location_valid,
				&temp_gps_location_info.latitude[0],
				&temp_gps_location_info.latitude_NS,
				&temp_gps_location_info.longitude[0],
				&temp_gps_location_info.longitude_EW,
				&temp_gps_location_info.speed[0],
				&temp_gps_location_info.speed_direction[0],
				&temp_gps_location_info.date[0]
			);
		if(n<9)
		{
			//InfoPrintf("parse gps RMC fail\r\n");
			temp_gps_location_info.location_valid = 'V';
		}
        if(strlen((const char*)&temp_gps_location_info.date[0])< 6 ||
           strlen((const char*)&temp_gps_location_info.time[0])< 6)
        {
           // InfoPrintf("parse gps time fail\r\n");
			temp_gps_location_info.location_valid = 'V';
        }

		/*{
			float a;
			a=atof(temp_gps_location_info.latitude);
			
			a-=1000.23;
			if(a)InfoPrintf("--xss");
		}*/
		
		if(temp_gps_location_info.location_valid == 'A')
		{
			curr_lati =atof(temp_gps_location_info.latitude);
			curr_longi=atof(temp_gps_location_info.longitude);

			//---lati------------------------------------------------------
			if((last_lati!=182.0))
			{
				if(last_lati>curr_lati)
				{
					if(last_lati-curr_lati>0.15)	
					{
						InfoPrintf("纬度漂移A\r\n");
						lati_shift_move_cnt++;
					}
					else
					{
						last_lati=curr_lati;
						lati_shift_move_cnt=0;
					}
				}	
				else
				{
					if(curr_lati-last_lati>0.15)	
					{
						InfoPrintf("纬度漂移B\r\n");
						lati_shift_move_cnt++;
					}
					else
					{
						last_lati=curr_lati;
						lati_shift_move_cnt=0;
					}
				}
			}
			else
			{
				last_lati=curr_lati;	
				lati_shift_move_cnt=0;
			}

			//---longi------------------------------------------------------
			if((last_longi!=182.0))
			{
				if(last_longi>curr_longi)
				{
					if(last_longi-curr_longi>0.15)	
					{
						InfoPrintf("经度漂移A\r\n");
						longi_shift_move_cnt++;
					}
					else
					{
						last_longi=curr_longi;
						longi_shift_move_cnt=0;
					}
				}	
				else
				{
					if(curr_longi-last_longi>0.15)	
					{
						InfoPrintf("经度漂移B\r\n");
						longi_shift_move_cnt++;
					}
					else
					{
						last_longi=curr_longi;
						longi_shift_move_cnt=0;
					}
				}
			}
			else
			{
				last_longi=curr_longi;	
				longi_shift_move_cnt=0;
			}

			if((lati_shift_move_cnt==0)&&(longi_shift_move_cnt==0))
			{
				gps_location_info.location_valid='A';
			}
			else if((lati_shift_move_cnt>=20)||(longi_shift_move_cnt>=20))
			{
				gps_location_info.location_valid='A';
			}
			else
			{
				gps_location_info.location_valid='V';
			}

			if(gps_location_info.location_valid=='A')
			{
				//copy lati longi
				temp=GPS_TIMELEN+1 + 1 + GPS_LATLEN+1 +1 + GPS_LONGLEN+1 +1 +GPS_SPEEDLEN+1 +GPS_SPEEDFXLEN+1 +GPS_DATELEN+1;
				memcpy((uint8_t *)&gps_location_info,(uint8_t *)&temp_gps_location_info,temp);	
			}
		}
		else
		{
			//已确定当前不定位
			last_lati=182.0;
			last_longi=182.0;
			gps_location_info.location_valid='V';
		}
		
		OSSemPost(&gps_lock,OS_OPT_POST_1,&os_err);
	}
	else if(strncmp((char const*)buf, "$GPGGA", 6)==0 && gps_location_info.location_valid=='A')
	{
		DbgRmPrintf(GPS_RM,"%s",buf);
		OSSemPend(&gps_lock,10000,OS_OPT_PEND_BLOCKING,NULL,&os_err);
		//$GNGGA,032257.000,2241.003225,N,11416.657230,E,1,16,0.690,79.556,M,0,M,,*58
		n = sscanf((char const*)buf,"%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%[^,],%*[^,],%[^,]",
				&gps_location_info.numSV[0],
				&gps_location_info.altitude[0]);
		if(n<1)
		{
			gps_location_info.numSV[0] = '0';
		}

		#if 1
		calibrat_time_cnt++;
		if(calibrat_time_cnt>=30)//500ms接收一次,
		{
			calibrat_time_cnt=0;
			
			if(gps_location_info.location_valid=='A')
			{
				static uint8_t err_local_time_cnt=0;
				
				ReadRTC(&rd_rtc);
				
				get_localtime_from_gps(gps_location_info.date,gps_location_info.time,&rtc_time);

				temp=abs(rtc_to_uint32(rd_rtc)-rtc_to_uint32(rtc_time));
				if(temp>=2)
				{
					err_local_time_cnt++;
					if(err_local_time_cnt>=2)//1分钟校时一次->10s
					{
						InfoPrintf("GPS定位校时:");
						InfoPrintf("%u-%u-%u %u:%u:%u\r\n",rtc_time.year,rtc_time.month,rtc_time.day,\
						rtc_time.hour,rtc_time.minute,rtc_time.second);
						InfoPrintf("本地RTC时间:");
						getnow();
						WriteRTC(&rtc_time);
						InfoPrintf("校时后读出RTC:");
						getnow();
						err_local_time_cnt=0;
					}
				}
				else
				{
					err_local_time_cnt=0;
				}
			}
		}
		#endif
       
		OSSemPost(&gps_lock,OS_OPT_POST_1,&os_err);
		
		#if 0
		InfoPrintf("\r\n时间:%s,  ",&gps_location_info.time[0]);
		InfoPrintf("定位:%c\r\n",gps_location_info.location_valid);
		InfoPrintf("纬度:%s, ",&gps_location_info.latitude[0]);
		InfoPrintf("纬向:%c  ",gps_location_info.latitude_NS);
		InfoPrintf("经度:%s,",&gps_location_info.longitude[0]);
		InfoPrintf("经向:%c\r\n",gps_location_info.longitude_EW);
		InfoPrintf("速度:%s,",&gps_location_info.speed[0]);
		InfoPrintf("速向:%s  ",&gps_location_info.speed_direction[0]);
		InfoPrintf("日期:%s,",&gps_location_info.date[0]);
		InfoPrintf("海拔:%s\r\n",&gps_location_info.altitude[0]);
		InfoPrintf("星数:%s\r\n",&gps_location_info.numSV[0]);
		#endif
	}
	
	return 0;
}

#endif

#define ___GPS____MIDDLE__________________________________________________________
static int parse_gps_frame_data(uint8_t *buf)
{
	#define USE_QUECEL_GPS 1

	OS_ERR os_err;
	int n;
	uint32_t temp;
	static uint8_t cnt=0;

	//static uint32_t last_rx_gps_time=0xFFFFFFFF;
	//static float last_lati=0;
	//static float last_longi=0;
	//float  current_value;

	BEIJING_TIME_T gps_time;
	BEIJING_TIME_T current_time;
	static uint8_t calibrat_time_cnt=0;
	//uint8_t temp_flag='V';	
	static uint8_t err_local_time_cnt=0;
	uint8_t error_move_flag=0;
	gps_location_info_T temp_gps_info={0};

	//InfoPrintf("%s",buf);
	OSSemPend(&gps_lock,10000,OS_OPT_PEND_BLOCKING,NULL,&os_err);
	#if USE_QUECEL_GPS
	if(strncmp((char const *)buf, "$GNRMC", 6)==0)
	#else
	if(strncmp((char const *)buf, "$GPRMC", 6)==0)
	#endif
	{
		if(cnt++%5==0)
		InfoPrintf("%s",buf);
		
		//$GNRMC,004505.000,A,2241.001098,N,11416.656622,E,0.028,129.792,201015,,E,A*3F
		//                               time      va   lati  NS    longi  EW    sp   sp-dr  date
		n=sscanf((char const*)buf,"%*[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]",
				&temp_gps_info.time[0],
				&temp_gps_info.location_valid,
				&temp_gps_info.latitude[0],
				&temp_gps_info.latitude_NS,
				&temp_gps_info.longitude[0],
				&temp_gps_info.longitude_EW,
				&temp_gps_info.speed[0],
				&temp_gps_info.speed_direction[0],
				&temp_gps_info.date[0]
				);
		
		if(n<9)
		{
			//InfoPrintf("parse gps RMC fail\r\n");
			temp_gps_info.location_valid = 'V';
		}
		
        if(strlen((const char*)&temp_gps_info.date[0])< 6 ||
           strlen((const char*)&temp_gps_info.time[0])< 6)
        {
           // InfoPrintf("parse gps time fail\r\n");
			temp_gps_info.location_valid = 'V';
        }

		if(temp_gps_info.location_valid =='A')
		{
			if(error_move_flag==0)
			{
				memcpy(gps_location_info.time,temp_gps_info.time,GPS_TIMELEN);
				gps_location_info.location_valid=temp_gps_info.location_valid;
				
				memcpy(gps_location_info.latitude,temp_gps_info.latitude,GPS_LATLEN);
				gps_location_info.latitude_NS=temp_gps_info.latitude_NS;
					
				memcpy(gps_location_info.longitude,temp_gps_info.longitude,GPS_LONGLEN);
				gps_location_info.longitude_EW=temp_gps_info.longitude_EW;
				
				memcpy(gps_location_info.speed,temp_gps_info.speed,GPS_SPEEDLEN);
				memcpy(gps_location_info.speed_direction,temp_gps_info.speed_direction,GPS_SPEEDFXLEN);
				memcpy(gps_location_info.date,temp_gps_info.date,GPS_DATELEN);
			}
			
			//last_rx_gps_time=time();

			if(gps_location_info.location_valid=='A')
			{
				calibrat_time_cnt++;
				if(calibrat_time_cnt>=20)
				{
					calibrat_time_cnt=0;
					get_current_time(&current_time);
					get_localtime_from_gps((char *)(gps_location_info.date),(char *)(gps_location_info.time),&gps_time);
					temp=abs(rtc_to_uint32(current_time)-rtc_to_uint32(gps_time));
					
					if(temp>=2)
					{
						err_local_time_cnt++;
						if(err_local_time_cnt>=2)//在间隔20*1秒内,连续2次相差超过2秒,则校时.
						{
							InfoPrintf("GPS定位校时:");
							InfoPrintf("%u-%02d-%02d %02d:%02d:%02d\r\n",gps_time.year,gps_time.month,gps_time.day,\
															gps_time.hour,gps_time.minute,gps_time.second);
							InfoPrintf("本地RTC时间:");
							printf_time_now();
							write_rtc(gps_time);
							InfoPrintf("校时后读出RTC:");
							printf_time_now();
							err_local_time_cnt=0;
						}
					}
					else
					{
						err_local_time_cnt=0;
					}
				}
			}
			else
			{
				calibrat_time_cnt=0;
				err_local_time_cnt=0;
			}
		}
		else
		{
			InfoPrintf("#");
			//InfoPrintf("##\r\n");
		}
	}
	#if 0
	else if(strncmp((char const*)buf, "$GPGGA", 6)==0 && gps_location_info.location_valid=='A')
	{
		//$GNGGA,032257.000,2241.003225,N,11416.657230,E,1,16,0.690,79.556,M,0,M,,*58
		n = sscanf((char const*)buf,"%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%[^,],%*[^,],%[^,]",
				&temp_gps_info.numSV[0],
				&temp_gps_info.altitude[0]);
		if(n<1)
		{
			temp_gps_info.numSV[0] = '0';
			temp_gps_info.location_valid = 'V';
		}

		if(gps_location_info.location_valid == 'A')
		{
			memcpy(gps_location_info.altitude,temp_gps_info.altitude,strlen((char const *)(temp_gps_info.altitude)));
			memcpy(gps_location_info.numSV,temp_gps_info.numSV,strlen((char const *)(temp_gps_info.numSV)));
		}

		#if 0
		if(gps_location_info.location_valid=='A'){
		InfoPrintf("\r\n时间:%s,  ",&gps_location_info.time[0]);
		InfoPrintf("定位:%c\r\n",gps_location_info.location_valid);
		InfoPrintf("纬度:%s, ",&gps_location_info.latitude[0]);
		InfoPrintf("纬向:%c  ",gps_location_info.latitude_NS);
		InfoPrintf("经度:%s,",&gps_location_info.longitude[0]);
		InfoPrintf("经向:%c\r\n",gps_location_info.longitude_EW);
		InfoPrintf("速度:%s,",&gps_location_info.speed[0]);
		InfoPrintf("速向:%s  ",&gps_location_info.speed_direction[0]);
		InfoPrintf("日期:%s,",&gps_location_info.date[0]);
		InfoPrintf("海拔:%s\r\n",&gps_location_info.altitude[0]);
		InfoPrintf("星数:%s\r\n",&gps_location_info.numSV[0]);}
		#endif
	}
	#endif

	#if 0
	else if(strncmp((char const*)buf, "$GPGSV", 6)==0)
	{
		InfoPrintf("%s",buf);

	}
	#endif
	
	OSSemPost(&gps_lock,OS_OPT_POST_1,&os_err);
	return 0;
}

/*
	brief:
		(1)extract one gps frame from the link buf
		(2)get gps location info from the extraced frame
*/
static uint16_t read_data_from_linkbuf(void)
{
	static enum gps_check_frame_stp step=step_search_head;
	static int i=0;	
	int read_len; 	//从link buffer读出的字节数
	uint16_t ret;
	int ptr;			//已处理的read_from_link_buf中的position
	uint8_t temp_byte;

	//read_len = uart_read(GPS_UART_PORT, read_from_link_buf, READ_FROM_LINK_BUF_LEN);
	read_len = uart2_receive_data(read_from_link_buf,READ_FROM_LINK_BUF_LEN);
	if(read_len<=0)
	{
		return 0;
	}
	
	ret=read_len;
	ptr=0;	

	while(read_len--)
	{
		temp_byte=read_from_link_buf[ptr++];
		//InfoPrintf("%c",temp_byte);
		switch(step)
		{
			case step_search_head:
				i=0;
				memset(extracted_frame_buf,EXTRACTED_FRAME_BUF_LEN,0);
				if(temp_byte=='$')
				{
					step=step_search_tail;
					extracted_frame_buf[i++]=temp_byte;
				}
				break;
				
			case step_search_tail:
				if((temp_byte!='$') && (i<EXTRACTED_FRAME_BUF_LEN))
				{
					if(extracted_frame_buf[i-1]==',' && temp_byte==',' )
					{
						extracted_frame_buf[i++]='0';
					}
					
					extracted_frame_buf[i++]=temp_byte;	
					
					if(temp_byte==0x0A)				// '\n'
					{
						if(extracted_frame_buf[i-2]==0x0D)	// '\r'
						{//extract one frame now !!!
							parse_gps_frame_data(extracted_frame_buf);							
							step=step_search_head;
						}
						else
						{//error
							//InfoPrintf("错误 tail\r\n");
							step=step_search_head;
						}
					}
				}
				else
				{
					//error
					i=0;
					if(temp_byte=='$')
					{
						//InfoPrintf("错误 $\r\n");
						memset(extracted_frame_buf,EXTRACTED_FRAME_BUF_LEN,0);
						extracted_frame_buf[i++]=temp_byte;
						step=step_search_tail;
					}
					else if(i>=EXTRACTED_FRAME_BUF_LEN)
					{
						//InfoPrintf("超长错误%d\r\n",i);
						step=step_search_head;
					}
				}
				break;
				
			default:
				break;
		}
	}
	
	return ret;
}

#define ___GPS_APP__________________________________________________________________________

int gps_check_lacation_state(void)
{
	OS_ERR os_err;

	OSSemPend(&gps_lock,1000,OS_OPT_PEND_BLOCKING,NULL,&os_err);

	if('A'==gps_location_info.location_valid)
	{	
		OSSemPost(&gps_lock,OS_OPT_POST_1,&os_err);
		return 1;
	}
	else
	{
		OSSemPost(&gps_lock,OS_OPT_POST_1,&os_err);
		return 0;
	}
}

/*
	return :
			0 :north latitude
			1 :south latitude
*/
uint8_t gps_get_south_north(void)
{
	OS_ERR os_err;

	OSSemPend(&gps_lock,1000,OS_OPT_PEND_BLOCKING,NULL,&os_err);
	if('N'==gps_location_info.latitude_NS)
	{	
		OSSemPost(&gps_lock,OS_OPT_POST_1,&os_err);
		return 0;
	}
	else
	{
		OSSemPost(&gps_lock,OS_OPT_POST_1,&os_err);
		return 1;
	}
}

/*
	return :
			0 :east longitude
			1 :west longitude
*/
uint8_t gps_get_east_west(void)
{
	OS_ERR os_err;

	OSSemPend(&gps_lock,1000,OS_OPT_PEND_BLOCKING,NULL,&os_err);
	if('E'==gps_location_info.longitude_EW)
	{	
		OSSemPost(&gps_lock,OS_OPT_POST_1,&os_err);
		return 0;
	}
	else
	{
		OSSemPost(&gps_lock,OS_OPT_POST_1,&os_err);
		return 1;
	}
}



void gps_get_location_info(gps_location_info_T *ptr)
{
	OS_ERR os_err;

	OSSemPend(&gps_lock,1000,OS_OPT_PEND_BLOCKING,NULL,&os_err);
	memcpy((uint8_t *)ptr,(uint8_t *)(&gps_location_info),sizeof(gps_location_info_T));
	OSSemPost(&gps_lock,OS_OPT_POST_1,&os_err);
}

//获得GPS可见星数
uint8_t  Get_GPS_Number(void)  //qdhai add
{
	uint8_t len=0;   
	uint8_t  gps_num=0;
	OS_ERR os_err;
	
	OSSemPend(&gps_lock,1000,OS_OPT_PEND_BLOCKING,NULL,&os_err);
	len = strlen((char const*)gps_location_info.numSV);
	if(len==1)
	{
		gps_num=gps_location_info.numSV[0]-'0';
	}
	else if(len==2)
	{ 
		gps_num=(gps_location_info.numSV[0]-'0')*10+gps_location_info.numSV[1]-'0';
	}
	OSSemPost(&gps_lock,OS_OPT_POST_1,&os_err);
	return gps_num;
}

static void gps_led_ctl(void)
{
	OS_ERR os_err;
	static uint32_t last=0;
	static uint8_t flag=0;	
	
	/*if(gps_location_info.location_valid!='A')
	{

		if(OSTimeGet(&os_err)-last>500)
		{
			last=OSTimeGet(&os_err);
			if(flag++%2==0)
			GPIO_OutputValue(0, (1<<13),1);
			else
			GPIO_OutputValue(0, (1<<13),0);
		}
	}
	else
	{
		GPIO_OutputValue(0, (1<<13), 0);
	}*/
}


void GpsTaskMsg_handle(TASK_MSG_T *p_msg)
{

	switch (p_msg->msg_src_id)
	{
		case DEV_MANAGE_TASK_ID:
			switch (p_msg->msg_father_type)
			{
				case FATHER_MSG_T_TASK_POWER_CTRL:
					if(p_msg->msg_son_type==POWER_CTRL_PWR_ON)
					{
						gps_open_module();
					}
					else if(p_msg->msg_son_type==POWER_CTRL_WAKEUP)
					{

					}
					else if(p_msg->msg_son_type==POWER_CTRL_SLEEP)
					{

					}
					else if(p_msg->msg_son_type==POWER_CTRL_PWR_OFF)
					{

					}
					break;
				default:
					break;
			}
			break;
		case MODEM_MANAGE_TASK_ID:
			break;
		default:
			break;
	}		
}

void GpsTask(void *p_arg)
{
	TASK_MSG_T *task_msg;
	OS_MSG_SIZE msg_size;
	uint32_t last_time=0;
	//uart3_data_package_t test_buf[100]={0};
	//int len,i;
	OS_ERR os_err;
	p_arg=p_arg;

	//gps led
	/*PINSEL_ConfigPin(0, 13, 0);
	GPIO_SetDir(0, (1<<13), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(0, (1<<13), 1);*/

	OSSemCreate(&gps_lock,"gps_lock",1,&os_err);
	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf ("create gps_lock failed !!\r\n");
	}

	#if 1
	{
	BEIJING_TIME_T current_time;
	current_time.year=2016;
	current_time.month=6;
	current_time.day=19;
	current_time.hour=23;
	current_time.minute=00;
	current_time.second=20;
	write_rtc(current_time);
	}
	#endif

	
	while(1)
	{
		task_msg = OSTaskQPend(0,OS_OPT_PEND_NON_BLOCKING,&msg_size,NULL,&os_err);
		if(NULL != task_msg)
		{
			GpsTaskMsg_handle(task_msg);
			user_free(task_msg,__FUNCTION__);
		}

		
		//user_delay_ms(TIME_10ms);

		read_data_from_linkbuf();
		gps_led_ctl();
		
		
		if(user_get_os_time()-last_time>T_3S)
		{
			last_time=user_get_os_time();
			InfoPrintf("GpsTask loop...\r\n");
		}
		
		OSSchedRoundRobinYield(&os_err);
	}

}


