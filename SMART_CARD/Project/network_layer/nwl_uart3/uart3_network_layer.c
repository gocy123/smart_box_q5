// porting head file
#include "stm32f10x.h"
#include "platform_config.h"

//static head file  
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "ringbuffer.h"
#include "uart3_network_layer.h"


#define ___________UART3__PORT_AREA____START______

static int uart3_init_link(void);
inline int uart3_wr_rx_link_buffer(uint8_t *buf, int len);
static int uart3_trig_a_tx_interrupt(void);

#define UART3_SEND_DATA_RETRY_TIMES  (500)

enum uart3_send_state{
	UART3_SEND_IDLE=0,
	UART3_SEND_SENDING
};
//rx ringbuffer
#define UART3_RX_RINGBUF_LEN    (256*sizeof(uart3_data_package_t))
static struct rt_ringbuffer uart3_rx_ringbuffer_controller={0};
static uint8_t uart3_rx_ringbuf[UART3_RX_RINGBUF_LEN]={0};

//tx ringbuffer
#define UART3_TX_RINGBUF_LEN    (128*sizeof(uart3_data_package_t))
static struct rt_ringbuffer uart3_tx_ringbuffer_controller={0};
static uint8_t uart3_tx_ringbuf[UART3_TX_RINGBUF_LEN]={0};

//sending status flag
static enum uart3_send_state uart3_send_sta=UART3_SEND_IDLE;


static void uart3_retry_delay(void)
{
	#if 0
	OS_ERR os_err;
	OSTimeDly(ms,OS_OPT_TIME_DLY,&os_err);
	#else
	uint32_t i;
	i=0xFF;
	while(i--)
	{
		
	}
	#endif
}


void user_uart3_open(void)
{
	#define PRINT_BARDRATE_UART3 (115200)
	//#define PRINT_BARDRATE_UART1 (115200)
	USART_InitTypeDef USART_InitStructure={0};
	//NVIC_InitTypeDef NVIC_InitStructure={0};


	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable clock */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* Configure USART2 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART2 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	

	USART_InitStructure.USART_BaudRate = PRINT_BARDRATE_UART3;//*2;

	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART2 */
	USART_Init(USART3, &USART_InitStructure);

	/* Enable USARTy Receive and Transmit interrupts */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	/* Enable the USARTy */
	USART_Cmd(USART3, ENABLE);
	
	//config link layer
	uart3_init_link();
}

void user_uart3_close(void)
{

}

void user_uart3_wakeup(void)
{

}

void user_uart3_sleep(void)
{

}

void user_uart3_reset(void)
{
	user_uart3_close();
	user_uart3_open();
}

static void user_uart3_en_tx_int(void)
{
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

static void user_uart3_dis_tx_int(void)
{
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
}



/*
	brief:  write one minimum data package to peripheral TX regs

	input: 
			databyte: one msg will be sent
	return:
			0: send fail
			1: send ok
*/
static int user_uart3_transmit_one_data_package(uart3_data_package_t package)
{
	//int ret_len;
	uint8_t ret;
	do
	{
		ret=USART_GetFlagStatus(USART3,USART_FLAG_TXE);
	}while(ret==0);
	USART_SendData(USART3, (uint8_t) package);

	return 1;
}

/*
	brief: read one minimum data package from the peripheral RX regs

	input: 
			databyte: pointer that point to the rx databyte
	return:
			0: receive one byte fail
			1: receive one byte ok
*/
static int user_uart3_receive_one_data_package(uart3_data_package_t *package)
{
	*package = (uart3_data_package_t)(USART3->DR);
	return 1;
}

void USART3_IRQHandler(void)
{
	//uint32_t intsrc, tmp, tmp1;
	uart3_data_package_t tempchar;

	// Receive Data Available or Character time-out
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		if(user_uart3_receive_one_data_package(&tempchar)==1)
		{
			//InfoPrintf("%c",tempchar);
		
			if(uart3_wr_rx_link_buffer((uint8_t *)&tempchar,sizeof(tempchar))!=1)
			{
				//InfoPrintf("UART3 RX link buffer full\r\n");
			}
		}
	}

	// Transmit Holding Empty
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{
			uart3_trig_a_tx_interrupt();
	}

}


#define __________UART3__PORT_AREA____END________





/*
	return:
			0: idle
			1: busy,can`t write data to tx buf
*/
int uart3_get_busy_state(void)
{
	return uart3_send_sta;
}


static int uart3_init_link(void)
{
	rt_ringbuffer_init(&uart3_rx_ringbuffer_controller,
						uart3_rx_ringbuf,
						UART3_RX_RINGBUF_LEN);
	
	rt_ringbuffer_init(&uart3_tx_ringbuffer_controller,
							uart3_tx_ringbuf,
							UART3_TX_RINGBUF_LEN);

	uart3_send_sta=UART3_SEND_IDLE;

	return 0;
}

/*
	output: 
			package: one package that readout from tx ringbuffer
	return:
			0: no data need to be sent 
			1: read one msg successfully
		   -1: error data in ringbuffer,app caller shuld reset the tx ringbuffer
*/
static int uart3_rd_one_package_from_tx_ringbuffer(uart3_data_package_t *package)
{
	uint16_t package_size=sizeof(uart3_data_package_t);
	uint32_t ret_len;

	if(rt_ringbuffer_data_len(&uart3_tx_ringbuffer_controller)<package_size)
	{
		return 0;
	}

	ret_len=rt_ringbuffer_get(	&uart3_tx_ringbuffer_controller,
                      			(uint8_t *)package,
                       			 package_size );
	if(ret_len==package_size)
	{
		return 1;
	}
	else if(ret_len==0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}


/*
	use to start one transmition by app caller,
 	or 
 	use to enable continue transmiting  in IRQ function

 	return:
			1: tx one package successfully, will occur one tx INT soon,
			   peripheral is in tx sending state.
			0: tx ringbuffer is empty,trig fail,
			   peripheral is in tx idle state.
		   -1: read tx package from tx ringbuffer ok,but send fail.
		   -2: error data format in ringbuffer,need to reset the ringbuffer.
*/
static int uart3_trig_a_tx_interrupt(void)
{
	int ret;
	uart3_data_package_t tx_data_package;
	
	ret = uart3_rd_one_package_from_tx_ringbuffer(&tx_data_package);
	if(ret==1)
	{
		if(user_uart3_transmit_one_data_package(tx_data_package)==1)
		{
			uart3_send_sta=UART3_SEND_SENDING;
			user_uart3_en_tx_int();
			return 1;
		}
	}
	else if(ret==0)
	{
		//no data in ringbuffer
		user_uart3_dis_tx_int();
		uart3_send_sta=UART3_SEND_IDLE;
		return 0;
	}
	else if(ret==-1)
	{
		//error data in tx ringbuffer ,neet to be reset
		user_uart3_dis_tx_int();
		uart3_send_sta=UART3_SEND_IDLE;
		return -2;
	}

	user_uart3_dis_tx_int();
	uart3_send_sta=UART3_SEND_IDLE;
	return 0;
}


/*
	write (1 ~~ UART3_TX_RINGBUF_LEN) bytes to uart ringbuf

	input:
			buf: input data
			len: bytes amount need to write
	return:
			the real amount that written to ringbuf,
			it must be the 0 or len !!!!!!

	note : this function is called by uart3_send_data() only, other app 
	       function should not call this function
*/
static int uart3_wr_tx_link_buf_max_len(uint8_t *buf, uint32_t len)
{	
	int empty_len;
	int ret_len=0;
	//int ret;

	if(len>UART3_TX_RINGBUF_LEN)
	{
		return 0;
	}
	else
	{
		empty_len=rt_ringbuffer_space_len(&uart3_tx_ringbuffer_controller);
		if(empty_len>=len)
		{
			//因为会改变ringbuffer控制器值,所以需要禁止发送中断,阻止在ISR中同时修改ringbuffer控制器
			//user_uart2_dis_tx_int();
			ret_len = rt_ringbuffer_put(&uart3_tx_ringbuffer_controller,buf,len);
			//user_uart2_en_tx_int();
			if(ret_len!=len)
			{
				ret_len=0;//error!!!!!
			}
			
			if(uart3_send_sta==UART3_SEND_IDLE)
			{				
				uart3_trig_a_tx_interrupt();
			}
		}
		else 
		{
			return 0;
		}
	}
	return ret_len;
}

/*
	brief: write one byte to the rx linkbuf

	input: 
			databyte: UART3 rx reg data
	return:
			0:failed
			1:success
*/
int uart3_wr_rx_link_buffer(uint8_t *buf, int len)
{	  
	return rt_ringbuffer_put(&uart3_rx_ringbuffer_controller,buf,len);
}





//-----------------------APP CALL----------------------------------------

/*
	brief: wr len bytes data to the tx linkbuf

	input: 
			buf: contain the data that want to send by can
			len: bytes amount that want to write into linkbuf
	return:
			the real amount written to the tx link buff
note:
		APP call this function to send UART0 datas
*/
int uart3_send_data(uint8_t *buf, uint32_t len)
{	
	int ret=0;
	int ret_len=0;
	int i=0,j=0,k=0;
	uint16_t retry_times;
	uint16_t retry_times_2;
	
	uint8_t *ptr;
	
	ptr=buf;

	if(len>UART3_TX_RINGBUF_LEN)
	{
		i=(len/UART3_TX_RINGBUF_LEN); //
		j=(len%UART3_TX_RINGBUF_LEN); //
		
		retry_times=UART3_SEND_DATA_RETRY_TIMES;
		for(k=0;k<i;)
		{
			if(retry_times<=0)
			{
				break;
			}
			retry_times--;
			
			retry_times_2=UART3_SEND_DATA_RETRY_TIMES;
			while(uart3_get_busy_state()==UART3_SEND_SENDING)
			{
				uart3_retry_delay();
				if(retry_times_2<=0)
				{
					break;
				}
				retry_times_2--;
			}
			
			if(retry_times_2<=0)
			{
				break;
			}
			
			ret=uart3_wr_tx_link_buf_max_len(ptr,UART3_TX_RINGBUF_LEN);
			ret_len+=ret;
			if(ret == UART3_TX_RINGBUF_LEN)
			{
				retry_times=UART3_SEND_DATA_RETRY_TIMES;
				ptr+=UART3_TX_RINGBUF_LEN;
				k++;
			}
			else
			{
				break;
			}
		}

		if(k==i)
		{
			if(j!=0)
			{
				retry_times=UART3_SEND_DATA_RETRY_TIMES;
				while(1)
				{
					ret=uart3_wr_tx_link_buf_max_len(ptr,j);
					if(ret==0)
					{
						uart3_retry_delay();
					}
					else
					{
						ret_len+=ret;
						break;
					}
					
					if(retry_times<=0)
					{
						break;
					}
					retry_times--;
				}
			}
		}
	}
	else
	{
		retry_times=UART3_SEND_DATA_RETRY_TIMES;
		while(1)
		{
			ret=uart3_wr_tx_link_buf_max_len(buf,len);
			if(ret==0)
            {
              uart3_retry_delay();
            }
            else
            {
              ret_len+=ret;
              break;
            }
			if(retry_times<=0)
			{
				break;
			}
			retry_times--;
		}
	}
	
	return ret_len;
}


/*
	brief: read data from rx link buffer 
	input:
			len: bytes amount that want to read out
	output:
			buf: contain the read out datas
	return:
			the really read out bytes amount
note:
		APP call this function to read UART1 rx datas

*/
int uart3_receive_data(uint8_t *buf,int read_len)
{  
	uint16_t package_size=sizeof(uart3_data_package_t);
	uint32_t ringbuf_cache_len_in_bytes;

	if(read_len>UART3_RX_RINGBUF_LEN)
	{
		return -1;
	}

	if(read_len%package_size!=0)
	{
		return -1;//illegal read length
	}

	ringbuf_cache_len_in_bytes=rt_ringbuffer_data_len(&uart3_rx_ringbuffer_controller);
	
	if(ringbuf_cache_len_in_bytes < package_size)
	{//缓存的数据 < 最小数据单元
		return 0;
	}

	if(read_len<=ringbuf_cache_len_in_bytes)
	{//需读取 < 缓存
		return rt_ringbuffer_get(  &uart3_rx_ringbuffer_controller,
									buf,
								   (uint16_t)read_len
								);
	}
	else
	{//需读取 > 缓存  --->先读出已缓存的数据(按package_size为最小单位读出)
		return rt_ringbuffer_get(  &uart3_rx_ringbuffer_controller,
								    buf,
								 (((uint16_t)ringbuf_cache_len_in_bytes/sizeof(uart3_data_package_t))*sizeof(uart3_data_package_t)));
	}
}
























