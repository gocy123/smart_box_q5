#ifndef __SGMW_PROTOCOL_RX_H
#define __SGMW_PROTOCOL_RX_H
#include "user_system_cfg.h"

#ifdef USE_PROTOCOL_SGMW

#include <stdint.h>
#define ACK_MSG_MAX_NUM  (4)
#define MAX_UPDATE_PARAM_MSG_NUM (2)


extern OS_Q 	rx_server_ack_q;

extern void *pend_recv_gprs_data_q(uint16_t wait_time,uint16_t *msg_size);
extern 	uint8_t sgmw_analysis_modem_gprs_q(uint8_t * ptr,uint16_t len);

extern void remote_receive_task(void *p_para);
#endif
#endif

