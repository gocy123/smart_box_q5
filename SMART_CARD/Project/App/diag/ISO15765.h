#ifndef _ISO15765_H
#define _ISO15765_H
#include <stdint.h>

#define TST_TX   0
#if TST_TX
//tx
#define  ISO15765_TX_ID 	0x68
#define  ISO15765_RX_ID		0x70
#else
//rx
#define  ISO15765_TX_ID 	0x70
#define  ISO15765_RX_ID		0x68


#endif

/*
	ISO 15765�ܹ���Ϊ4������:
	15765-1:����㼰������·��˵��
	15765-2:�����˵��
	15765-3:Ӧ�ò�˵��
	15765-4:�ŷ����˵��




	���е�����������(3����),������ͳһ�Ľṹ��
	(1)�����������:
		�� ���Ͷ��ϲ�   ʹ��,����������㴫�ݿ�����Ϣ �� ��Ϸ������ݡ�
		
	(2)ȷ�Ϸ�������:
		�� ���Ͷ������ ʹ��,�������ϲ�ȷ��:�ϲ�������ķ����ִ�н��(�ɹ���ʧ��)
		
	(3)ָʾ��������:
		�� ���ն������ ʹ��,�������ϲ�ָʾ:���ݽ��յ�ִ�н��(�ɹ���ʧ��)

��Щ������û��ָ�������Ӧ�ó���ӿ�

��: 

Ӧ�ò��ĳ�����������:

service_name.type{

	para A
	para B
	para C
	...
	...
}

����:

	service_name:��ʾ����ķ���������,��Ӧ�ò�����
	type		:��ʾ�����������(����request��ȷ��confirm��ָʾindication)
	para		:��Ҫ�����Ĳ���
	
*/

//----------------------------------------------------------------------------------
//  �ؼ��������    
//		N 	: �ڴ˴��������ʾ Net ,������
//  N_PDU 	: ����Э�����ݵ�Ԫ�� Net Protocol Data Unit ������Ԫ		
//	   BS   : ���С
//	ST_min  : ������ʱ��
//
//
//
//
//
//
//
//
//
//
//
//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
//       ���������ݵ�Ԫ˵�� start 
//----------------------------------------------------------------------------------
typedef unsigned char 	N_AI_T;//λ������
typedef unsigned char 	N_SA_T;//����Դ��ַ(0x00~~0xFF)  ,�������������ʵ��
typedef unsigned char 	N_TA_T;//����Ŀ���ַ(0x00~~0xFF),��������������ʵ��
typedef enum {
	PHYSICAL_ADD=0,//1 �� 1  ͨ��
	FUNCTIONAL_ADD //1 �� �� ͨ��,����֧�ֵ�֡ͨ��
	
} N_TAtype_T;//����Ŀ���ֵ����,����ͨ��ģʽ��

typedef unsigned char 	N_AE_T;		//�����ֵ��չ��
typedef unsigned short 	N_MSG_LEN_T;//Ҫ���ͻ��߽��յ����ݵĳ���(1~~4095)��
typedef unsigned char * N_MsgData_T;//�ò����������ϲ�ʵ�����н��������ݡ�
//typedef enum {
//	STmin=0,
//	BS 
//} N_Parameter_T;//�ò���ȷ�������Ĳ�����

typedef unsigned char 	N_ParaValue_T;	//�ò��������Э�����<Parameter>��Ϊָʾ����

typedef struct {
	
	//���ͷ� ����CAN֡ �ĺ�ʱ,�����ʱ
	//timeout: 1000ms,defined by ISO15765
	//any pdu
	uint16_t 	N_As; 

	//���շ� ����CAN֡ �ĺ�ʱ,�����ʱ
	//timeout: 1000ms,defined by ISO15765
	//any pdu
	uint16_t	N_Ar; 

	//���ͷ� �ӻ�ȡFF,FC��confirm,��FC��indicate--��-- ���յ���һ������֡ ��ʱ��
	//timeout: 1000ms,defined by ISO15765
	uint16_t 	N_Bs;

	//���շ� �ӻ�ȡFF��indicate,  ��FC��confirm---��-- ������һ������֡   ��ʱ��
	//timeout(undefine)
	uint16_t 	N_Br;

	//���ͷ� �ӻ�ȡFC��confirm����CF��indicate----��-- ������һ������֡   ��ʱ��
	//timeout:undefine
	uint16_t 	N_Cs;

	//���շ� �ӻ�ȡFC��confirm����CF��indicate----��-- ���յ���һ������֡ ��ʱ��
	//timeout: 1000ms,defined by ISO15765
	uint16_t 	N_Cr;

}ISO15765_NETWORK_LY_TIME_T;

typedef enum
{

	N_OK=0,
	N_TIMEOUT_A,//1
	N_TIMEOUT_Bs,//2
	N_TIMEOUT_Cr,
	N_WRONG_SN,
	N_INVALID_FS,//5
	N_UNEXP_PDU,
	N_WFT_OVRN,
	N_BUFFER_OVFLW,//8
	N_ERROR
			 /*	���ͬʱ���������������ϵĴ��������Ӧ
				�����ϱ� ö��ֵ�ϵ͵Ĵ��� ���ϲ� */
} N_Result_T;//	�ò�������������ִ�еĽ��״̬��

typedef enum
{
	N_CHAN_PARA_OK=0,
	N_CHAN_PARA_RX_ON,
	N_CHAN_PARA_WRONG_PARAMETER,
	N_CHAN_PARA_WRONG_VALUE
} N_Result_ChangeParameter_T;//	�ò�������������ִ�еĽ��״̬��Ϣ

//----------------------------------------------------------------------------------
//       ���������ݵ�Ԫ˵�� end 
//----------------------------------------------------------------------------------


typedef struct {
	unsigned char bit0:1;
	unsigned char bit1:1;
	unsigned char bit2:1;
	unsigned char bit3:1;
	unsigned char bit4:1;
	unsigned char bit5:1;
	unsigned char bit6:1;
	unsigned char bit7:1;
	 
} Byte_Bit_T;//


typedef union {
	unsigned char 	Byte_flag;
	Byte_Bit_T 		ByteBit;
	
} Byte_Union_T;//


typedef struct {
	// ConTinue Send ��Ȩ��������	
	unsigned char CTS:1;	
	
	// ��������ȴ�,���и������ֵΪ:N_WFTmax
	unsigned char WAIT:1;	
	
	// �������,������ָʾ������ݵ���֡�б�ʾ���ֽڸ�����Ϣ
	// �����˽����߿ɴ洢����Ϣ����
	unsigned char OVFLW:1;	
	 
} Flow_Control_T;//���ص���ؿ��ơ�״̬��־��


enum FS_sta {
	// ConTinue Send ��Ȩ��������	
	 FS_CTS=0,	
	
	// ��������ȴ�,���и������ֵΪ:N_WFTmax
	 FS_WAIT,	
	
	// �������,������ָʾ������ݵ���֡�б�ʾ���ֽڸ�����Ϣ
	// �����˽����߿ɴ洢����Ϣ����
	 FS_OVFLW
	 
} ;//���ص���ؿ��ơ�״̬��־��






#if 1
#define N_PDU_ERR_TYPE_NONE 			(0X00)
#define N_PDU_ERR_TYPE_SF_DATA_LEN 		(0X01)
#define N_PDU_ERR_TYPE_CF_DATA_LEN 		(0X02)
#define N_PDU_ERR_TYPE_CF_DATA_MAX_LEN 	(0X03)
#define N_PDU_ERR_TYPE_CF_NO_START 		(0X04)
#define N_PDU_ERR_TYPE_CF_SN 			(0X05)
#define N_PDU_ERR_TYPE_CF_BREAK_CF      (0X06)
#define N_PDU_ERR_TYPE_CF_UNEXP_CF      (0X07)

#endif

typedef enum n_usdata_cf_{
	confirm_ok=0,
	confirm_err_timeout,
	confirm_err_flow_ctl,
	confirm_err_overflow,
	confirm_err_len
}N_USData_confirm_t;

typedef enum n_usdata_indi_{
	indicate_ok=0,
	indicate_err_timeout,
	indicate_err_flow_ctl
}N_USData_indicate_t;


//----------------------------------------------------------------------------------
//       NETWORK LAYER PROTOCOL start 
//----------------------------------------------------------------------------------
/*
	�����Э��������¹���:
	(1)���͡��������4095���ֽڵ�������Ϣ
	(2)���淢�͡�������ɵ�״̬

	���ͷ�ʽ: ��֡���ͣ���֡����

	��֡����ʱ����Ϣ�ĳ�������֡(FF_N_PDU)�б����ͣ�������ʹ�õ�����֡������֡
	��������֡: ���շ�ͨ�����ػ��ƽ���������֪ͨ���ͷ�
	            ���ͷ���������֪֡ͨ���շ������շ����ܵ����ֽڵĽ��գ�����Ӧ��
	            �ͷŵķ��͡�
	            ����:���С(BS)-->����Ȩ�������������N_PDU֮ǰ�����շ������ͷ�
	                 ����N_PDU������
	                 ������ʱ��(STmin)-->���ͷ��ڷ�����������֡����ȴ������ʱ�䡣
	                 

	
*/


/*����Э�����ݵ�Ԫ(N_PDU)��4����������:
(1)SF_N_PDU   :single frame 
(2)FF_N_PDU   :first frame
(3)CF_N_PDU   :continue frame
(4)FC_N_PDU   :flow control frame


	N_PDU format
	������3����: [��ַ��Ϣ(N_AI)] [Э�������Ϣ(N_PCI)] [������(N_Data)]

	N_AI	: CAN_ID ??

	N_PCI	: �������ָ�֡�� SF_N_PDU,FF_N_PDU,CF_N_PDU,FC_N_PDU

	N_Data	: data bytes
*/


#define SF_PCI 0x00
#define FF_PCI 0x01
#define CF_PCI 0x02
#define FC_PCI 0x03

//-----DATA0----------------------------------
typedef struct {
	#if 0
	union{
	unsigned char SF_data_len :4; 			   //single frame data lenght
	unsigned char FF_MultiFrame_data_len_low:4;//low 4 bits len value of multi_frame at first frame DATA[0]
	unsigned char CF_SN	:4;					   //serial number of continue frame at continue frame DATA[0] 
	unsigned char CF_FS :4;					   //flow status of continue frame(CTS,WAIT,OVFLW) at continue frame DATA[0]
	}
	#else
	//��֡����Ϣ�峤��,��֡����Ϣ�峤�ȵ�4λ,����֡��serial number,����֡��״̬��
	unsigned char SF_dl_all_FF_dl_low_CF_sn_FC_fs:4;
	#endif
	unsigned char N_PCI_Type:4;
} DATA0_Bit_T;


//-----DATA1----------------------------------
typedef struct {
	union{
		unsigned char SF_data1;
		unsigned char FF_MultiFrame_data_len_high;
		unsigned char CF_data1;
		unsigned char FC_BS;//block size of flow contrle frame 
	}Byte;
} DATA1_Byte_T;

//-----DATA2----------------------------------
typedef struct {
	union{
		unsigned char SF_data2;
		unsigned char FF_MultiFrame_data1;
		unsigned char CF_data1;
		unsigned char FC_STmin;//minimum interval time of two frame ,used by flow control frame
	}Byte;
} DATA2_Byte_T;


#define N_PCI_T DATA0_Bit_T

typedef struct {
	//DATA 0
	unsigned char SF_dl_all_FF_dl_low_CF_sn_FC_fs:4;
	unsigned char N_PCI_Type:4;

	//DATA1
	union{
		unsigned char SF_data1;
		unsigned char FF_MultiFrame_data_len_high;
		unsigned char CF_data1;
		unsigned char FC_BS;//block size of flow contrle frame 
	};

	//DATA2
	union{
		unsigned char SF_data2;
		unsigned char FF_MultiFrame_data1;
		unsigned char CF_data2;
		unsigned char FC_STmin;//minimum interval time of two frame ,used by flow control frame
	};
	//DATA3...7
	unsigned char DATA3;
	unsigned char DATA4;
	unsigned char DATA5;
	unsigned char DATA6;
	unsigned char DATA7;
}N_PDU_T_;

typedef union {
	N_PDU_T_ 		n_pdu_fmt;
	unsigned char 	DATA[8];
} N_PDU_T;//




//----------------------------------------------------------------------------------
//       NETWORK LAYER PROTOCOL end 
//----------------------------------------------------------------------------------

enum pdu_rx_sta{
	PDU_RX_STEP_END=0,
	PDU_RX_STEP_START,
	PDU_RX_STEP_CONTINUE,
	PDU_RX_STEP_UNVALID_DATA,	//abandon this msg,going on receive
	PDU_RX_STEP_ERR_DATA, 		//STOP Receive
	PDU_RX_STEP_ERR_FC,
	PDU_RX_STEP_ERR_MALLOC
};

enum pdu_tx_sta{
	PDU_TX_IDLE=0,
	PDU_TX_START,
	PDU_TX_CONTINUE,
	PDU_TX_END,	
	PDU_TX_ERROR
};




#pragma pack(1)
typedef struct {
	uint16_t had_rx_info_len;	
    uint16_t need_to_rx_info_len;
	uint16_t loc_st_buf_max_len;
	uint8_t  rx_FC_BS_counter;
    
	uint8_t  rx_FC_CTS_flag		:1;
	uint8_t  rx_FC_WAIT_flag	:1;
	uint8_t  rx_FC_OVFLW_flag	:1;
	uint8_t  NWL_now_receiving_flag:1;
    uint8_t  s_reserved:4;

	uint8_t  rx_multi_frm_CF_SN;
/*
	--------------------- FC frame discription --------------------------------------
	---PCI type and FS----
	The initial byte contains the type (type = 3) in the first four bits, and a
	flag in the next four bits indicating if the transfer is allowed (0 = Clear
	To Send, 1 = Wait, 2 = Overflow/abort). 
	---BS----
	The next byte is the block size, the count of frames that may be sent before 
	waiting for the next flow control frame. 
	A value of zero allows the remaining frames to be sent without flow control 
	or delay. 
	---STmin----
	The third byte is the Separation Time (ST), the minimum delay time 
	between frames. 
	ST values up to 127 (0x7F) specify the minimum number of milliseconds to delay 
	between frames;
	while values in the range 241(0xF1) to 249(0xF9) specify delays increasing 
	from 100 to 900 microseconds. 
	Note that: !!!!!!!!
	the Separation Time is defined as the minimum time between the end of one frame 
	to the beginning of the next. 
	Robust implementations should be prepared to accept frames from a sender that 
	misinterprets(���) this as the frame repetition rate i.e. from start-of-frame 
	to  start-of-frame. Even careful implementations may fail to account for the 
	minor effect of bit-stuffing in the physical layer.
	
	--------------------- CF frame discription --------------------------------------
	The sender transmits the rest of the message using Consecutive Frames. Each 
	Consecutive Frame has a one byte PCI, with a four bit type (type = 2) followed 
	by a 4-bit sequence number. 
	---SN----
	The sequence number starts at 1(why??) and increments with each frame sent (1, 
	2,..., 15, 0, 1,...), with which lost or discarded frames can be detected. Each 
	consecutive frame starts at 0, initially for the first set of data in the first 
	frame will be considered as 0th data. So the first set of CF(Consecutive frames) 
	start from "1". There afterwards when it reaches "15", will be started from "0". 
	The 12 bit length field (in the FF) allows up to 4095 bytes of user data in a 
	segmented message, but in practice the typical application-specific limit is 
	considerably lower because of receive buffer or hardware limitations.*/	
	uint8_t  rx_responser_FC_FS;
	uint8_t  rx_responser_FC_BS;
	uint32_t rx_responser_FC_STmin; //unit: us
	

	enum pdu_rx_sta  current_rx_status;


	uint32_t rx_time_tree;

	uint8_t  error_flag; //user define,just for debug
	N_Result_T rx_result;//ISO15765 define

}N_PDU_RX_USER_DEF_T;
#pragma pack()

#pragma pack(1)
typedef struct { 
	uint8_t  initiate_tx_flag:1;
	uint8_t  NWL_now_sending_flag:1;
	uint8_t  error_flag;
	uint8_t  loc_FC_FS;
	uint8_t  loc_FC_BS_value;
	uint32_t loc_rsp_FC_STmin;
	uint32_t tx_time_tree;
	uint8_t  tx_SN;
	uint8_t  tx_FC_BS_counter;
	N_Result_T tx_result;//ISO15765 define
	uint8_t  tx_step;	
}N_PDU_TX_USER_DEF_T;
#pragma pack()


#pragma pack(1)
typedef struct {
	uint8_t 	FS		:4; //CTS:��������,WAIT:�ȴ���OVFLW:���
	uint8_t 	PCI_Type:4; //�̶�Ϊ:0011b,������λΪ0011,��ʾ����֡������λ������ʾ����״̬
	uint8_t 	BS;
	uint8_t 	STmin;
	uint8_t     reserve1;
	uint8_t     reserve2;
	uint8_t     reserve3;
	uint8_t     reserve4;
	uint8_t     reserve5;
}N_PDU_FC_T;//����֡8���ֽ����ݵĽṹ����
#pragma pack()

int APP_send_ISO15765_data(uint8_t *data_ptr,uint16_t data_len);
void ISO15765Task(void *p_arg);
#endif

