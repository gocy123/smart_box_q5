#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#include <stdint.h> 
#include <stdlib.h>
#include <string.h>

typedef struct {
	uint32_t year;//要带上2000
	uint32_t month;
	uint32_t day;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	uint32_t week;
}BEIJING_TIME_T;


unsigned long mktime (	unsigned int year, unsigned int mon,  
    					unsigned int day,  unsigned int hour,  
    					unsigned int min,  unsigned int sec);  
void localtime(unsigned int time , BEIJING_TIME_T *prtc);//1970-2100之间能正确转换


#ifdef __cplusplus
}
#endif

#endif
