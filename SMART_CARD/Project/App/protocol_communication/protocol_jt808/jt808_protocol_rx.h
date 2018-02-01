#ifndef __JT808_PROTOCOL_RX_H
#define __JT808_PROTOCOL_RX_H

#include "user_system_cfg.h"

#ifdef USE_PROTOCOL_JT808
#include <stdint.h>

#define ACK_MSG_MAX_NUM  (4)
#define MAX_UPDATE_PARAM_MSG_NUM (2)

#pragma pack(1)
typedef struct jt808_s{
uint8_t  head_fix;
uint16_t msg_id;
uint16_t msg_attribute;
uint8_t  tm_phone_num[6];
uint16_t msg_sn;
uint8_t  pdu[1];

}JT808_PROTCOL_T;
#pragma pack()


extern OS_Q 	rx_server_ack_q;

extern void *pend_recv_gprs_data_q(uint16_t wait_time,uint16_t *msg_size);
extern void remote_receive_task(void *p_para);

#endif

#endif

