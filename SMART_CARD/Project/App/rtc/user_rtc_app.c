#include <stdint.h>

#include "os.h"
#include "os_cfg_app.h"

#include "heads.h"

static uint8_t rtc_calibrate_flag=0;
static OS_MUTEX rtc_rd_wr_mutex={0};


#if 1
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  //RCC_LSEConfig(RCC_LSE_ON);
  RCC_LSICmd(ENABLE);
  /* Wait till LSE is ready */
  //while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  //{}
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

void RTC_Configuration1(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  //BKP_DeInit();

  /* Enable LSE */
  //RCC_LSEConfig(RCC_LSE_ON);
  RCC_LSICmd(ENABLE);
  /* Wait till LSE is ready */
  //while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  //{}
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}


#else
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}
#endif
/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval Current time RTC counter value
  */
uint32_t Time_Regulate(void)
{
  uint32_t Tmp_HH = 0xF0, Tmp_MM = 0xF0, Tmp_SS = 0xF0;

  InfoPrintf("\r\n==============Time Settings=====================================");

  /* Return the value to store in RTC counter register */
  return((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
}

/**
  * @brief  Adjusts time.
  * @param  None
  * @retval None
  */
void Rtc_Time_Adjust(uint32_t time_u32)
{
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time */
  RTC_SetCounter(time_u32);//(Time_Regulate());
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}


void user_open_rtc(void)
{
	RTC_TIME_Type RTCFullTime;
	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A6)
	{
		/* RTC Configuration */
		RTC_Configuration();
		/* Adjust time by values entered by the user on the hyperterminal */
		Rtc_Time_Adjust(Time_Regulate());
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A6);
	}
	else
	{
		RTC_Configuration1();
	
		/* Check if the Power On Reset flag is set */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			InfoPrintf("\r\n\n Power On Reset occurred....");
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			InfoPrintf("\r\n\n External Reset occurred....");
		}

		//InfoPrintf("\r\n No need to configure RTC....");
		/* Wait for RTC registers synchronization */
        
		RTC_WaitForSynchro();

		/* Enable the RTC Second */
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}

	/* Clear reset flags */
  	RCC_ClearFlag();
	
	rtc_calibrate_flag=1;
	
}


int get_rtc_init_flag(void)
{
	return rtc_calibrate_flag;
}

int get_current_time(BEIJING_TIME_T *current_time)
{
	RTC_TIME_Type RTCFullTime;
	uint32_t time_u32;
	OS_ERR os_err;
	
  	if(OSRunning==OS_STATE_OS_RUNNING)
	{
		if(rtc_rd_wr_mutex.Type!=OS_OBJ_TYPE_MUTEX)
		{
			OSMutexCreate(&rtc_rd_wr_mutex,"RTC RW MT",&os_err);
		}
		OSMutexPend(&rtc_rd_wr_mutex,1000,OS_OPT_PEND_BLOCKING,NULL,&os_err);
	}
	
	//RTC_GetFullTime (LPC_RTC, &RTCFullTime);
	time_u32=RTC_GetCounter();
	
	localtime(time_u32 , current_time);//1970-2100之间能正确转换

	//if(RTCFullTime.YEAR)  做有效性判断  !!!!!!!!!!
	//  (RTCFullTime.MONTH)(RTCFullTime.DOM)
	//(RTCFullTime.HOUR)(RTCFullTime.MIN)(RTCFullTime.SEC)


	if(OSRunning==OS_STATE_OS_RUNNING)
	{
		OSMutexPost(&rtc_rd_wr_mutex,OS_OPT_POST_NONE,&os_err);
	}
	
    return 0;
}

void write_rtc(BEIJING_TIME_T current_time)
{
   	OS_ERR os_err;
	uint32_t time_u32;
	//合法性判断
	//...
	if(OSRunning==OS_STATE_OS_RUNNING)
	{
		if(rtc_rd_wr_mutex.Type!=OS_OBJ_TYPE_MUTEX)
		{
			OSMutexCreate(&rtc_rd_wr_mutex,"RTC RW MT",&os_err);
		}
		OSMutexPend(&rtc_rd_wr_mutex,1000,OS_OPT_PEND_BLOCKING,NULL,&os_err);
	}
	
	//写入 , 要加一个互斥量 zgc
	//RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, 	(uint32_t)current_time.second);
	//RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, 	(uint32_t)current_time.minute);
	//RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR, 	(uint32_t)current_time.hour);
	
	//RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, 	(uint32_t)current_time.day);
	//RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, 		(uint32_t)current_time.month);
	//RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, 		(uint32_t)current_time.year);

	
	time_u32=mktime (	current_time.year, current_time.month ,current_time.day,
						current_time.hour, current_time.minute,current_time.second);

	Rtc_Time_Adjust(time_u32);

	rtc_calibrate_flag=1;

	if(OSRunning==OS_STATE_OS_RUNNING)
	{
		OSMutexPost(&rtc_rd_wr_mutex,OS_OPT_POST_NONE,&os_err);
	}
}

void printf_time_now(void)
{
	BEIJING_TIME_T current_time;
	uint32_t time_u32;
	OS_ERR os_err;
 	#if 1
	if(OSRunning==OS_STATE_OS_RUNNING)
	{
		if(rtc_rd_wr_mutex.Type!=OS_OBJ_TYPE_MUTEX)
		{
			OSMutexCreate(&rtc_rd_wr_mutex,"RTC RW MT",&os_err);
		}
		
		OSMutexPend(&rtc_rd_wr_mutex,1000,OS_OPT_PEND_BLOCKING,NULL,&os_err);
	}
	
	//要加一个互斥量。。。。 zgc
	//RTC_GetFullTime (LPC_RTC, &RTCFullTime);
	time_u32=RTC_GetCounter();
		
	localtime(time_u32 , &current_time);//1970-2100之间能正确转换

		

	InfoPrintf("current time:%u-%02d-%02d %02d:%02d:%02d\r\n",current_time.year,current_time.month,current_time.day,\
	current_time.hour,current_time.minute,current_time.second);
	if(OSRunning==OS_STATE_OS_RUNNING)
	{
		OSMutexPost(&rtc_rd_wr_mutex,OS_OPT_POST_NONE,&os_err);
	}
	#endif
}


