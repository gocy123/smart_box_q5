#ifndef _MODEM_SIM6320_AT_PRO_H
#define _MODEM_SIM6320_AT_PRO_H
#include "os_cfg_app.h"			//TCB,STACK,PRIORITY,任务间消息的结构体定义和宏

#ifdef USE_SIM6320_MODEM

int at_pro_2g_module_data(char *at_gprs_mode,char *wait_cipsend_ack);

#endif

#endif



