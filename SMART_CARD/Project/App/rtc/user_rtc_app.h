#ifndef _USER_RTC_APP_H
#define _USER_RTC_APP_H

#include "user_system_cfg.h"

#include <stdint.h>


void user_open_rtc(void);
int get_rtc_init_flag(void);
int get_current_time(BEIJING_TIME_T *current_time);
void write_rtc(BEIJING_TIME_T current_time);
void printf_time_now(void);

#endif

