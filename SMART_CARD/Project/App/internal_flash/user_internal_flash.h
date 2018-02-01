#ifndef USER_INTERNAL_FLASH_H
#define USER_INTERNAL_FLASH_H

#include <stdint.h>
#include "user_rf_app.h"

//单片机的内部FLASH的最后的4K FLASH 用来保存参数(61440~~65535) 
#define LF_WAKEUP_ID_ADDR_HOME  ((u32)0x0800EC00L) //    0--1023 保存唤醒ID	

#define REMOTE_ID_ADDR_HOME   	((u32)0x0800F000L) //    0--1023 保存唤醒ID	
#define REMOTE_LOQ_ADDR_HOME  	((u32)0x0800F400L) // 1034--4095 保存LOQ值
#define REMOTE_LOQ_ADDR_END   	((u32)0x08010000L) 


int internal_flash_erase(uint32_t start_Page_Address,uint16_t page_num);
int internal_flash_wr_ecartek(uint32_t flash_word_Address,uint32_t data_32);//0x8000EC00
int internal_flash_wr_common(uint32_t flash_word_Address,uint8_t *ptr,uint32_t len_byte);//0x8000EC00
int internal_flash_rd_common(uint32_t flash_word_Address,uint8_t *rd_buf,uint32_t rd_len);//0x8000EC00
uint32_t internal_flash_rd(uint32_t flash_word_addres);

bool FLASH_WriteRemoteID(REMOTE_STATUS* remote); 
bool FLASH_WriteRemoteLOQ(REMOTE_STATUS* remote);
bool FLASH_ReadRemoteID(REMOTE_STATUS* remote); 
bool FLASH_ReadRemoteLOQ(REMOTE_STATUS* remote); 
#endif
