#ifndef _JT808_PT_MISC_H
#define _JT808_PT_MISC_H
#include "user_system_cfg.h"

#ifdef USE_PROTOCOL_JT808
#include "stdint.h"











extern uint8_t get_register_ok_flag(void);
extern uint8_t *get_authorize_code(void);

extern void jt808_com_ack_pro(uint8_t *buf,uint16_t len);
extern void jt808_ack_reg_pro(uint8_t *buf,uint16_t len);




#endif
#endif

