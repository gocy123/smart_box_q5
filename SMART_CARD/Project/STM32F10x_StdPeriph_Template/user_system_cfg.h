#ifndef __USER_SYSTEM_CFG_H
#define __USER_SYSTEM_CFG_H



#if 1
//compile config
//-(1)---------------------------------------------------
//#define USE_PROTOCOL_SGMW
	//#define ENABLE_ITV 0
	//#define PROTOCOL_V1_0_2_3 0

#define USE_PROTOCOL_JT808
//#define USE_PROTOCOL_JHN



//-(2)---------------------------------------------------
#define USE_SIM800_MODEM
//#define USE_SIM6320_MODEM
//#define USE_MC20_MODEM

//-(3)---------------------------------------------------
//#define SGMW_IP
//#define WEIGE_IP
//#define ONENET_IP
#define EXLIVE_IP
//#define MOBI_IP
#endif






#if 1

/****************************************************************
	time value define
****************************************************************/
#define T_5MS		(5)

#define T_10MS		(10)
#define T_50MS		(T_10MS*5)//(T_5MS*10)
#define T_100MS		(T_10MS*10)//(T_5MS*20)
#define T_200MS		(T_10MS*20)//T_5MS*40)
#define T_250MS		(T_10MS*25)//(T_5MS*50)
#define T_300MS		(T_10MS*30)//(T_5MS*60)
#define	T_500MS		(T_10MS*50)//(T_5MS*100)
#define	T_600MS		(T_100MS*6)//(T_100MS*6)
#define	T_1500MS	(T_500MS*3)

#define	T_1S		(1000)//200
#define	T_2S		(T_1S*2)
#define	T_3S		(T_1S*3)
#define	T_4S		(T_1S*4)
#define	T_5S		(T_1S*5)
#define	T_6S		(T_1S*6)
#define T_8S        (T_1S*8)
#define	T_9S		(T_1S*9)
#define	T_10S		(T_1S*10)
#define	T_20S		(T_1S*20)
#define	T_25S		(T_1S*25)
#define	T_30S		(T_1S*30)
#define	T_40S		(T_1S*40)

#define T_1MIN		(T_1S*60)
#define T_2MIN		(T_1MIN*2)
#define T_5MIN		(T_1MIN*5)
#define T_10MIN		(T_1MIN*10)
#define T_20MIN		(T_1MIN*20)
#define T_40MIN		(T_1MIN*40)
#define T_1HOUR		(T_1MIN*60)
#define T_1DAY		(24*T_1HOUR)
#define T_360DAY	(360*T_1DAY)

#define	T_RTC_1S	    (1)
#define	T_RTC_2S		(T_1S*2)
#define	T_RTC_3S		(T_1S*3)
#define	T_RTC_4S		(T_1S*4)
#define	T_RTC_5S		(T_1S*5)
#define	T_RTC_6S		(T_1S*6)
#define	T_RTC_7S		(T_1S*7)
#define	T_RTC_8S		(T_1S*8)
#define	T_RTC_9S		(T_1S*9)
#define	T_RTC_10S		(T_1S*10)
#define	T_RTC_20S		(T_1S*20)
#define	T_RTC_25S		(T_1S*25)
#define	T_RTC_30S		(T_1S*30)
#define	T_RTC_40S		(T_1S*40)

#define T_RTC_1MIN		(T_1S*60)
#define T_RTC_2MIN		(T_1MIN*2)
#define T_RTC_5MIN		(T_1MIN*5)
#define T_RTC_10MIN		(T_1MIN*10)
#define T_RTC_20MIN		(T_1MIN*20)
#define T_RTC_40MIN		(T_1MIN*40)
#define T_RTC_1HOUR		(T_1MIN*60)
#define T_RTC_1DAY		(24*T_1HOUR)
#define T_RTC_360DAY	(360*T_1DAY)

#if 1

#if 0
typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Access denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	FR_INVALID_PARAMETER,	/* (19) Given parameter is invalid */
    FR_READ_COMPLETE
} FRESULT;
#endif
#endif



#endif


#endif

