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
	ISO 15765总共分为4个部分:
	15765-1:物理层及数据链路层说明
	15765-2:网络层说明
	15765-3:应用层说明
	15765-4:排放相关说明




	所有的网络层服务项(3大类),都具有统一的结构。
	(1)请求服务项类:
		由 发送端上层   使用,用于向网络层传递控制信息 和 诊断服务数据。
		
	(2)确认服务项类:
		由 发送端网络层 使用,用于向上层确认:上层所请求的服务的执行结果(成功或失败)
		
	(3)指示服务项类:
		由 接收端网络层 使用,用于向上层指示:数据接收的执行结果(成功或失败)

这些服务项没有指定具体的应用程序接口

例: 

应用层的某服务项定义如下:

service_name.type{

	para A
	para B
	para C
	...
	...
}

其中:

	service_name:表示具体的服务项名称,由应用层命名
	type		:表示服务项的类型(请求request、确认confirm、指示indication)
	para		:需要交互的参数
	
*/

//----------------------------------------------------------------------------------
//  关键术语解释    
//		N 	: 在此代码里面表示 Net ,即网络
//  N_PDU 	: 网络协议数据单元， Net Protocol Data Unit 基本单元		
//	   BS   : 块大小
//	ST_min  : 间隔最短时长
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
//       服务项数据单元说明 start 
//----------------------------------------------------------------------------------
typedef unsigned char 	N_AI_T;//位数待定
typedef unsigned char 	N_SA_T;//网络源地址(0x00~~0xFF)  ,代表发送者网络层实体
typedef unsigned char 	N_TA_T;//网络目标地址(0x00~~0xFF),代表接收者网络层实体
typedef enum {
	PHYSICAL_ADD=0,//1 对 1  通信
	FUNCTIONAL_ADD //1 对 多 通信,但仅支持单帧通信
	
} N_TAtype_T;//网络目标低值类型,代表通信模式。

typedef unsigned char 	N_AE_T;		//网络低值扩展。
typedef unsigned short 	N_MSG_LEN_T;//要发送或者接收的数据的长度(1~~4095)。
typedef unsigned char * N_MsgData_T;//该参数包含与上层实体所有交互的数据。
//typedef enum {
//	STmin=0,
//	BS 
//} N_Parameter_T;//该参数确定网络层的参数。

typedef unsigned char 	N_ParaValue_T;	//该参数分配给协议参数<Parameter>作为指示服务。

typedef struct {
	
	//发送方 发送CAN帧 的耗时,自身耗时
	//timeout: 1000ms,defined by ISO15765
	//any pdu
	uint16_t 	N_As; 

	//接收方 发送CAN帧 的耗时,自身耗时
	//timeout: 1000ms,defined by ISO15765
	//any pdu
	uint16_t	N_Ar; 

	//发送方 从获取FF,FC的confirm,或FC的indicate--到-- 接收到下一个流控帧 的时间
	//timeout: 1000ms,defined by ISO15765
	uint16_t 	N_Bs;

	//接收方 从获取FF的indicate,  或FC的confirm---到-- 发送下一个流控帧   的时间
	//timeout(undefine)
	uint16_t 	N_Br;

	//发送方 从获取FC的confirm，或CF的indicate----到-- 发送下一个连续帧   的时间
	//timeout:undefine
	uint16_t 	N_Cs;

	//接收方 从获取FC的confirm，或CF的indicate----到-- 接收到下一个连续帧 的时间
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
			 /*	如果同时产生了两个或以上的错误，网络层应
				该先上报 枚举值较低的错误 给上层 */
} N_Result_T;//	该参数包含服务项执行的结果状态。

typedef enum
{
	N_CHAN_PARA_OK=0,
	N_CHAN_PARA_RX_ON,
	N_CHAN_PARA_WRONG_PARAMETER,
	N_CHAN_PARA_WRONG_VALUE
} N_Result_ChangeParameter_T;//	该参数包含服务项执行的结果状态信息

//----------------------------------------------------------------------------------
//       服务项数据单元说明 end 
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
	// ConTinue Send 授权继续发送	
	unsigned char CTS:1;	
	
	// 请求继续等待,其有个最大限值为:N_WFTmax
	unsigned char WAIT:1;	
	
	// 缓冲溢出,多用于指示拆分数据的首帧中表示的字节个数信息
	// 超出了接收者可存储的信息总量
	unsigned char OVFLW:1;	
	 
} Flow_Control_T;//流控的相关控制、状态标志符


enum FS_sta {
	// ConTinue Send 授权继续发送	
	 FS_CTS=0,	
	
	// 请求继续等待,其有个最大限值为:N_WFTmax
	 FS_WAIT,	
	
	// 缓冲溢出,多用于指示拆分数据的首帧中表示的字节个数信息
	// 超出了接收者可存储的信息总量
	 FS_OVFLW
	 
} ;//流控的相关控制、状态标志符






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
	网络层协议具有如下功能:
	(1)发送、接收最多4095个字节的数据信息
	(2)报告发送、接收完成的状态

	发送方式: 单帧发送，多帧发送

	多帧发送时，信息的长度在首帧(FF_N_PDU)中被发送，后续会使用到连续帧、流控帧
	关于流控帧: 接收方通过流控机制将接受能力通知发送方
	            发送方发送流控帧通知接收方，接收方才能调整字节的接收，以适应发
	            送放的发送。
	            包括:块大小(BS)-->在授权继续发送其余的N_PDU之前，接收方允许发送方
	                 最大的N_PDU个数。
	                 间隔最短时间(STmin)-->发送方在发送两个连续帧间隔等待的最短时间。
	                 

	
*/


/*网络协议数据单元(N_PDU)有4中数据类型:
(1)SF_N_PDU   :single frame 
(2)FF_N_PDU   :first frame
(3)CF_N_PDU   :continue frame
(4)FC_N_PDU   :flow control frame


	N_PDU format
	包含了3个域: [地址信息(N_AI)] [协议控制信息(N_PCI)] [数据域(N_Data)]

	N_AI	: CAN_ID ??

	N_PCI	: 用于区分该帧是 SF_N_PDU,FF_N_PDU,CF_N_PDU,FC_N_PDU

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
	//单帧的信息体长度,首帧的信息体长度低4位,连续帧的serial number,流控帧的状态符
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
	misinterprets(误解) this as the frame repetition rate i.e. from start-of-frame 
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
	uint8_t 	FS		:4; //CTS:继续发送,WAIT:等待，OVFLW:溢出
	uint8_t 	PCI_Type:4; //固定为:0011b,即高四位为0011,表示流控帧，低四位用来表示流控状态
	uint8_t 	BS;
	uint8_t 	STmin;
	uint8_t     reserve1;
	uint8_t     reserve2;
	uint8_t     reserve3;
	uint8_t     reserve4;
	uint8_t     reserve5;
}N_PDU_FC_T;//流控帧8个字节数据的结构定义
#pragma pack()

int APP_send_ISO15765_data(uint8_t *data_ptr,uint16_t data_len);
void ISO15765Task(void *p_arg);
#endif

