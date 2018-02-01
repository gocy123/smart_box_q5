#ifndef __JT808_TELECOM_PROT_MANAGEH
#define __JT808_TELECOM_PROT_MANAGEH

#include "os.h"
#include "stdint.h"


enum air_protocol_state_t
{
	AIR_PT_STA_CONN_OFF=0,			//δ����socket
	AIR_PT_STA_SOCKET_CREATE_OK,	//����socket ok ,next need to login...
	AIR_PT_STA_LOGIN_OK,			//�ѵ�½(Ӧ�ò�Э��)
	AIR_PT_STA_REQ_LOGOUT,			//������½
	AIR_PT_STA_LOGOUT_OK			//����½(Ӧ�ò�Э��,��socket����)	
};

typedef struct prot_{
	enum air_protocol_state_t 	link_state;
	unsigned char 				login_step;
	unsigned char 				logout_step;

}USER_AIR_PROTOCOL_NET_MANAGE_T;

extern void 	telecom_protocol_manage_task(void*p_para);
#endif

