
#ifndef ENCODING_V8_H_
#define ENCODING_V8_H_

typedef	unsigned char		uchar;
typedef	unsigned int		uint;
typedef	unsigned long		ulong;
typedef union{
  uchar byte[4];
  ulong words;
}un_DWORD;

void IV8_Decrypt(unsigned char *iData);

#endif //ENCODING_H_
