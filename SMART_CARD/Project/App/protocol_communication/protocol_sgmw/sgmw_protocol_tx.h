#ifndef __SGWM_PROTOCOL_TX_H
#define __SGWM_PROTOCOL_TX_H
#include "user_system_cfg.h"

#ifdef USE_PROTOCOL_SGMW

#include "os_cfg_app.h"			//TCB,STACK,PRIORITY,任务间消息的结构体定义和宏
#include "os.h"


#define SGMW_DEV_ID_LEN 				(4)
#define SGMW_AUTHORIZE_TYPE_LEN 		(1)
#define SGMW_AUTHORIZE_KEY_LEN			(16)
#define SGMW_AUTHORIZE_CONTENT_FLAG_LEN (1)

#define SGMW_TM_BARCODE_LEN 			(27)
#define SGMW_SIM_SN_LEN 				(13)
#define SGMW_CCID_SN_LEN				(20)
#define SGMW_VIN_LEN					(17)














#define SGMW_PROTOCOL_FIX 0x7E
#define SGMW_PT_FRAME_MININUM_LEN (9)

#define SGWM_PROTOCL_ESCAPE_CODE  0x7D//0x7E-->0x7D 0x02,0x7D-->0x7D 0x01,

#define GENERAL_ACK_MSG_ID       0x0000	//通用应答消息ID
#define UP_LOGIN_MSG_ID          0x0F01	//终端向平台登录鉴权
#define DN_LOGIN_ACK_MSG_ID      0x0C01	//平台应答终端登陆

#define UP_REPORT_MSG_ID         0x0F02	//终端上报数据，平台用通用应答消息ID作应答

#define UP_REQ_RSA_KEY_ID		 0x0F03
#define DN_ACK_RSA_KEY_ID		 0x0F04

#define UP_REP_AES_KEY_ID		 0x0F05
#define DN_ACK_AES_KEY_ID		 0x0F06

#define DN_DOWN_DBC_FILE_MSG_ID            0x0E01//平台下发DBC文件，终端用通用应答消息ID作应答
#define DN_GET_DEFAULT_DBC_MSG_ID          0x0E05//平台查看默认DBC文件，
#define UP_GET_DEFAULT_DBC_ACK_MSG_ID      0x0C05//终端应答 平台查看默认DBC文件，

#define DN_SET_DEFAULT_DBC_MSG_ID          0x0B05//平台修改默认DBC文件，终端用通用应答消息ID作应答

#define DN_GET_DBC_LIST_MSG_ID          	0x0E06//平台查看默认DBC文件列表，
#define UP_GET_DBC_LIST_ACK_MSG_ID      	0x0C06//终端应答 DBC文件列表，

#define DN_DEL_DBC_FILE_MSG_ID          	0x0B06//平台删除DBC文件，，终端用通用应答消息ID作应答

#define DN_GET_VER_MSG_ID              	 	0x0E02//平台查看终端版本，
#define UP_GET_VER_MSG_ID               	0x0C02//终端应答 平台查看终端版本，

#define DN_GET_REPORT_PERIOD_MSG_ID     	0x0E03//平台查看上传频率
#define UP_GET_REPORT_PERIOD_MSG_ID     	0x0C03//终端应答 上传频率
#define DN_SET_REPORT_PERIOD_MSG_ID     	0x0B03//设置 上传频率，终端使用通用应答


#define DN_GET_HEARTBEAT_MSG_ID         	0x0E04//查看心跳频率
#define UP_GET_HEARTBEAT_ACK_MSG_ID     	0x0C04//终端应答 查看心跳频率
#define DN_SET_HEARTBEAT_MSG_ID         	0x0B04//设置心跳频率，终端使用通用应答


#define DN_GET_USE_FILE_TYPE_MSG_ID       	0x0E07//平台查看终端采用的是哪种文件回传，DBC/TBC
#define UP_GET_USE_FILE_TYPE_ACK_MSG_ID   	0x0C07//终端应答 
#define DN_SET_USE_FILE_TYPE_MSG_ID       	0x0B07//设置终端采用哪种文件，终端使用通用应答

#define DN_GET_SAVE_DATA_EN_MSG_ID        	0x0E08//平台查看终端是否保存数据
#define UP_GET_SAVE_DATA_EN_ACK_MSG_ID    	0x0C08//终端应答 
#define DN_SET_SAVE_DATA_EN_MSG_ID        	0x0B08//设置终端是否保存数据，终端使用通用应答


#define DN_GET_SAVE_DATA_TIME_MSG_ID        0x0E09//平台查看终端保存数据时间
#define UP_GET_SAVE_DATA_TIME_ACK_MSG_ID    0x0C09//终端应答 
#define DN_SET_SAVE_DATA_TIME_MSG_ID        0x0B09//设置终端保存数据时间，终端使用通用应答，0:atuo,>1,天


#define DN_GET_SD_STATE_MSG_ID        		0x0E10//平台查看终端SD卡状态
#define UP_GET_SD_STATE_ACK_MSG_ID    		0x0C10//终端应答 

#define DN_GET_SD_VOL_MSG_ID        		0x0E11//平台查看终端SD卡容量
#define UP_GET_SD_VOL_ACK_MSG_ID    		0x0C11//终端应答 ，Mb

#define DN_GET_SD_USED_MSG_ID        		0x0E12//平台查看终端SD卡已用大小
#define UP_GET_SD_USED_ACK_MSG_ID    		0x0C12//终端应答 ,Mb


#define DN_GET_REMOTE_NET_PARAM_MSG_ID    	0x0E13//平台查看终端当前的IP端口号
#define UP_GET_REMOTE_NET_PARAM_ACK_MSG_ID 	0x0C13//查看终端当前的IP端口号应答 
#define DN_SET_REMOTE_NET_PARAM_MSG_ID    	0x0B13//修改当前IP/域名端口号

#define NEW_UPDATE_PACKEGE_ID           	0x0E14//新升级包指令 
#define REQUEST_UPDATE_PACKEGE_ID       	0x0E15//请求升级包
#define REQUEST_UPDATE_PACKEGE_ACK_ID   	0x0C15//请求申请包应答


#pragma pack(1)
typedef struct __sgwm_general_ack_t
{
	unsigned short 	serial;
	unsigned short 	msg_id;
	unsigned char 	result;
}sgwm_general_ack_t;
#pragma pack()

typedef struct __sgwm_time_t
{
	unsigned char year_ge:4;
	unsigned char year_shi:3;
	unsigned char re_report:1;
	
	unsigned char month_ge:4;
	unsigned char month_shi:4;
	
	unsigned char day_ge:4;
	unsigned char day_shi:4;
	
	unsigned char hour_ge:4;
	unsigned char hour_shi:4;
	
	unsigned char minute_ge:4;
	unsigned char minute_shi:4;
	
	unsigned char second_ge:4;
	unsigned char second_shi:4;
	
	unsigned char sec_mili_bai:4;
	unsigned char sec_mili_qian:4;
	
	unsigned char sec_mili_ge:4;
	unsigned char sec_mili_shi:4;
}sgwm_protocol_time_format_t;

typedef struct __sgwm_power_voltage_t  //供电电压表
{
    unsigned char stor_bat_ge:4;   //蓄电池
    unsigned char stor_bat_shi:4;
	
    unsigned char stor_bat_baifen:4;
    unsigned char stor_bat_shifen:4;
	
	unsigned char lith_bat_shifen:4; //锂电池
	unsigned char lith_bat_ge:4;
	
	unsigned char lith_bat_qianfen:4;
	unsigned char lith_bat_baifen:4;
}sgwm_power_voltage_t;

typedef struct __sgwm_gsm_gps_status_t //GSM和GPS的状态表 ( 3 bytes)
{
   unsigned char gps_valid:1;  			// 1定位0未定位
   unsigned char can_rec_valid:1; 		// 1接收到数据0未
   unsigned char dev_breakdown_valid:1;	// 1有故障 0未
   unsigned char reserved:5;  			// 保留
   
   unsigned char gps_num_ge:4;     		//gps颗数
   unsigned char gps_num_shi:4;
   
   unsigned char gsm_signal_ge:4;  		//gsm信号强度
   unsigned char gsm_signal_shi:4;
}sgwm_gsm_gps_status_t;

typedef struct __sgwm_md5_t  // md5值
{
  unsigned char md5_buf[16];
}sgwm_md5_t;



typedef struct __sgwm_rec_file_t//接收到的DBC文件
{
   unsigned char 	file_type;		//文件标识1dbc文件 2 tbc文件
   unsigned short 	rec_file_len;	//rx file data len
   unsigned short 	file_len;		//文件长度
   unsigned char   *file;			//文件数据
   unsigned char   *file_name;		//文件名数据
   unsigned char 	file_name_len;	//文件名长度
   char 			file_md5[16];	//文件MD5值
}sgwm_rec_file_t;

typedef struct __sgwm_gps_t
{
	//unsigned char valid;
/*  ============ 经度==longitude================ */	
	unsigned char lng_du_shi:4;//度十位
	unsigned char lng_du_bai:2;//度百位
	unsigned char lng_ew:2;//00:east longitude,01:west 
	
	unsigned char lng_fen_shi:4;//分 十位
	unsigned char lng_du_ge:4;//度个位
	
	unsigned char lng_fen_shifen:4;//分 十分位
	unsigned char lng_fen_ge:4;//分 个位

	unsigned char lng_fen_qianfen:4;//分 千分位
	unsigned char lng_fen_baifen:4;//分 百分位

	unsigned char lng_fen_shiwanfen:4;//分 十万分位
	unsigned char lng_fen_wanfen:4;//分 万分位
/*	============ 纬度====latitude============== */	
	
	unsigned char lat_du_shi:4;//度十位
	unsigned char lat_du_bai:2;//度百位
	unsigned char lat_ns:2;//00: south latitude,01:north latitude 
	
	unsigned char lat_fen_shi:4;//分 十位
	unsigned char lat_du_ge:4;//度个位
	
	unsigned char lat_fen_shifen:4;//分 十分位
	unsigned char lat_fen_ge:4;//分 个位

	unsigned char lat_fen_qianfen:4;//分 千分位
	unsigned char lat_fen_baifen:4;//分 百分位

	unsigned char lat_fen_shiwanfen:4;//分 十万分位
	unsigned char lat_fen_wanfen:4;//分 万分位

/*	============ 高度====hight============== */
	unsigned char high_bai:4;//百位
	unsigned char high_qian:3;//千位
	unsigned char high_lev:1;//00: up water level,01:below water level 

	unsigned char high_ge:4;//个位
	unsigned char high_shi:4;// 十位
	
/*	============ 方向====direct============== */
	unsigned char dir_shi:4;// 十位	
	unsigned char dir_bai:4;//百位

	unsigned char dir_shifen:4;//十分位
	unsigned char dir_ge:4;//个位
/*	============ 速度====speed============== */
    unsigned char speed_shi:4;// 十位 
    unsigned char speed_bai:4;//百位

    unsigned char speed_shifen:4;//十分位
    unsigned char speed_ge:4;//个位
/*	============ 累计里程====ODO============== */
	unsigned char odo_qianwan:4;// 千万位	
	unsigned char odo_yi:4;//亿位

	unsigned char odo_shiwan:4;//十万位
	unsigned char odo_baiwan:4;//百万位

	unsigned char odo_qian:4;//千位
	unsigned char odo_wan:4;//万位

	unsigned char odo_shi:4;//十位
	unsigned char odo_bai:4;//百位

	unsigned char odo_shifen:4;//十分位
	unsigned char odo_ge:4;//个位	
}sgwm_protocol_gps_format_t;


typedef struct xst_msg_attr{
	unsigned short pdu_len:14; //msg body len
	unsigned short aes_flag:1; //AES FLAG
	unsigned short multi_pack_flag:1;// 0:one pack,1:more than one pack
} info_data_t;

#pragma pack(1)
typedef struct __sgwm_protc_head
{
	unsigned short msg_id;
	union {
	uint16_t msg_type;
	info_data_t st_msg_attr;
	};
	unsigned short serial;
//	unsigned short pack_cnt;//tatal pack count
//	unsigned short pack_num;//from 1 to pack_cnt
}sgwm_protc_head_t;
#pragma pack()

typedef struct __sgwm_frame
{
	unsigned char prefix;
	sgwm_protc_head_t st_head;
	unsigned char pdu[1];//just make a ptr
}sgwm_frame_t;



struct login_pack_attr{
	unsigned char TM_barcode_flag:1;
	unsigned char SIM_sn_flag:1;
	unsigned char CCID_sn_flag:1;
	unsigned char Vin_sn_flag:1;
};

#pragma pack(1)
typedef struct __sgwm_login_msg
{
	union{
	unsigned char content_flag;
	struct login_pack_attr ct_flag_bit; 
	};

}sgwm_login_msg_info_data_cfg_t;
#pragma pack()


extern OS_TMR 	period_report_tmr;

extern void 		set_sd_save_status(uint8_t SD_save_status );
extern void 		set_rep_tbc_type(uint8_t report_TBC_type );
extern uint32_t 	get_sd_save_time(void);
extern uint8_t 		get_sd_save_status(void );
extern uint8_t 		get_rep_tbc_type(void );
extern void 		set_sd_save_time(uint32_t  SD_save_time );
extern void  		get_dbc_md5(uint8_t *buf,uint8_t *md5_t,uint8_t len);

extern unsigned short htons(unsigned short n);
extern uint16_t sgmw_DeEscape_package(uint8_t*Dst,uint8_t *Src,uint16_t SrcLen);
extern  int check_escape_bytes_amount(uint8_t *p,int len);
extern void reset_serial_num(void);
extern void sgmw_protocol_format_pwr_voltage_for_rep(sgwm_power_voltage_t *power_vol_t);
extern int sgmw_pack_and_rep_req_rsa_key(void);
extern int sgmw_pack_and_rep_aes_key(uint8_t *aes_key,uint8_t key_len);
extern int sgmw_pack_and_rep_login_frame(void);
extern int sgmw_pack_and_rep_logout_frame(void);
extern int  normal_ack_to_server(uint16_t serial,uint16_t msg_id,uint8_t result);
extern int  request_update_package(uint8_t *md5,uint8_t serial);
extern void md5_test(void);
extern void ack_server_set_param(uint16_t rec_serial,uint16_t rec_msg_id,uint8_t result);
extern void ack_server_query_param(uint16_t rec_serial,uint16_t rec_msg_id);
extern int sgmw_pack_and_rep_pd_frame(uint8_t *info_data_ptr,uint16_t info_data_len);
extern int sgmw_save_itv(uint8_t *info_data_ptr,uint32_t info_data_len);
extern int sgmw_rep_add_pd_package(uint8_t *info_data_ptr ,uint16_t info_data_len ,
								   uint8_t *info_data_ptr1,uint16_t info_data_len1
						  		  );
extern int sgmw_rec_dbc_tbc_file(unsigned char mult_pack_f,
								 unsigned char *pdu,unsigned short len);
extern 	int 	 sgmw_pack_and_post_pd_rep_info_data_q(void);

extern int get_report_period(void);
extern int get_gprs_send_fun(void);

extern void change_report_period(uint32_t preriod);
extern void period_report_save_task(void*p_para);
void remote_report_task(void*p_para);

#endif
#endif

