#ifndef _UART2_NETWORK_LAYER_H
#define _UART2_NETWORK_LAYER_H

#include <stdint.h>

typedef unsigned char uart2_data_package_t;

void user_uart2_open(void);
void user_uart2_close(void);
void user_uart2_wakeup(void);
void user_uart2_sleep(void);
void user_uart2_reset(void);
int uart2_send_data(uint8_t *buf, uint32_t len);
int uart2_receive_data(uint8_t *buf,int len);














#endif
























