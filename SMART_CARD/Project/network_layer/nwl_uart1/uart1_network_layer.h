#ifndef _UART1_NETWORK_LAYER_H
#define _UART1_NETWORK_LAYER_H

#include <stdint.h>

typedef unsigned char uart1_data_package_t;

void user_uart1_open(void);
void user_uart1_close(void);
void user_uart1_wakeup(void);
void user_uart1_sleep(void);
void user_uart1_reset(void);
int uart1_send_data(uint8_t *buf, uint32_t len);
int uart1_receive_data(uint8_t *buf,int len);














#endif
























