//porting head files
#include "user_debug_app.h"
#include "heads.h"

//static head file
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#if 1
#define PRINT_USE_MUTEX 1
#if PRINT_USE_MUTEX
OS_MUTEX print_mutex;
#endif


#define DEBUG_PORT_UART1 1
#define DEBUG_PORT_UART2 0
#define DEBUG_PORT_UART3 0

void debug_port_init(void)
{
	OS_ERR os_err;

	#if DEBUG_PORT_UART1
	user_uart1_open();
	#elif DEBUG_PORT_UART2
	user_uart2_open();
	#else
	user_uart3_open();
	#endif

	OSMutexCreate(&print_mutex,"print mutex",&os_err);
}


void debug_port_deinit(void)
{
	#if DEBUG_PORT_UART1
	user_uart1_close();
	#elif DEBUG_PORT_UART2
	user_uart2_close();
	#else
	user_uart3_close();
	#endif
}



#define CMD_BUFFER_LEN 500
static char buffer[CMD_BUFFER_LEN]; 
void InfoPrintf (char *fmt, ...)
{
	#if 1
	unsigned int len;
	va_list arg_ptr;
	
	//CPU_SR_ALLOC();
	//CPU_CRITICAL_ENTER();
	
	#if PRINT_USE_MUTEX
    OS_ERR os_err;
	//if(print_mutex.Type != OS_OBJ_TYPE_MUTEX)
	//{
	//	OSMutexCreate(&print_mutex,"print mutex",&os_err);
	//}
	OSMutexPend(&print_mutex,0,OS_OPT_PEND_BLOCKING,NULL,&os_err);
	#endif

	//return;

	va_start(arg_ptr, fmt);  
	len = vsnprintf(buffer, CMD_BUFFER_LEN, fmt, arg_ptr);
	
	if(buffer[len-1] == '\n' && buffer[len-2] != '\r')
	{
		if(len == CMD_BUFFER_LEN)
		len--;
		buffer[len-1] = '\r';
		buffer[len] = '\n';
		len++;
	}

	#if DEBUG_PORT_UART1
	uart1_send_data((unsigned char *)buffer,len);
	#elif DEBUG_PORT_UART2
	uart2_send_data((unsigned char *)buffer,len);
	#else
	uart3_send_data((unsigned char *)buffer,len);
	#endif

	va_end(arg_ptr);
	
	#if PRINT_USE_MUTEX
	OSMutexPost(&print_mutex,OS_OPT_POST_NONE,&os_err);
	#endif
	
	//CPU_CRITICAL_EXIT();

	#endif
}

#define PRTF_BUFFER_LEN 120
static char buffer1[PRTF_BUFFER_LEN+1]; 
void BlockPrintf(char *fmt, ...)
{
	
	//uint8_t *ptr=NULL;
	//uint32_t timeOut;
	unsigned int len;
	//int ret;

	va_list arg_ptr;

	va_start(arg_ptr, fmt);  
	/*
	int _vsnprintf(char *buffer, size_t max_count, const char *format, va_list vArgList);
��������˵��: 
����1. char *buffer 		[out], �����ɵĸ�ʽ�����ַ������������. 
����2. size_t max_count 	[in] , buffer�ɽ��ܵ�����ֽ���,��ֹ��������Խ��. 
����3. const char *format 	[in] , ��ʽ���ַ��� 
����4. va_list vArgList 	[in] , va_list����. va:variable-argument:�ɱ���� 

	����ֵ:
	����ɹ����ô˺���������д��buffer�е��ַ��ĸ�������������β��'\0'����
	snprintf��vsnprintf�������ܹ�д����size��С��������β��'0'�����ֽ�����
	��������Ϊ����ԭ�򱻽ضϣ�
	���سɹ�д��buffer���ַ���,����������β��'\0',(������㹻���ڴ�ռ�Ļ�)��
	���ԣ��������ֵ����size���ߴ���size����ʾ�����buffer���ַ����ضϣ�
	���������������������򷵻�һ��������
	*/
	len = vsnprintf(buffer1, PRTF_BUFFER_LEN, fmt, arg_ptr);
	
	if(buffer1[len-1] == '\n' && buffer1[len-2] != '\r')
	{
		buffer1[len-1] = '\r';
		buffer1[len] = '\n';
		len++;
	}
	//ptr=(uint8_t *)buffer1;
	#if DEBUG_PORT_UART1
	uart1_send_data((unsigned char *)buffer1,len);
	#elif DEBUG_PORT_UART2
	uart2_send_data((unsigned char *)buffer1,len);
	#else
	uart3_send_data((unsigned char *)buffer1,len);
	#endif
	va_end(arg_ptr);
}

#endif


