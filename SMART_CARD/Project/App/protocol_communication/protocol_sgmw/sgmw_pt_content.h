#ifndef _SGMW_PT_CONTENT_H
#define _SGMW_PT_CONTENT_H

#include "user_system_cfg.h"

#ifdef USE_PROTOCOL_SGMW

#include "stdint.h"


#if 0

extern uint8_t sgmw_tm_barcode[];
extern uint8_t sgmw_tm_sim[];
extern uint8_t sgmw_tm_ccid[];//898602b2211530000179
extern uint8_t sgmw_tm_vin[];


extern char 	*rd_barcode(void);
extern uint8_t  get_mount_flag(void);
extern char 	*get_sim_ccid (void);
extern uint16_t get_main_power_voltage(void);
extern uint8_t  get_no_bat_state(void);
extern uint16_t get_battery_voltage(void);
extern uint32_t WriteToAppArea(uint32_t *FlashAddr,unsigned char *buf,uint32_t len);
extern int get_upgradefile_md5(uint8_t *ret_md5,uint32_t addr_t,uint32_t file_len);
extern uint32_t tbc_get_report_buf_size(void);
extern void tbc_clear_report_data_buf(void);
extern void tbc_clear_report_update_flag(void);
extern uint8_t get_acc_status(void);
extern int get_default_tbc_name(uint8_t * file_name_t);
extern char * get_version(void);
extern uint32_t read_file_exist_flag(uint8_t i);
extern void  get_tbc_file_list(uint8_t *buf,uint16_t *len);
extern unsigned int get_heartbeat_period(void);
extern int Get_Emmc_Info(uint8_t pdrv, uint8_t cmd, void *pBuf);
extern void feed_wdt(void);
extern void read_tbc_file_name(uint8_t *pbuf,uint8_t i);
extern uint32_t  read_tbc_file_total(uint8_t i);
extern int write_multi_tbcfile_param(uint32_t f_type,uint8_t *name,uint32_t f_name_len,\
							  		 uint32_t t_add,uint32_t f_file_len,uint8_t *t_file_md5,\
							 		 uint32_t default_f,uint32_t t_total,uint32_t f_exist_flag,\
							  		 uint8_t i);
extern void set_sd_save_status(uint8_t SD_save_status );
extern void set_rep_tbc_type(uint8_t report_TBC_type );
extern uint32_t get_sd_save_time(void);
extern uint8_t get_sd_save_status(void );
extern uint8_t get_rep_tbc_type(void );
extern void set_sd_save_time(uint32_t  SD_save_time );
extern void get_dbc_md5(uint8_t *buf,uint8_t *md5_t,uint8_t len);
extern int sgmw_get_dev_id(void);


#endif

















#endif

#endif

