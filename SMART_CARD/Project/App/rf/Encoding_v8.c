#include "Encoding_v8.h"

#define small_data 0x9e3779b9	//iv8 ok
#define large_data 0xc6ef3720

uchar const Data_58F0[] = {0x0B,0x46,0x50,0x2D,0x5E,0x25,0x37,0x18,0x2B,0xF9,0x3A,0x19,0x62,0x2C,0x12,0x06};

ulong Char2Long(uchar const *pt) {
	un_DWORD tmp;
	tmp.byte[0] = pt[3];
	tmp.byte[1] = pt[2];
	tmp.byte[2] = pt[1];
	tmp.byte[3] = pt[0];
	return tmp.words;
	
}
void Long2Char(ulong t1, uchar *sp) {
	uchar *p = (uchar *)(&t1);
	sp[3] = *p;
	sp[2] = *(p + 1);
	sp[1] = *(p + 2);
	sp[0] = *(p + 3);
}
void DBCM_46E9(ulong t1, uchar *pt) {
	ulong t2 = Char2Long(pt);
	t2 -= t1;
	Long2Char(t2, pt);
}
void IV8_Decrypt(uchar* iData) 
{
	uchar sp[20];
	uchar i;
	ulong t1 = large_data;

	Long2Char(t1, sp + 8);     //Ìîsp[8~~11]

	for(i = 0; i < 8; i++) 
	{
		sp[i + 12] = iData[i]; //Ìîsp[12~~19]
	}
	
loop:
	t1 = (sp[9] << 8) + sp[10];
	t1 = (t1 >> 3);
	t1 = (t1 & 0x3);
	t1 = (t1 << 2);
	t1 = Char2Long(Data_58F0 + t1) + Char2Long(sp + 8);
	Long2Char(t1, sp + 4);
	
	t1 = Char2Long(sp + 12);
	t1 = (t1 + ((t1 >> 5) ^ (t1 << 4))) ^ Char2Long(sp + 4);
	DBCM_46E9(t1, sp + 16);
	
	t1 = Char2Long(sp + 8);
	t1 = (t1 - small_data);
	Long2Char(t1, sp + 8);
	
	t1 = ((sp[11] & 0x3) << 2);
	t1 = Char2Long(Data_58F0 + t1) + Char2Long(sp + 8);
	Long2Char(t1, sp + 4);

	t1 = Char2Long(sp + 16);
	t1 = (t1 + ((t1 >> 5) ^ (t1 <<4 ))) ^ Char2Long(sp + 4);
	DBCM_46E9(t1, sp + 12);

	t1 = Char2Long(sp + 8);
	if(t1 != 0) goto loop;
	
	for(i = 0; i < 8; i++) iData[i] = sp[i + 12];
}