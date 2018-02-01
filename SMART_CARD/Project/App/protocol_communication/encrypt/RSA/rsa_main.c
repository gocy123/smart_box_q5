#if 0
#include <string.h>
#include <stdio.h>

#include "bignum.h"


unsigned char source_data[254]="10000000000000000000000000000000\
00000000000000000000000000000000\
00000000000000000000000000000000\
00000000000000000000000000000000\
00000000000000000000000000000000\
00000000000000000000000000000000\
00000000000000000000000000000000\
000000000000000000000000000000";

unsigned char E_key[]="010001";

unsigned char D_key[]="797D936E7BF43084F72ACF697BA5DB44\
E31624F16508289D1B688FFF97D373F9\
98DDC8066F6FBC557822FC523AE13F4C\
4DE498A7CA5CC0341EE0C4E7A76206A6\
5A7B9613888EEC174DCEE6D43D5671DF\
379A4560F2582D7BBFFA7D5AE4604E86\
F15A240081C38C3D3E332CEE9CDAE88E\
A8C7097F04DF43A55B7D41D6BF2713D1";



unsigned char C_data[]="65AA4266BCC553EECE1C68CFD1649222\
897E282BB976B43BC64BFEFFECFD0C6\
2B121B38DB759542CB2E3A5E72C09387\
62E7342EAB39765BDCC8C5FDC877A3A2\
6952A3FF28CDBC960275877BFFAD85F5\
B47314D3DA74834763699F5C20C1E8A4\
9AA1086BBD8216E0C47E819D36360234\
623295B6EDBD3F957B5B4D95B0740C82C";

unsigned char N_modulus[256+64]="AAE16D3B63AC6CCEEEEE35F22379AB0A
FC3CBF938FE13D54005FB6C3407BC08A
39EEF132EB6161BE3FFC521E75E3B874
7A23C879E6C9AA0E9E1459EAE9A8D876
B68C9741D2E3A942B0D3034071BD313A
D4F77FAE460A7FFC32549597FE61A152
3E587E3C6ECB11324109C222CA8A96BF
752CB84CE5E5EDBA3EF702EF27029773";


int rsa_test(void)
{
	unsigned char *ret; 
	//ret=rsa_encrypt_data(source_data,E_key,N_modulus);
	ret=rsa_encrypt_data(source_data,E_key,N_modulus);

	if(ret!=NULL)
	{
		InfoPrintf("模指运算结果: %s\r\n", ret);
		
	}
	    return 0;
}

#else
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "bignum.h"

#if 0
unsigned char source_data[254]="10000000000000000000000000000000\
00000000000000000000000000000000\
00000000000000000000000000000000\
00000000000000000000000000000000\
00000000000000000000000000000000\
00000000000000000000000000000000\
00000000000000000000000000000000\
000000000000000000000000000000";
#endif
unsigned char RSA_E_exponent_str[64]="010001";

unsigned char RSA_N_modulus_str[256+64]="AAE16D3B63AC6CCEEEEE35F22379AB0A\
FC3CBF938FE13D54005FB6C3407BC08A\
39EEF132EB6161BE3FFC521E75E3B874\
7A23C879E6C9AA0E9E1459EAE9A8D876\
B68C9741D2E3A942B0D3034071BD313A\
D4F77FAE460A7FFC32549597FE61A152\
3E587E3C6ECB11324109C222CA8A96BF\
752CB84CE5E5EDBA3EF702EF27029773";

uint16_t RSA_N_len_str=0;
uint8_t  RSA_E_len_str=0;

uint8_t  RSA_key_valid_flag=0;


void hex_to_string(uint8_t *hex_buf,uint16_t hex_data_len,uint8_t *str_buf,uint16_t *string_len)
{
	uint8_t str_tab[16]="0123456789ABCDEF";
	int i,j;//,k;
	uint8_t temp;

	j=0;
	for(i=0;i<hex_data_len;i++)
	{
		temp=hex_buf[i];
		str_buf[j++]=str_tab[(temp>>4)];
		str_buf[j++]=str_tab[(temp&0x0F)];
	}

	*string_len=j;
}
/*
	return :
			 0: fail
			 1: successful
*/
int string_to_hex(uint8_t *str_buf,uint8_t *hex_buf,uint16_t *hex_data_len)
{
	uint8_t str_tab[16]="0123456789ABCDEF";
	//uint8_t hex_tab[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	int i,j;//,k;
	//uint8_t temp;

	uint16_t str_len=strlen((char const *)str_buf);

	if(str_len%2==1)return 0;

	//"abcdef"-->"ABCDEF"
	for(i=0;i<str_len;i++)
	{
		if(str_buf[i]>=0x61 && str_buf[i]<=0x7a)
		{
			str_buf[i]-=0x20;	
		}
	}
	
	//check error input data
	for(i=0;i<str_len;i++)
	{
		if((str_buf[i]<0x30) || (str_buf[i]>0X39&&str_buf[i]<0X41)||(str_buf[i]>0x5a))
		{
			return 0;
		}
	}


	//convert
	for(i=0;i<str_len;)
	{
		for(j=0;j<16;j++)
		{
			if(str_buf[i]==str_tab[j])
			{
				hex_buf[i/2]=(j<<4);	
			}
		}
		
		for(j=0;j<16;j++)
		{
			if(str_buf[i+1]==str_tab[j])
			{
				hex_buf[i/2]|=j;	
			}
		}
		
		i++;
		i++;
	}
	*hex_data_len=str_len/2;
	return 1;
}


int rsa_test(void)
{
	#if 0
	unsigned char *ret; 
	ret=rsa_encrypt_data(source_data,RSA_E_exponent_str,RSA_N_modulus_str);

	if(ret!=NULL)
	{
		InfoPrintf("模指运算结果: %s\r\n", ret);
		
	}
	#endif
  	return 0;
}

#endif

