#ifndef _RSA_MAIN_H
#define _RSA_MAIN_H
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include "bignum.h"

extern unsigned char RSA_N_modulus_str[];
extern unsigned char RSA_E_exponent_str[];
extern uint16_t RSA_N_len_str;
extern uint8_t  RSA_E_len_str;
extern uint8_t  RSA_key_valid_flag;

void hex_to_string(uint8_t *hex_buf,uint16_t hex_data_len,uint8_t *str_buf,uint16_t *string_len);
int string_to_hex(uint8_t *str_buf,uint8_t *hex_buf,uint16_t *hex_data_len);







#endif
