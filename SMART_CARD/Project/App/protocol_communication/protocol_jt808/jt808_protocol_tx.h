
#ifndef __JT808_PROTOCOL_TX_H
#define __JT808_PROTOCOL_TX_H

#include "user_system_cfg.h"

#ifdef USE_PROTOCOL_JT808
#include "os.h"


#define SGMW_DEV_ID_LEN 				(4)
#define SGMW_AUTHORIZE_TYPE_LEN 		(1)
#define SGMW_AUTHORIZE_KEY_LEN			(16)
#define SGMW_AUTHORIZE_CONTENT_FLAG_LEN (1)

#define SGMW_TM_BARCODE_LEN 			(27)
#define SGMW_SIM_SN_LEN 				(13)
#define SGMW_CCID_SN_LEN				(20)
#define SGMW_VIN_LEN					(17)


#define JT808_PROTOCOL_FIX 0x7E
#define JT808_PT_FRAME_MININUM_LEN (15)

#define JT808_PROTOCL_ESCAPE_CODE  0x7D//0x7E-->0x7D 0x02,0x7D-->0x7D 0x01,


#define TM_COM_ACK_ID 						0x0001
#define SV_COM_ACK_ID						0x8001
#define TM_HEAT_BEAT_ID						0x0002
#define TM_REGISTER_ID						0x0100
#define SV_ACK_TM_REG_ID					0x8100
#define TM_DEREG_ID							0x0003
#define TM_GET_AUTHORISED_ID				0x0102
#define SV_SET_TM_PARA_ID					0x8103
#define SV_GET_TM_PARA_ID					0x8104
#define TM_ACK_GET_PARA_ID					0x0104
#define SV_CTL_TM_ID						0x8105
#define TM_GPS_INFO_REP_ID					0x0200
#define SV_GET_TM_GPS_INFO_ID				0x8201
#define TM_ACK_GET_GPS_INFO_ID				0x0201
#define SV_TEMP_CTL_TRACK_GPS_ID			0x8300
#define SV_EVENT_SET_ID						0x8301
#define TM_EVENT_REP_ID						0x0301
#define SV_ASK_QUESTION_ID					0x8302
#define TM_ACK_ASK_QUESTION_ID				0x0302
#define SV_INOF_PLAY_MENU_SET_ID			0x8303
#define TM_INFO_PLAY_CTL_REQ_ID				0x0303
#define SV_INFO_SERVICE_ID					0x8304
#define SV_PHONE_CALL_BACK_ID				0x8400
#define SV_SET_TELEPHONE_BOOT_ID			0x8401
#define SV_VEHICLE_CTL_ID					0x8500
#define TM_ACK_VEHICLE_CTL_ID				0x0500
#define SV_SET_CIRCLE_AREA_ID				0x8600
#define SV_DELETE_CIRCULAR_AREA_ID			0x8601
#define SV_SET_RECTANGLE_AREA_ID			0x8602
#define SV_DELETE_RECTANGLE_AREA_ID			0x8603
#define SV_SET_POLYGON_AREA_ID				0x8604
#define SV_DELETE_POLYGON_AREA_ID			0x8605
#define SV_SET_ROUTE_ID						0x8606
#define SV_DELETE_ROUTE_ID					0x8607
#define SV_SAMPLE_TRAVEL_INFO_ID			0x8700
#define TM_TRAVEL_INFO_REP_ID				0x0700
#define SV_SET_TRAVEL_INFO_REC_PARA_ID		0x8701
#define TM_TRANSPORT_BILL_REP				0x0701
#define TM_DRIVER_ID_REP_ID					0x0702
#define TM_MULTI_MEDIA_EVENT_REP_ID			0x0800
#define TM_MULTI_MEDIA_DATA_REP_ID			0x0801
#define SV_ACK_MULTI_MEDIA_DATA_REP_ID		0x8800
#define SV_CTL_TAKING_ONE_PHOTO_ID			0x8801
#define SV_MULTI_MEDIA_DATA_INDEXING_ID		0x8802
#define TM_ACK_MEDIA_DATA_INDEXING_ID		0x0802
#define SV_STORED_MEDIA_DATA_REP_ID			0x8803
#define SV_START_RECORDING_ID				0x8804
#define SV_DATA_DOWN_DIRECTLY_ID			0x8900
#define TM_DATA_UP_DIRECTLY_ID				0x0900
#define SV_RSA_PUBLIC_KEY_ID				0x8A00
#define TM_RSA_PUBLIC_KEY_ID				0x0A00

#define CAR_LICENSE_NUM_LEN 8 
#pragma pack(1)
typedef struct reg_t{

	uint16_t province_id;
	uint16_t town_id;
	uint8_t  manufacturer[5];
	uint8_t  tm_type[20];
	uint8_t  tm_id[7];
	uint8_t  license_plate_color;
	//uint8_t  license_num[18];
	#ifndef ONENET_IP
	uint8_t  license_num[CAR_LICENSE_NUM_LEN];//≥µ≈∆ ‘¡B13579
	#endif
	
}REGISTER_INFO_T;
#pragma pack()

#pragma pack(1)
typedef struct gps_sta_t{

	uint8_t acc_sta:1; //1:acc on 
	uint8_t gps_location_valid:1;//1:gps location valid
	uint8_t latitude_type:1;//1:south lati,0:north lati
	uint8_t longitude_type:1;//1:west longi,0:east longi
	uint8_t bussiness_operate_sta:1;//1:verhicle is in bussiness operate sta,0:is in idle sta,
	uint8_t lati_longi_encrypt_sta:1;//1:encrypt,0:unenc
	uint8_t reserve_1:2;
	
	uint8_t loading_level:2;//0:empty verhi,1:half loading,2:reserve,3:full loading level
	uint8_t oil_way_sta:1;//1:normal,0: oil-way broken(disconnect)
	uint8_t electrical_circuit_sta:1;//1:verhicle electrical circuit ok,0:circuit broken(disconnect)
	uint8_t door_lock_sta:1;//1:door locked ,0:door unlocked
	uint8_t front_door_open_sta:1;//1:open ,0:closed
	uint8_t middle_door_open_sta:1;//1:open ,0:closed
	uint8_t back_door_open_sta:1;//1:open ,0:closed

	uint8_t driver_door_open_sta:1;//1:open ,0:closed
	uint8_t undefine_door_open_sta:1;//1:open ,0:closed
	uint8_t use_gps_to_locate:1;//1:use,0:no use
	uint8_t use_beidou_to_locate:1;//1:use,0:no use
	uint8_t use_glonass_to_locate:1;//1:use,0:no use
	uint8_t use_galileo_to_locate:1;//1:use,0:no use
	uint8_t reserve_2:2;
	
	uint8_t reserve_3:8;

}GPS_INFO_STATUS_T;
#pragma pack()



#pragma pack(1)
typedef struct gps_s{

	uint32_t 				alarm_flags;	//0
	union
	{
		GPS_INFO_STATUS_T 	status;			//4
		uint32_t 			uint32_status;
	};
	uint32_t 				latitude;		//8
	uint32_t 				longitude;		//12
	uint16_t 				altitude;		//16
	uint16_t 				speed;			//18
	uint16_t				direction;		//20
	uint8_t  				date_time[6];	//22

}GPS_INFO_REP_T;				//total=28 bytes
#pragma pack()










extern uint16_t jt808_DeEscape_package(uint8_t*Dst,uint8_t *Src,uint16_t SrcLen);
void remote_report_task(void*p_para);
extern void change_report_period(uint32_t preriod);
extern void period_report_save_task(void*p_para);
extern int jt808_pack_and_rep_register_frame(void);
extern int jt808_pack_and_rep_login_frame(void);







extern OS_TMR 	period_report_tmr;

extern void 		set_sd_save_status(uint8_t SD_save_status );
extern void 		set_rep_tbc_type(uint8_t report_TBC_type );
extern uint32_t 	get_sd_save_time(void);
extern uint8_t 		get_sd_save_status(void );
extern uint8_t 		get_rep_tbc_type(void );
extern void 		set_sd_save_time(uint32_t  SD_save_time );
extern void  		get_dbc_md5(uint8_t *buf,uint8_t *md5_t,uint8_t len);

extern unsigned short htons(unsigned short n);
extern  int check_escape_bytes_amount(uint8_t *p,int len);
extern void reset_serial_num(void);
extern int sgmw_pack_and_rep_req_rsa_key(void);
extern int sgmw_pack_and_rep_aes_key(uint8_t *aes_key,uint8_t key_len);
extern int  normal_ack_to_server(uint16_t serial,uint16_t msg_id,uint8_t result);
extern int  request_update_package(uint8_t *md5,uint8_t serial);
extern void md5_test(void);
extern void ack_server_set_param(uint16_t rec_serial,uint16_t rec_msg_id,uint8_t result);
extern void ack_server_query_param(uint16_t rec_serial,uint16_t rec_msg_id);
extern int sgmw_pack_and_rep_pd_frame(uint8_t *info_data_ptr,uint16_t info_data_len);
extern int sgmw_rep_add_pd_package(uint8_t *info_data_ptr ,uint16_t info_data_len ,
								   uint8_t *info_data_ptr1,uint16_t info_data_len1
						  		  );
extern int sgmw_rec_dbc_tbc_file(unsigned char mult_pack_f,
								 unsigned char *pdu,unsigned short len);

extern int get_report_period(void);
extern int get_gprs_send_fun(void);

#endif
#endif

