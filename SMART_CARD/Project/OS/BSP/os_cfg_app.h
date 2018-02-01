/*
************************************************************************************************************************
*                                                     uC/OS-III
*                                                The Real-Time Kernel
*
*                                  (c) Copyright 2009-2010; Micrium, Inc.; Weston, FL
*                          All rights reserved.  Protected by international copyright laws.
*
*                                       OS CONFIGURATION (APPLICATION SPECIFICS)
*
* File    : OS_CFG_APP.H
* By      : JJL
* Version : V3.02.00
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or 
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your 
*           application/product.   We provide ALL the source code for your convenience and to help you 
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use 
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
************************************************************************************************************************
*/

#ifndef OS_CFG_APP_H
#define OS_CFG_APP_H

#include "os.h"
#include <stdint.h>
#include "common.h"



#define TIME_0ms 			(0u)
#define TIME_1ms 			(1u)
#define TIME_2ms 			(2u)
#define TIME_5ms			(5u)
#define TIME_10ms			(10u)
#define TIME_20ms			(20u)
#define TIME_30ms			(30u)
#define TIME_40ms			(40u)
#define TIME_50ms			(50u)
#define TIME_100ms			(100u)
#define TIME_200ms			(200u)
#define TIME_500ms			(500u)
#define TIME_1S				(1000u)
#define TIME_2S				(2000u)
#define TIME_5S				(5000u)
#define TIME_10S			(10000u)
#define TIME_20S			(20000u)



extern 			OS_Q ISO15765_RX_CAN_MSG_Q;
#define 		ISO15765_RX_CAN_MSG_Q_NUM    (20)

extern 			OS_Q ISO15765_RX_APP_DATA_Q;
#define 		ISO15765_RX_APP_DATA_Q_NUM    (3)

/*
************************************************************************************************************************
*                                                      CONSTANTS
************************************************************************************************************************
*/

                                                            /* --------------------- MISCELLANEOUS ------------------ */
#define  OS_CFG_MSG_POOL_SIZE            100u               /* Maximum number of messages                             */
#define  OS_CFG_ISR_STK_SIZE             128u               /* Stack size of ISR stack (number of CPU_STK elements)   */
#define  OS_CFG_TASK_STK_LIMIT_PCT_EMPTY  10u               /* Stack limit position in percentage to empty            */

                                                          	/* ---------------------- IDLE TASK --------------------- */
#define  OS_CFG_IDLE_TASK_STK_SIZE       128u               /* Stack size (number of CPU_STK elements)                */

                                                            /* ------------------ ISR HANDLER TASK ------------------ */
#define  OS_CFG_INT_Q_SIZE                10u               /* Size of ISR handler task queue                         */
#define  OS_CFG_INT_Q_TASK_STK_SIZE      128u               /* Stack size (number of CPU_STK elements)                */

                                                            /* ------------------- STATISTIC TASK ------------------- */
#define  OS_CFG_STAT_TASK_PRIO            11u               /* Priority                                               */
#define  OS_CFG_STAT_TASK_STK_SIZE       128u               /* Stack size (number of CPU_STK elements)                */
#define  OS_CFG_STAT_TASK_RATE_HZ         10u               /* Rate of execution (10 Hz Typ.)                         */


                                                            /* ------------------------ TICKS ----------------------- */
#define  OS_CFG_TICK_TASK_PRIO             4u               /* Priority                                               */
#define  OS_CFG_TICK_TASK_STK_SIZE       128u               /* Stack size (number of CPU_STK elements)                */
#define  OS_CFG_TICK_RATE_HZ            1000u               /* Tick rate in Hertz (10 to 1000 Hz)                     */
#define  OS_CFG_TICK_WHEEL_SIZE           17u               /* Number of 'spokes' in tick  wheel; SHOULD be prime     */


                                                       		/* ----------------------- TIMERS ----------------------- */
#define  OS_CFG_TMR_TASK_PRIO             11u               /* Priority of 'Timer Task'                               */
#define  OS_CFG_TMR_TASK_STK_SIZE        (128u+64u)//128u zgc               Stack size (number of CPU_STK elements)     
#define  OS_CFG_TMR_TASK_RATE_HZ          10u               /* Rate for timers (10 Hz Typ.)                           */
#define  OS_CFG_TMR_WHEEL_SIZE            17u               /* Number of 'spokes' in timer wheel; SHOULD be prime     */

//
//#define FINSH_TASK_PRIO   4//(OS_LOWEST_PRIO - 7)
#define  FINSH_TASK_PRIO   						(5u)







//(1)================== TCB ====================================
extern OS_TCB  	DeviceManageTask_Tcb;
extern OS_TCB  	Can0ManageTask_Tcb;
extern OS_TCB  	GpsTask_Tcb;
extern OS_TCB  	ISO15765Task_Tcb;
extern OS_TCB  	ModemManageTask_Tcb;
extern OS_TCB  	modem_receive_task_Tcb;
extern OS_TCB  	telecom_prot_manage_task_Tcb;
extern OS_TCB  	remote_report_task_Tcb;
extern OS_TCB  	period_report_save_task_Tcb;
extern OS_TCB  	remote_receive_task_Tcb;
extern OS_TCB  	storage_task_Tcb;
extern OS_TCB  	rf_task_Tcb;

//(2)================= TASK STACK SIZE ==========================
#define  DEVICE_MANAGE_TASK_STK_SIZE			(128u)//(128u)
#define  CAN0_MANAGE_TASK_STK_SIZE				(64u)//(256)
#define  GPS_TASK_STK_SIZE       				(256u)//(150u)               
#define  ISO15765_TASK_STK_SIZE                 (128u)//(128u)
#define  MODEM_MANAGE_TASK_STK_SIZE       		(128u)//(256u) 
#define  MODEM_RECEIV_TASK_STK_SIZE				(128u)//(256u)
#define  TELECOM_PROT_MANAGE_TASK_STK_SIZE		(128u)//(1200u)//(256u)
#define  REMOTE_REPORT_TASK_STK_SIZE			(128u)//(256u)
#define  PERIOD_REP_SAVE_TASK_STK_SIZE			(64u)//(192u)
#define  REMOTE_RECEIVE_TASK_STK_SIZE			(128u)//(256u)
#define  STORAGE_TASK_STK_SIZE 					(64u)//(256u)
#define  RF_TASK_STK_SIZE 						(64u)//(256u)

//(3)================= TASK STACK ==============================
extern CPU_STK 	DeviceManageTask_Stk[];
extern CPU_STK 	Can0ManageTask_Stk[];
extern CPU_STK 	GpsTask_Stk[];
extern CPU_STK 	ISO15765Task_Stk[];
extern CPU_STK 	ModemManageTask_Stk[];
extern CPU_STK  modem_receive_task_Stk[];
extern CPU_STK 	telecom_prot_manage_task_stk[];
extern CPU_STK 	remote_report_task_stk[];
extern CPU_STK 	period_rep_save_task_stk[];
extern CPU_STK 	remote_receive_task_stk[];
extern CPU_STK 	storage_task_stk[];
extern CPU_STK 	rf_task_stk[];

//(4)================= PRIORITY ================================
#define  DEVICE_MANAGE_TASK_PRIO				(7u)
#define  CAN0_MANAGE_TASK_PRIO					(10u)
#define  GPS_TASK_PRIO            				(13u)               
#define  ISO15765_TASK_PRIO            			(9u)               
#define  MODEM_MANAGE_TASK_PRIO            	 	(10u)               
#define  MODEM_RECV_TASK_PRIO           		(11u)  
#define  TELECOM_PROTOCOL_MANAGE_TASK_PRIO		(11u)
#define  REMOTE_REPORT_TASK_PRIO           		(12u)               
#define  PERIOD_REP_SAVE_TASK_PRIO           	(12u)               
#define  REMOTE_RECEIVE_TASK_PRIO           	(12u)               
#define  STORAGE_TASK_PRIO           			(12u)               
#define  RF_TASK_PRIO							(8u)

//(5)================ TASK Q quality ===========================
#define  DEVICE_MANAGE_TASK_MSG_QTY				(2u)
#define  CAN0_MANAGE_TASK_MSG_QTY				(4)
#define  GPS_TASK_MSG_QTY  		  				(4u)
#define  ISO15765_TASK_MSG_QTY  		  		(4u)
#define  MODEM_MANAGE_TASK_MSG_QTY  		  	(4u)
#define  MODEM_RECV_TASK_MSG_QTY  		  		(4u)
#define  TELECOM_PROT_MANAGE_TASK_MSG_QTY		(4u)
#define  REMOTE_REPORT_TASK_MSG_QTY				(10u)
#define  PD_REPORT_SAVE_TASK_MSG_QTY			(6u)
#define  REMOTE_RECEIVE_TASK_MSG_QTY			(6u)
#define  STORAGE_TASK_MSG_QTY					(3u)
#define  RF_TASK_MSG_QTY						(3u)

//(6)================ TASK time_quanta =======================
#define  DEVICE_MANAGE_TASK_TIME_QUANTA			(20u)
#define  CAN0_MANAGE_TASK_TIME_QUANTA			(20u)
#define  GPS_TASK_TIME_QUANTA 		  			(20u)
#define  ISO15765_TASK_TIME_QUANTA  		  	(20u)
#define  MODEM_MANAGE_TASK_TIME_QUANTA  		(20u)
#define  MODEM_RECV_TASK_TIME_QUANTA  		  	(20u)
#define  TELECOM_PROT_MANAGE_TASK_TIME_QUANTA	(20u)
#define  REMOTE_REPORT_TASK_TIME_QUANTA			(20u)
#define  PD_REPORT_SAVE_TASK_TIME_QUANTA		(20u)
#define  REMOTE_RECEIVE_TASK_TIME_QUANTA		(20u)
#define  STORAGE_TASK_TIME_QUANTA				(20u)
#define  RF_TASK_TIME_QUANTA					(20u)

//================= END =====================================


#if 1
enum device_run_step_t
{
	DEV_RUN_STEP_START=0,
	DEV_RUN_STEP_RUNNING,	
	DEV_RUN_STEP_ACC_OFF,
	DEV_RUN_STEP_WAIT_TO_SLEEP,
	DEV_RUN_STEP_START_SLEEP,
	DEV_RUN_STEP_SLEEP,
	DEV_RUN_STEP_START_WAKEUP,
	DEV_RUN_STEP_STANDBY
};

enum acc_state_t
{
	ACC_OFF_PWR=0,
	ACC,
	ACC_ON_PWR,
	ACC_START,
	ACC_OFF_AND_CHARGE,
	ACC_ON_AND_CHARGE,
	ACC_NO_INIT
};

enum pwr_ctrl_t
{
	POWER_CTRL_PWR_OFF=0,	
	POWER_CTRL_SLEEP,
	//POWER_CTRL_STANDBY,
	POWER_CTRL_WAKEUP,
	POWER_CTRL_PWR_ON   		
};

enum send_data_id_t
{
	SEND_PERIOD_REPORT_DATA=0,
	SEND_ALARM_DATA,
	SEND_QRY_ACK_DATA
};
enum rmt_upd_param_id_t
{
	RMT_UPD_TBC_DFT =0,
	RMT_UPD_IP,
	RMT_UPD_REPORT_T
};

//========================= START ========================================
enum task_msg_src_id_t{
	DEV_MANAGE_TASK_ID=0,
	MODEM_MANAGE_TASK_ID,
	MODEM_RECV_TASK_ID,
	CAN_MANAGE_TASK_ID,
	GPS_TASK_ID,	
	REPORT_MANAGE_TASK_ID,
	REMOTE_RECV_TASK_ID,
	REMOTE_UPDATE_TASK_ID,
	
	TEST_TASK_ID
};


enum task_msg_type_t{
	FATHER_MSG_T_ACC_STATUS_INDICATION=0,
	FATHER_MSG_T_TASK_POWER_CTRL,
	FATHER_MSG_T_MODEM_SOCKET_STATE,
	FATHER_MSG_T_REPORT_DATA,
	FATHER_MSG_T_REMOTE_UPDATE_PARAM,
	FATHER_MSG_T_ACK_TO_SERVER_SET,//应答服务器下发的设置结果
	FATHER_MSG_T_ACK_TO_SERVER_QRY,//应答服务器的查询结果
	FATHER_MSG_T_ACK_SLEEP
};


#pragma pack(1)
typedef struct __normal_time
{
	uint16_t year;
	uint8_t  mon;
	uint8_t  day;
	uint8_t  hour;
	uint8_t  min;
	uint8_t  sec;
	uint16_t msec;
}MSG_DATE_TIME_T;
#pragma pack()
#pragma pack(1)
typedef struct  __task_msg_t
{
	enum task_msg_src_id_t 	msg_src_id;
	enum task_msg_type_t 	msg_father_type;
	unsigned short 			msg_son_type; 
	unsigned char 			result;
	MSG_DATE_TIME_T 		msg_time;
	unsigned short 			msg_sn;
}TASK_MSG_T;
#pragma pack()
//========================= END ========================================


/************************/

typedef struct  __remote_result_q_t
{
	unsigned short 			serial;
	unsigned short 			cmd_id;
	enum task_msg_src_id_t  src_id;
	unsigned char 			msg_type;
	unsigned char  			result;
	unsigned char  			pdu[2];
}remote_result_q_t;


#endif

extern void *user_malloc(unsigned int size,char const*caller);
extern void user_free(void *rmem,char const*caller);
extern 	void user_delay_ms(uint32_t ms);
extern uint32_t high_low_switch_32(uint32_t a);
extern uint16_t high_low_switch_16(uint16_t a);



#endif
