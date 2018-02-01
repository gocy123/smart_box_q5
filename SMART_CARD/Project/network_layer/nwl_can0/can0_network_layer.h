#ifndef _UART0_NETWORK_LAYER_H
#define _UART0_NETWORK_LAYER_H

#include <stdint.h>
#include "drivers.h"


typedef CAN_MSG_Type can0_data_package_t;//

void user_can0_open(void);
void user_can0_close(void);
void user_can0_wakeup(void);
void user_can0_sleep(void);
void user_can0_clear_err_counter(void);
void user_can0_reset(void);
int can0_send_data(uint8_t *buf, uint32_t len);
int can0_receive_data(uint8_t *buf,int read_len);














#endif
























