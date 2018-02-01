#ifndef _HEADS_H
#define _HEADS_H

#include "stm32f10x.h"


#include "ringbuffer.h"
#include "common.h"


#include "uart1_network_layer.h"
#include "uart2_network_layer.h"
#include "uart3_network_layer.h"
#include "user_debug_app.h"



#include "DeviceManage.h"
#include "user_rtc_app.h"
#include "user_gps_app.h"
#include "user_rf_app.h"


#ifdef USE_SIM800_MODEM
#include "modem_sim800_AT_pro.h"
#include "modem_sim800_app.h"
#endif

#ifdef USE_MC20_MODEM
#include "modem_MC20_AT_pro.h"
#include "modem_MC20_app.h"
#endif

#include "jt808_telecom_prot_manage.h"


typedef struct {	/** Seconds Register */	uint32_t SEC;	/** Minutes Register */	uint32_t MIN;	/** Hours Register */	uint32_t HOUR;	/** Day of Month Register */	uint32_t DOM;	/** Day of Week Register */	uint32_t DOW;	/** Day of Year Register */	uint32_t DOY;	/** Months Register */	uint32_t MONTH;	/** Years Register */	uint32_t YEAR;} RTC_TIME_Type;



#endif





