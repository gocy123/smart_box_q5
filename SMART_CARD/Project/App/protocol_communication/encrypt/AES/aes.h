#ifndef _AES_H
#define _AES_H

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned int
#endif
#define	AES_FIX_CODE		0x485AB426
typedef struct
{
    uint32 erk[64];     /* encryption round keys */
    uint32 drk[64];     /* decryption round keys */
    int nr;             /* number of rounds */
}aes_context;

extern void AesDecryptData(uint8* databuffer,uint32 datalen);
extern void AesEncryptData(uint8* databuffer,uint32 datalen);
extern void SetAesKey(unsigned char CK[16]);
extern void rand_extend(unsigned char* p_rand);
extern void get_secret_key(unsigned char* p_pin);
extern void get_authentication_rand(unsigned char* p_rand);
extern void aes_encrypt_data(unsigned char* p_encrypt, unsigned char* p_rand);
extern void get_authentication_rand(unsigned char* p_rand);

#endif /* aes.h */
