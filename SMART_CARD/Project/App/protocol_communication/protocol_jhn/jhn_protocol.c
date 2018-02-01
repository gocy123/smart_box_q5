#include "jhn_protocol.h"
#include <stdint.h>
typedef struct {
	char info_len_str[6];
	uint8_t *info_data;
}JHN_PROTOCOL_T;

/*char szLen[6]={0};
int nLen = 30 ;
sprintf(szLen,"%06X",nLen);*/

//"000020"+"2134312545656786654rbfwgejutyityutyrterwwwert"


//0000202134312545656786654rbfwgejutyityutyrterwwwert



#if 0

#include <stdio.h>

void main(void)
{
	char input_char;
	char char_A,char_B,char_C,char_D;

	char_A='A';
	char_B='B';
	char_C='C';
	char_D='D';

	scanf("%c",&input_char);

	if(input_char==char_A)
	{
		printf("input A\r\n");
	}
	else if(input_char==char_B)
	{
		printf("input B\r\n");
	}	
	else if(input_char==char_C)
	{
		printf("input C\r\n");
	}
	else if(input_char==char_D)
	{
		printf("input D\r\n");
	}
}



#include <stdio.h>

void main(void)
{
	unsigned int salary;
	unsigned int tax;
	printf("please input your salary:");
	scanf("%d",&salary);
	if(salary<=1500)
	{
		tax=0;
	}
	else if(salary<=4500)
	{
		tax=105;
	}
	else if(salary<=9000)
	{
		tax=555;
	}
	else if(salary<=35000)
	{
		tax=1005;
	}
	else if(salary<=55000)
	{
		tax=2755;
	}
	else if(salary<=80000)
	{
		tax=5505;
	}
	else
	{
		tax=13505;
	}
	
	printf("tax=%d\r\n",tax);
}

#endif














