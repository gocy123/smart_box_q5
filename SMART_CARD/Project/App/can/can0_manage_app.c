#include "user_system_cfg.h"
// porting head file
#include "can0_manage_app.h"
#include "os.h"
#include "os_cfg_app.h"

//static head file
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "ringbuffer.h"
#include "can0_network_layer.h"
#include "ISO15765.h"
#include "user_debug_app.h"

#include <stdlib.h>
uint8_t  user_tx_data_ptr[4095]={0};
uint16_t user_tx_data_len=4095;



void PrintCan0Msg(can0_data_package_t* CAN_Msg)
{
	uint8_t i;

	InfoPrintf("APP__CAN MSG: ID=0x%04X,",CAN_Msg->id);
	InfoPrintf("ID fmt:");
	if(CAN_Msg->format==STD_ID_FORMAT)
	{
		InfoPrintf("STD,");
	}
	else
	{
		InfoPrintf("EXT,");
	}
	InfoPrintf("len=%d,",CAN_Msg->len);

	InfoPrintf("type=");
	if(CAN_Msg->type==DATA_FRAME)
	{
		InfoPrintf("DATA");
	}
	else
	{
		InfoPrintf("REMOTE");
	}

	InfoPrintf("\r\ndata: ");

	for(i=0;i<8;i++)
	{
		if(i<4)
		InfoPrintf("%02X,",CAN_Msg->dataA[i]);
		else
		InfoPrintf("%02X,",CAN_Msg->dataB[i-4]);
		
	}
	InfoPrintf("\r\n");
}



void can0_port_init(void)
{
  user_can0_open();
}

void can0_port_deinit(void)
{
  user_can0_close();
}


void Can0ManageTask(void *p_arg)
{
	OS_ERR os_err;
	int i=0;
	can0_data_package_t can_rx_package;
	static uint32_t timestamp=0;
	//can0_data_package_t can_tx_msg[5];
	unsigned char *ptr=NULL;
	unsigned char *print_ptr=NULL;
	//uint16_t sent_len=0;
	//uint8_t flag=0;
	unsigned char temp_last=0;
	unsigned char temp_curr=0;

	uint8_t *ISO15765_rx_data_ptr=NULL;
	uint16_t ISO15765_rx_data_len;

	//uint8_t test_cnt=0;
    p_arg=p_arg;
#if 0
	can_tx_msg[0].id = 0x01;
	can_tx_msg[0].len = 0x08;
	can_tx_msg[0].type = DATA_FRAME;
	can_tx_msg[0].format = STD_ID_FORMAT;
	can_tx_msg[0].dataA[0] = can_tx_msg[0].dataA[1] = can_tx_msg[0].dataA[2]= can_tx_msg[0].dataA[3]= 0x55;
	can_tx_msg[0].dataB[0] = can_tx_msg[0].dataB[1] = can_tx_msg[0].dataB[2]= can_tx_msg[0].dataB[3]= 0xaa;

	can_tx_msg[1].id = 0x02;
	can_tx_msg[1].len = 0x08;
	can_tx_msg[1].type = DATA_FRAME;
	can_tx_msg[1].format = STD_ID_FORMAT;
	can_tx_msg[1].dataA[0] = can_tx_msg[1].dataA[1] = can_tx_msg[1].dataA[2]= can_tx_msg[1].dataA[3]= 0x55;
	can_tx_msg[1].dataB[0] = can_tx_msg[1].dataB[1] = can_tx_msg[1].dataB[2]= can_tx_msg[1].dataB[3]= 0xaa;

	can_tx_msg[2].id = 0x03;
	can_tx_msg[2].len = 0x08;
	can_tx_msg[2].type = DATA_FRAME;
	can_tx_msg[2].format = STD_ID_FORMAT;
	can_tx_msg[2].dataA[0] = can_tx_msg[2].dataA[1] = can_tx_msg[2].dataA[2]= can_tx_msg[2].dataA[3]= 0x55;
	can_tx_msg[2].dataB[0] = can_tx_msg[2].dataB[1] = can_tx_msg[2].dataB[2]= can_tx_msg[2].dataB[3]= 0xaa;	
	

	can_tx_msg[3].id = 0x04;
	can_tx_msg[3].len = 0x08;
	can_tx_msg[3].type = DATA_FRAME;
	can_tx_msg[3].format = STD_ID_FORMAT;
	can_tx_msg[3].dataA[0] = can_tx_msg[3].dataA[1] = can_tx_msg[3].dataA[2]= can_tx_msg[3].dataA[3]= 0x55;
	can_tx_msg[3].dataB[0] = can_tx_msg[3].dataB[1] = can_tx_msg[3].dataB[2]= can_tx_msg[3].dataB[3]= 0xaa;

	can_tx_msg[4].id = 0x05;
	can_tx_msg[4].len = 0x08;
	can_tx_msg[4].type = DATA_FRAME;
	can_tx_msg[4].format = STD_ID_FORMAT;
	can_tx_msg[4].dataA[0] = can_tx_msg[4].dataA[1] = can_tx_msg[4].dataA[2]= can_tx_msg[4].dataA[3]= 0x55;
	can_tx_msg[4].dataB[0] = can_tx_msg[4].dataB[1] = can_tx_msg[4].dataB[2]= can_tx_msg[4].dataB[3]= 0xaa;
#endif 
    
	can0_port_init();
	
	OSQCreate(&ISO15765_RX_CAN_MSG_Q,"ISO15765_RX_CAN_MSG Q",ISO15765_RX_CAN_MSG_Q_NUM,&os_err);
	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf("create ISO15765_RX_CAN_MSG_Q fail\r\n");
	}
	
	OSQCreate(&ISO15765_RX_APP_DATA_Q,"ISO15765_RX_APP_DATA Q",ISO15765_RX_APP_DATA_Q_NUM,&os_err);
	if(os_err != OS_ERR_NONE)
	{
		InfoPrintf("create ISO15765_RX_APP_DATA_Q fail\r\n");
	}

	//test
	#if 1
	for(i=0;i<user_tx_data_len;i++)
	{
		user_tx_data_ptr[i]=(uint8_t)i;
	}
	//user_tx_data_ptr[1000]=1;
	#endif
	//

	while(1)
	{
		if(OSTimeGet(&os_err)-timestamp>5000)
		{
			timestamp=OSTimeGet(&os_err);
			InfoPrintf("Can0ManageTask loop...\r\n");

			#if TST_TX
			APP_send_ISO15765_data(user_tx_data_ptr,user_tx_data_len);//64
			//APP_send_ISO15765_data(user_tx_data_ptr,test_cnt++);//64
			if(test_cnt>=129)test_cnt=0;
			#endif
		}


		ISO15765_rx_data_ptr=(uint8_t *)OSQPend(&ISO15765_RX_APP_DATA_Q,
												TIME_10ms,
												OS_OPT_PEND_NON_BLOCKING,
												&ISO15765_rx_data_len,
												NULL,
												&os_err
							 				   );
		if(ISO15765_rx_data_ptr!=NULL)
		{
			InfoPrintf("APP RX ISO15765 DATA(%d):",ISO15765_rx_data_len);
			for(i=0;i<ISO15765_rx_data_len;i++)
			{
				//InfoPrintf("%02X,",ISO15765_rx_data_ptr[i]);

				if(i!=0)
				{
					temp_last=ISO15765_rx_data_ptr[i-1]+1;
					temp_curr=ISO15765_rx_data_ptr[i];

					if(temp_curr!=temp_last)
					{
						InfoPrintf("接收数据错乱--i=%d--\r\n",i);
					}
				}
				
			}
			InfoPrintf("\r\n");

			user_free(ISO15765_rx_data_ptr,__FUNCTION__);
		}
		
		#if 1
		while(can0_receive_data((uint8_t *)&can_rx_package,sizeof(can0_data_package_t))==sizeof(can0_data_package_t))
		{
			//PrintCan0Msg(&can_rx_package);

			if(can_rx_package.id==ISO15765_RX_ID)
			{
				ptr=user_malloc(sizeof(can0_data_package_t),__FUNCTION__);
				if(ptr!=NULL)
				{
					memcpy(ptr,&can_rx_package,sizeof(can0_data_package_t));
				}
				
				OSQPost(&ISO15765_RX_CAN_MSG_Q,ptr,sizeof(can0_data_package_t),OS_OPT_POST_FIFO,&os_err);	
				if(os_err==OS_ERR_NONE)
				{
					//InfoPrintf("post #0# OK\r\n");
				}
				else
				{
					//处理步骤:
					//(1) 延时一下,再发一次,
					//(2) 发送FC_WAIT帧(这个帧应该要放在NWL层发送....)
					//(3) 增大消息队列

					user_delay_ms(TIME_10ms);
					OSQPost(&ISO15765_RX_CAN_MSG_Q,ptr,sizeof(can0_data_package_t),OS_OPT_POST_FIFO,&os_err);	
					if(os_err==OS_ERR_NONE)
					{
						//InfoPrintf("post #1# OK\r\n");
					}
					else
					{
						InfoPrintf("post ISO15765_RX_CAN_MSG_Q faile -%d----------------------\r\n",os_err);
						#if 1
						InfoPrintf("报文数据为:");
						print_ptr=(uint8_t *)(&can_rx_package.dataA[0]);
						for(i=0;i<8;i++)
						{
							InfoPrintf("%02X,",print_ptr[i]);
						}
						InfoPrintf("\r\n");
						#endif
						
						user_free(ptr,__FUNCTION__);
					}
				}
				
			}		
			
			//process can msg
			switch (can_rx_package.id)
			{
				case 0x01:

					break;
				default:
					break;
			}
		}
		#endif		

		user_delay_ms(TIME_10ms);
	}

}



