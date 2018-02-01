#ifndef _USER_GPS_APP_H
#define _USER_GPS_APP_H

#include <stdint.h>

#define GPS_TIMELEN			(10+1)
#define GPS_NUMSVLEN        4
#define GPS_DATELEN			(6+1)
#define GPS_LATLEN			(11+1)
#define GPS_LONGLEN			(12+1)
#define GPS_SPEEDLEN		(5+1)
#define GPS_SPEEDFXLEN		(7+1)
#define GPS_ALTITUDELEN		(6+1)
typedef  struct s_gps {
	
	//unsigned char  timexxxx[20];	
	unsigned char  time[GPS_TIMELEN];	
	unsigned char  location_valid;
	unsigned char  latitude[GPS_LATLEN];			
	unsigned char  latitude_NS;				
	unsigned char  longitude[GPS_LONGLEN]; 	
	unsigned char  longitude_EW;				
	unsigned char  speed[GPS_SPEEDLEN]; 
	unsigned char  speed_direction[GPS_SPEEDFXLEN]; 
	unsigned char  altitude[GPS_ALTITUDELEN];	
	unsigned char  date[GPS_DATELEN];
	unsigned char  numSV[GPS_NUMSVLEN];
}gps_location_info_T;

enum gps_check_frame_stp{
    step_search_head=0,
    step_search_tail,
    step_search_end
};

#pragma pack(1)
typedef struct __sgwm_time
{
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned short mil_second;
}gps_time_t;
#pragma pack()

extern uint8_t gps_get_south_north(void);
extern uint8_t gps_get_east_west(void);
extern void 	GpsTask(void *p_arg);
extern int  	gps_check_lacation_state(void);
extern void 	gps_get_location_info(gps_location_info_T *ptr);
extern uint8_t  Get_GPS_Number(void);









#endif
