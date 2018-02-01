#ifndef _UART3_NETWORK_LAYER_H
#define _UART3_NETWORK_LAYER_H

#include <stdint.h>

typedef unsigned char uart3_data_package_t;

void user_uart3_open(void);
void user_uart3_close(void);
void user_uart3_wakeup(void);
void user_uart3_sleep(void);
void user_uart3_reset(void);
int uart3_send_data(uint8_t *buf, uint32_t len);
int uart3_receive_data(uint8_t *buf,int len);














#endif
























