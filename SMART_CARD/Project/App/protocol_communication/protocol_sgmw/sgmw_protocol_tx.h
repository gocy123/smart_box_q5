#ifndef __SGWM_PROTOCOL_TX_H
#define __SGWM_PROTOCOL_TX_H
#include "user_system_cfg.h"

#ifdef USE_PROTOCOL_SGMW

#include "os_cfg_app.h"			//TCB,STACK,PRIORITY,�������Ϣ�Ľṹ�嶨��ͺ�
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

#define GENERAL_ACK_MSG_ID       0x0000	//ͨ��Ӧ����ϢID
#define UP_LOGIN_MSG_ID          0x0F01	//�ն���ƽ̨��¼��Ȩ
#define DN_LOGIN_ACK_MSG_ID      0x0C01	//ƽ̨Ӧ���ն˵�½

#define UP_REPORT_MSG_ID         0x0F02	//�ն��ϱ����ݣ�ƽ̨��ͨ��Ӧ����ϢID��Ӧ��

#define UP_REQ_RSA_KEY_ID		 0x0F03
#define DN_ACK_RSA_KEY_ID		 0x0F04

#define UP_REP_AES_KEY_ID		 0x0F05
#define DN_ACK_AES_KEY_ID		 0x0F06

#define DN_DOWN_DBC_FILE_MSG_ID            0x0E01//ƽ̨�·�DBC�ļ����ն���ͨ��Ӧ����ϢID��Ӧ��
#define DN_GET_DEFAULT_DBC_MSG_ID          0x0E05//ƽ̨�鿴Ĭ��DBC�ļ���
#define UP_GET_DEFAULT_DBC_ACK_MSG_ID      0x0C05//�ն�Ӧ�� ƽ̨�鿴Ĭ��DBC�ļ���

#define DN_SET_DEFAULT_DBC_MSG_ID          0x0B05//ƽ̨�޸�Ĭ��DBC�ļ����ն���ͨ��Ӧ����ϢID��Ӧ��

#define DN_GET_DBC_LIST_MSG_ID          	0x0E06//ƽ̨�鿴Ĭ��DBC�ļ��б�
#define UP_GET_DBC_LIST_ACK_MSG_ID      	0x0C06//�ն�Ӧ�� DBC�ļ��б�

#define DN_DEL_DBC_FILE_MSG_ID          	0x0B06//ƽ̨ɾ��DBC�ļ������ն���ͨ��Ӧ����ϢID��Ӧ��

#define DN_GET_VER_MSG_ID              	 	0x0E02//ƽ̨�鿴�ն˰汾��
#define UP_GET_VER_MSG_ID               	0x0C02//�ն�Ӧ�� ƽ̨�鿴�ն˰汾��

#define DN_GET_REPORT_PERIOD_MSG_ID     	0x0E03//ƽ̨�鿴�ϴ�Ƶ��
#define UP_GET_REPORT_PERIOD_MSG_ID     	0x0C03//�ն�Ӧ�� �ϴ�Ƶ��
#define DN_SET_REPORT_PERIOD_MSG_ID     	0x0B03//���� �ϴ�Ƶ�ʣ��ն�ʹ��ͨ��Ӧ��


#define DN_GET_HEARTBEAT_MSG_ID         	0x0E04//�鿴����Ƶ��
#define UP_GET_HEARTBEAT_ACK_MSG_ID     	0x0C04//�ն�Ӧ�� �鿴����Ƶ��
#define DN_SET_HEARTBEAT_MSG_ID         	0x0B04//��������Ƶ�ʣ��ն�ʹ��ͨ��Ӧ��


#define DN_GET_USE_FILE_TYPE_MSG_ID       	0x0E07//ƽ̨�鿴�ն˲��õ��������ļ��ش���DBC/TBC
#define UP_GET_USE_FILE_TYPE_ACK_MSG_ID   	0x0C07//�ն�Ӧ�� 
#define DN_SET_USE_FILE_TYPE_MSG_ID       	0x0B07//�����ն˲��������ļ����ն�ʹ��ͨ��Ӧ��

#define DN_GET_SAVE_DATA_EN_MSG_ID        	0x0E08//ƽ̨�鿴�ն��Ƿ񱣴�����
#define UP_GET_SAVE_DATA_EN_ACK_MSG_ID    	0x0C08//�ն�Ӧ�� 
#define DN_SET_SAVE_DATA_EN_MSG_ID        	0x0B08//�����ն��Ƿ񱣴����ݣ��ն�ʹ��ͨ��Ӧ��


#define DN_GET_SAVE_DATA_TIME_MSG_ID        0x0E09//ƽ̨�鿴�ն˱�������ʱ��
#define UP_GET_SAVE_DATA_TIME_ACK_MSG_ID    0x0C09//�ն�Ӧ�� 
#define DN_SET_SAVE_DATA_TIME_MSG_ID        0x0B09//�����ն˱�������ʱ�䣬�ն�ʹ��ͨ��Ӧ��0:atuo,>1,��


#define DN_GET_SD_STATE_MSG_ID        		0x0E10//ƽ̨�鿴�ն�SD��״̬
#define UP_GET_SD_STATE_ACK_MSG_ID    		0x0C10//�ն�Ӧ�� 

#define DN_GET_SD_VOL_MSG_ID        		0x0E11//ƽ̨�鿴�ն�SD������
#define UP_GET_SD_VOL_ACK_MSG_ID    		0x0C11//�ն�Ӧ�� ��Mb

#define DN_GET_SD_USED_MSG_ID        		0x0E12//ƽ̨�鿴�ն�SD�����ô�С
#define UP_GET_SD_USED_ACK_MSG_ID    		0x0C12//�ն�Ӧ�� ,Mb


#define DN_GET_REMOTE_NET_PARAM_MSG_ID    	0x0E13//ƽ̨�鿴�ն˵�ǰ��IP�˿ں�
#define UP_GET_REMOTE_NET_PARAM_ACK_MSG_ID 	0x0C13//�鿴�ն˵�ǰ��IP�˿ں�Ӧ�� 
#define DN_SET_REMOTE_NET_PARAM_MSG_ID    	0x0B13//�޸ĵ�ǰIP/�����˿ں�

#define NEW_UPDATE_PACKEGE_ID           	0x0E14//��������ָ�� 
#define REQUEST_UPDATE_PACKEGE_ID       	0x0E15//����������
#define REQUEST_UPDATE_PACKEGE_ACK_ID   	0x0C15//���������Ӧ��


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

typedef struct __sgwm_power_voltage_t  //�����ѹ��
{
    unsigned char stor_bat_ge:4;   //����
    unsigned char stor_bat_shi:4;
	
    unsigned char stor_bat_baifen:4;
    unsigned char stor_bat_shifen:4;
	
	unsigned char lith_bat_shifen:4; //﮵��
	unsigned char lith_bat_ge:4;
	
	unsigned char lith_bat_qianfen:4;
	unsigned char lith_bat_baifen:4;
}sgwm_power_voltage_t;

typedef struct __sgwm_gsm_gps_status_t //GSM��GPS��״̬�� ( 3 bytes)
{
   unsigned char gps_valid:1;  			// 1��λ0δ��λ
   unsigned char can_rec_valid:1; 		// 1���յ�����0δ
   unsigned char dev_breakdown_valid:1;	// 1�й��� 0δ
   unsigned char reserved:5;  			// ����
   
   unsigned char gps_num_ge:4;     		//gps����
   unsigned char gps_num_shi:4;
   
   unsigned char gsm_signal_ge:4;  		//gsm�ź�ǿ��
   unsigned char gsm_signal_shi:4;
}sgwm_gsm_gps_status_t;

typedef struct __sgwm_md5_t  // md5ֵ
{
  unsigned char md5_buf[16];
}sgwm_md5_t;



typedef struct __sgwm_rec_file_t//���յ���DBC�ļ�
{
   unsigned char 	file_type;		//�ļ���ʶ1dbc�ļ� 2 tbc�ļ�
   unsigned short 	rec_file_len;	//rx file data len
   unsigned short 	file_len;		//�ļ�����
   unsigned char   *file;			//�ļ�����
   unsigned char   *file_name;		//�ļ�������
   unsigned char 	file_name_len;	//�ļ�������
   char 			file_md5[16];	//�ļ�MD5ֵ
}sgwm_rec_file_t;

typedef struct __sgwm_gps_t
{
	//unsigned char valid;
/*  ============ ����==longitude================ */	
	unsigned char lng_du_shi:4;//��ʮλ
	unsigned char lng_du_bai:2;//�Ȱ�λ
	unsigned char lng_ew:2;//00:east longitude,01:west 
	
	unsigned char lng_fen_shi:4;//�� ʮλ
	unsigned char lng_du_ge:4;//�ȸ�λ
	
	unsigned char lng_fen_shifen:4;//�� ʮ��λ
	unsigned char lng_fen_ge:4;//�� ��λ

	unsigned char lng_fen_qianfen:4;//�� ǧ��λ
	unsigned char lng_fen_baifen:4;//�� �ٷ�λ

	unsigned char lng_fen_shiwanfen:4;//�� ʮ���λ
	unsigned char lng_fen_wanfen:4;//�� ���λ
/*	============ γ��====latitude============== */	
	
	unsigned char lat_du_shi:4;//��ʮλ
	unsigned char lat_du_bai:2;//�Ȱ�λ
	unsigned char lat_ns:2;//00: south latitude,01:north latitude 
	
	unsigned char lat_fen_shi:4;//�� ʮλ
	unsigned char lat_du_ge:4;//�ȸ�λ
	
	unsigned char lat_fen_shifen:4;//�� ʮ��λ
	unsigned char lat_fen_ge:4;//�� ��λ

	unsigned char lat_fen_qianfen:4;//�� ǧ��λ
	unsigned char lat_fen_baifen:4;//�� �ٷ�λ

	unsigned char lat_fen_shiwanfen:4;//�� ʮ���λ
	unsigned char lat_fen_wanfen:4;//�� ���λ

/*	============ �߶�====hight============== */
	unsigned char high_bai:4;//��λ
	unsigned char high_qian:3;//ǧλ
	unsigned char high_lev:1;//00: up water level,01:below water level 

	unsigned char high_ge:4;//��λ
	unsigned char high_shi:4;// ʮλ
	
/*	============ ����====direct============== */
	unsigned char dir_shi:4;// ʮλ	
	unsigned char dir_bai:4;//��λ

	unsigned char dir_shifen:4;//ʮ��λ
	unsigned char dir_ge:4;//��λ
/*	============ �ٶ�====speed============== */
    unsigned char speed_shi:4;// ʮλ 
    unsigned char speed_bai:4;//��λ

    unsigned char speed_shifen:4;//ʮ��λ
    unsigned char speed_ge:4;//��λ
/*	============ �ۼ����====ODO============== */
	unsigned char odo_qianwan:4;// ǧ��λ	
	unsigned char odo_yi:4;//��λ

	unsigned char odo_shiwan:4;//ʮ��λ
	unsigned char odo_baiwan:4;//����λ

	unsigned char odo_qian:4;//ǧλ
	unsigned char odo_wan:4;//��λ

	unsigned char odo_shi:4;//ʮλ
	unsigned char odo_bai:4;//��λ

	unsigned char odo_shifen:4;//ʮ��λ
	unsigned char odo_ge:4;//��λ	
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

