#include "Common.h"
#include "user_rtc_app.h"


void localtime(unsigned int time , BEIJING_TIME_T *prtc)//1970-2100֮������ȷת��
{
	unsigned  int  n32_Pass4year;
	int n32_hpery;
	const char Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	//����ʱ��
	//  time=time-timezone;//��ʱ��

	//  if(time < 0)
	//  {
	//     time = 0;
	//  }
	//ȡ��ʱ��
	prtc->second = time % 60;
	time /= 60;
	//ȡ����ʱ��
	prtc->minute = time % 60;
	time /= 60;
	//ȡ��ȥ���ٸ����꣬ÿ������ 1461*24 Сʱ
	n32_Pass4year= time / (1461 * 24);
	//�������
	prtc->year = (n32_Pass4year << 2)+70;
	//������ʣ�µ�Сʱ��
	time %= 1461 * 24;
	//У������Ӱ�����ݣ�����һ����ʣ�µ�Сʱ��
	for (;;)
	{
		//һ���Сʱ��
		n32_hpery = 365 * 24;
		//�ж�����
		if ((prtc->year & 3) == 0)
		{
            //�����꣬һ�����24Сʱ����һ��
            n32_hpery += 24;
		}
        if (time < n32_hpery)
		{
            break;
		}
        prtc->year++;
        time -= n32_hpery;
    }
    //Сʱ��
    prtc->hour = time % 24;
    //һ����ʣ�µ�����
    time /= 24;
    //�ٶ�Ϊ����
    time++;
    //У��������������·ݣ�����
    if ((prtc->year & 3) == 0)
    {
		if (time > 60)
		{
			time--;
		}
		else
		{
			if (time == 60)
			{
				prtc->month = 1;
				prtc->day = 29;
				goto OVER;
			}
		}
	}
	
	//��������
	for (prtc->month = 0; Days[prtc->month] < time;prtc->month++)
	{
		time -= Days[prtc->month];
	}

	prtc->day = time;
	
	OVER:
	prtc->year += 1900;
	prtc->month++;

}

unsigned char *seg_sec_to_bcd_time(unsigned int sec, unsigned char *bcd_time, int time_zone)
{
    BEIJING_TIME_T tm;
  
    if(NULL == bcd_time) 
        return NULL;
    
    //sec += (time_zone * 3600);
    
    localtime(sec, &tm);
    
    bcd_time[0] = (((tm.year%100)/10) << 4) | (tm.year % 10);
    bcd_time[1] = (((tm.month%100)/10) << 4) | (tm.month % 10);
    bcd_time[2] = (((tm.day%100)/10) << 4) | (tm.day % 10);
    bcd_time[3] = (((tm.hour%100)/10) << 4) | (tm.hour % 10);
    bcd_time[4] = (((tm.minute %100)/10) << 4) | (tm.minute % 10);
    bcd_time[5] = (((tm.second %100)/10) << 4) | (tm.second % 10);
    
    return bcd_time;
}


unsigned long mktime (	unsigned int year, unsigned int mon,  
    					unsigned int day,  unsigned int hour,  
    					unsigned int min,  unsigned int sec)  
{  
    if (0 >= (int) (mon -= 2))  /* 1..12 -> 11,12,1..10 */  
	{   
         mon += 12;      /* Puts Feb last since it has leap day */  
         year -= 1;  
    }  
  
    return (((  
             (unsigned long) (year/4 - year/100 + year/400 + 367*mon/12 + day) +  
             year*365 - 719499  
          )*24 + hour /* now have hours */  
       )*60 + min /* now have minutes */  
    )*60 + sec; /* finally seconds */  
}


unsigned int time(void)
{
	BEIJING_TIME_T stNow;
	unsigned long t;
	int ret;

	ret=get_current_time(&stNow);
	  //��������ת����unixʱ�������
	t = mktime(stNow.year,stNow.month,stNow.day,\
		stNow.hour,stNow.minute,stNow.second);

	return t;
}

int rtc_to_uint32(BEIJING_TIME_T stNow)
{
	int t;
	  //��������ת����unixʱ�������
	t = (int)mktime(stNow.year,stNow.month,stNow.day,\
		stNow.hour,stNow.minute,stNow.second);

	return t;
}



#define BEIJING_TINE_ZONE   (8*60*60)
void LocalTime2UTC(BEIJING_TIME_T *LocalRtc , BEIJING_TIME_T *pUTCRtc)
{
	unsigned long tt;
	tt = mktime(LocalRtc->year,LocalRtc->month,LocalRtc->day,\
		LocalRtc->hour,LocalRtc->minute,LocalRtc->second);
	tt -= BEIJING_TINE_ZONE;
	localtime(tt,pUTCRtc);

}
unsigned int GpsTimeToUnix(char *GpsDate,char *GpsTime)
{
	unsigned int year;
	unsigned int mon;  
    unsigned int day;
	unsigned int hour;  
    unsigned int min;
	unsigned int sec;
	
	year = (GpsDate[4]-'0') *10 +(GpsDate[5]-'0');
	year += 2000;
	mon = (GpsDate[2]-'0') *10 + (GpsDate[3]-'0');
	day = (GpsDate[0]-'0') *10 + (GpsDate[1]-'0');

	hour = (GpsTime[0]-'0')  *10 + (GpsTime[1]-'0');
	min = (GpsTime[2]-'0')  *10 + (GpsTime[3]-'0');
	sec = (GpsTime[4]-'0')  *10 + (GpsTime[5]-'0');

	return mktime(year,mon,day,hour,min,sec);
}

void get_localtime_from_gps(char *gps_date,char *gps_time,BEIJING_TIME_T *rtc_time)
{
	localtime(GpsTimeToUnix(gps_date,gps_time) + BEIJING_TINE_ZONE,rtc_time);
}

uint8_t CharToInt(char c)
{
    uint8_t ucRet = 0;
    
    if(c >= '0' && c <= '9')
        ucRet = c - 0x30;
    else if(c >= 'a' && c <= 'f')
        ucRet = c - 0x61 + 10;
    else if(c >= 'A' && c <= 'F')
        ucRet = c - 0x41 + 10;
    
    return ucRet;
}

void StringToHex(char *pStr, uint8_t *pHexBuf)
{
    uint8_t ucLen, i;
    
    ucLen = strlen((const char *)pStr);
    
    for(i = 0; i < ucLen/2; i++)
    {
        pHexBuf[i] = CharToInt((char)pStr[2*i])*16 + CharToInt((char)pStr[2*i + 1]);
    }
}





