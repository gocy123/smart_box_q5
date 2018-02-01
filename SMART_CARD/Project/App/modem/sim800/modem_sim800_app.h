#ifndef __MODEM_SIM800_APP_H
#define __MODEM_SIM800_APP_H

#include "user_system_cfg.h"

#ifdef USE_SIM800_MODEM

#include "os.h"
#include "stdint.h"

#define MODEM_PWR_SURPPLY_EN_PORT   2
#define MODEM_PWR_SURPPLY_EN_PIN    9

#define MODEM_ON_OFF_KEY_PORT   	1
#define MODEM_ON_OFF_KEY_PIN    	17
#define MODEM_RESET_KEY_PORT   		2
#define MODEM_RESET_KEY_PIN    		2


#define MAX_AT_LEN			  		60			//  ATָ����󳤶�
#define GSM_SENDBUF_LEN         	300
#define GSM_REC_BUF_SIZE        	640
#define LEN_SERIAL_NUM           	10
#define MIN_SIGNAL_QUALITY       	10  		//SignalQuality



enum MODEM_STATE
{
	MODEM_STATE_OFF	=0,
	
	MODEM_STATE_SLEEP,//1
	MODEM_STATE_RESET,//2   
	MODEM_STATE_INIT,//3
	MODEM_STATE_INIT_WAIT,
	MODEM_STATE_CHK_CPIN,//4
	MODEM_STATE_SHUT_LINK,// 5
	MODEM_STATE_CHK_CREG,// 6  
	MODEM_STATE_CHK_NET,// 7
	MODEM_STATE_SET_APN,// 8
	MODEM_STATE_NET_OPEN,// 9
	MODEM_STATE_SOCKET_CREATE,// 10
	MODEM_STATE_LOGIN,// 11
	MODEM_STATE_LOGIN_WAIT,// 12
	MODEM_STATE_GPRS,// 13
	MODEM_STATE_PHONE,// 14
	MODEM_EXIT_GPRS,// 15

	
	MODEM_STATE_NOT_WORK
	
};

enum socket_ctl_t
{
	SOCKET_CTL_CLOSED=0,			//�ر�SOCKET,���ڳ�ʼ��ʱ,����SOCKETΪCLOSED״̬,ֱ���յ����ָ�����������Ӷ���
	SOCKET_CTL_SETUP_AND_KEEP,		//����socket,����IP,�˿�
	SOCKET_CTL_QUIT					//���ѽ���SOCKET��,�˳�SOCKET
};

/*
	����TCP����, SOCKET������ά�� ����صı�����־����(����������);
	�˽ṹ�� ���漰 Ӧ�ò����Э����صĿ���,���½,����,��½��,���Ǳ������report_task��(����Ӧ�ò�).
*/
typedef struct __MODEM {
	unsigned char sig_qty_2g; 			// GSM�ź�ǿ��
	unsigned char reset_cnt;			//������������ģ�����������Ժ󻹲���������������Ҫ�ϵ�
			 char rx_gprs_state;		//�Ƿ������ڽ���GPRS���ݵ�ͨ��״̬ 1:RXing, 0:not RXing gprs data
			 char waiting_cipsend_ack;	//�Ƿ����ڵȴ� > ����
	unsigned char tcp_link_state;		//0: not link to CMIOT ; 1: link to CMIOT ok
	unsigned char socket_link_state;	//0: not setup ; 1: setup ok,and keep OK

	enum socket_ctl_t socket_ctl;
}modem_t,*p_modem_t; 


extern uint8_t 	modem_get_sig_val(void);
extern int 		modem_send_net_data(uint8_t *p_data,uint32_t len);
extern void 	modem_receive_task(void*p_data);
extern int 		modem_ReCreat_socket(void);
extern void 	ModemManageTask(void * p_data);

extern void    	*modem_pend_send_at_rsp_q(uint16_t wait_time,uint16_t *msg_size);
extern void 	*modem_pend_gprs_at_rsp_q(uint16_t wait_time,uint16_t *msg_size);
extern void 	*modem_pend_receive_urc_q(uint16_t wait_time,uint16_t *msg_size);

extern OS_Q 	modem_send_at_rsp_q;
extern OS_Q 	modem_send_gprs_at_rsp_q;
extern OS_Q 	modem_receive_urc_q;
extern OS_Q 	modem_receive_gprs_data_q;

extern void *user_malloc(unsigned int size,char const*caller);
extern void user_free(void *rmem,char const*caller);
extern uint8_t 	get_modem_err_state(void);
#endif

#endif

