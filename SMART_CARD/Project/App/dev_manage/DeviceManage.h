#ifndef _DEVICEMANAGE_H
#define _DEVICEMANAGE_H
#include <stdint.h>




void *user_malloc(unsigned int size,char const*caller);
void user_free(void *rmem,char const*caller);
void user_delay_ms(uint32_t ms);
uint32_t user_get_os_time(void);



void DeviceManageTask(void *p_arg);

#endif


