#include "user_internal_flash.h"
#include "stm32f10x_flash.h"




/* Private define ------------------------------------------------------------*/
/* Define the STM32F10x FLASH Page Size depending on the used STM32 device */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

#define BANK1_WRITE_START_ADDR  ((uint32_t)0x08008000)
#define BANK1_WRITE_END_ADDR    ((uint32_t)0x0800C000)

#ifdef STM32F10X_XL
 #define BANK2_WRITE_START_ADDR   ((uint32_t)0x08088000)
 #define BANK2_WRITE_END_ADDR     ((uint32_t)0x0808C000)
#endif /* STM32F10X_XL */

uint8_t CrcTable[256] = {	//CRC-8
  0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D,	
  0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,	
  0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,	
  0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,	
  0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,	
  0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,	
  0x27,0x20,0x29,0x2E,0x3B,0x3C,0x35,0x32,0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A,	
  0x57,0x50,0x59,0x5E,0x4B,0x4C,0x45,0x42,0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A,	
  0x89,0x8E,0x87,0x80,0x95,0x92,0x9B,0x9C,0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4,	
  0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,0xEC,0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4,	
  0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44,	
  0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,0x21,0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34,	
  0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,0x76,0x71,0x78,0x7F,0x6A,0x6D,0x64,0x63,	
  0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,0x06,0x01,0x08,0x0F,0x1A,0x1D,0x14,0x13,	
  0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,0x96,0x91,0x98,0x9F,0x8A,0x8D,0x84,0x83,	
  0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,0xE6,0xE1,0xE8,0xEF,0xFA,0xFD,0xF4,0xF3};

int internal_flash_erase(uint32_t start_Page_Address,uint16_t page_num)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f10x_xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f10x.c file
	 */     
	uint32_t EraseCounter = 0x00;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;

	/* Porgram FLASH Bank1 ********************************************************/       
	/* Unlock the Flash Bank1 Program Erase controller */
	FLASH_UnlockBank1();

	/* Clear All pending flags */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

	/* Erase the FLASH pages */
	for(EraseCounter = 0; (EraseCounter < page_num) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
	{
		FLASHStatus = FLASH_ErasePage(start_Page_Address + (FLASH_PAGE_SIZE * EraseCounter));
	}
	FLASH_LockBank1();
	return 0;
}

int internal_flash_wr_ecartek(uint32_t flash_word_Address,uint32_t data_32)//0x8000EC00
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f10x_xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f10x.c file
	 */     
	uint32_t Address = 0;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	
	/* Porgram FLASH Bank1 ********************************************************/       
	/* Unlock the Flash Bank1 Program Erase controller */
	FLASH_UnlockBank1();

	/* Clear All pending flags */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
	
	/* Program Flash Bank1 */
	Address = flash_word_Address;
	FLASHStatus = FLASH_ProgramWord(Address, 0xECEC1313);
	if(FLASHStatus!=FLASH_COMPLETE)
	{
		return -1;
	}
	FLASHStatus = FLASH_ProgramWord(Address+4, data_32);
	if(FLASHStatus!=FLASH_COMPLETE)
	{
		return -1;
	}
	
	FLASH_LockBank1();

	return 0;
}

int internal_flash_wr_common(uint32_t flash_word_Address,uint8_t *ptr,uint32_t len_byte)//0x8000EC00
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f10x_xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f10x.c file
	 */     
	uint32_t Address = 0;
	uint32_t data_wr = 0;
	uint8_t *data_ptr=ptr;
	uint32_t i;
	uint32_t len_word;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;

	len_word=len_byte/4;
	if(len_byte%4!=0)
	{
		len_word++;
	}
	
	/* Porgram FLASH Bank1 ********************************************************/       
	/* Unlock the Flash Bank1 Program Erase controller */
	FLASH_UnlockBank1();

	/* Clear All pending flags */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
	
	/* Program Flash Bank1 */
	for(i=0;i<len_word;i++)
	{
		Address = flash_word_Address;
		data_wr = *( uint32_t*)ptr;
		FLASHStatus = FLASH_ProgramWord(Address, data_wr);
		if(FLASHStatus!=FLASH_COMPLETE)
		{
			return -1;
		}

		Address+=4;
		ptr+=4;
	}
	
	FLASH_LockBank1();

	return 0;
}

int internal_flash_rd_common(uint32_t flash_word_Address,uint8_t *rd_buf,uint32_t rd_len)//0x8000EC00
{
	uint32_t *ptr;
	uint32_t offset_addr,i;
	uint8_t tempbuf[4];
	uint32_t temp;
	uint32_t num;
	
	offset_addr=flash_word_Address;
	ptr=(uint32_t *)rd_buf;

	num=rd_len/4;

	for(i=0;i<num;i++)
	{
		ptr[i]=*( uint32_t*) flash_word_Address;	 //__IO
		flash_word_Address+=4;
	}

	if(rd_len%4!=0)
	{
		temp=*( uint32_t*) flash_word_Address;	 //__IO
		tempbuf[0]=(uint8_t)temp;
		tempbuf[1]=(uint8_t)(temp>>8);
		tempbuf[2]=(uint8_t)(temp>>16);
		tempbuf[3]=(uint8_t)(temp>>24);

		temp=rd_len%4;

		for(i=0;i<temp;i++)
		{
			rd_buf[num*4+i]=tempbuf[i];
		}
	}



	return 0;
}


uint32_t internal_flash_rd(uint32_t flash_word_addres)
{
	return *( uint32_t*) flash_word_addres;	 //__IO
}



/*******************************************************************************
* Function Name  : Cal_crc8
* Description    : CRC-8
* Input          : ptr - 数组地址
                  len - 长度
* Output         : None
* Return         : ECBYTE - 计算结果
*******************************************************************************/
uint8_t Cal_crc8(uint8_t *ptr, uint8_t len) 
{
	uint8_t CRCValue = 0;
	for(uint8_t i = 0; i < len; i++)
		CRCValue = CrcTable[CRCValue ^ ptr[i]]; //查表方式计算CRC

	return CRCValue;
}

/*******************************************************************************
* Function Name  : FLASH_ProgramMoreWord
* Description    : 连续写入多字节到指定FLASH地址，此函数必须在FLASH_WriteXXXX系列函数中使用，
* Input          : addr - 待写入起始地址，必须为4的倍数，否则会导致硬件错误
                   dwData - 待写入数组首地址
                   len - 待写入数组长度(DWORD)
* Output         : None
* Return         : None 
*******************************************************************************/
void FLASH_ProgramMoreWord(uint32_t addr, uint32_t* dwData, uint8_t len) 
{
	for(uint8_t i = 0; i < len; i++) 
	{
		FLASH_ProgramWord(addr + i * 4, *((uint32_t *)dwData + i));  //写入FLASH
	}
}

/*******************************************************************************
* Function Name  : FLASH_ReadMoreWord
* Description    : 连续读出多字节到指定FLASH地址
* Input          : addr - 待读出起始地址，必须为4的倍数，否则会导致硬件错误
                   dwData - 待读出数组首地址
                   len - 待读出数组长度
* Output         : None
* Return         : None 
*******************************************************************************/
void FLASH_ReadMoreWord(uint32_t addr, uint32_t* dwData, uint8_t len) 
{
  	for(uint8_t i = 0; i < len; i++)
  	{
   	 	*((uint32_t *)dwData + i) = *((vu32 *)addr + i); //读出FLASH数据
  	}
}


/*******************************************************************************
* Function Name  : FLASH_WriteRemoteID
* Description    : 存储遥控ID值等数据到指定FLASH地址
* Input          : remote - 遥控器属性数组指针，只获取其中20Byte数据
* Output         : None
* Return         : bool - TRUE, FALSE
*******************************************************************************/
bool FLASH_WriteRemoteID(REMOTE_STATUS* remote) 
{
	uint8_t Tmp[32];
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

	//应该要禁止中断

	//if(READ_STUDYKEY == Bit_RESET) 	//解锁FLASH写保护
	{
		FLASH_Unlock();
	}
	
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASHStatus = FLASH_ErasePage(REMOTE_ID_ADDR_HOME);	//块擦除 (1K bytes)
	if(FLASHStatus != FLASH_COMPLETE)					//设置FLASH写保护 
	{ 
		FLASH_Lock(); 
		return FALSE; 
	}
	
	remote->Crcid = Cal_crc8(&remote->Totle, 29); //20Byte
	
	FLASH_ProgramMoreWord(REMOTE_ID_ADDR_HOME,   (uint32_t *)(&remote->Crcid), 8); //保存第一块数据区
	
	FLASH_ProgramMoreWord(REMOTE_ID_ADDR_HOME+32,(uint32_t *)(&remote->Crcid), 8); //保存第二块数据区
	
	FLASH_Lock();	//设置FLASH写保护
	
	//第一块数据区比对
	FLASH_ReadMoreWord(REMOTE_ID_ADDR_HOME,   (uint32_t *)Tmp, 8);
	if(memcmp(Tmp, (uint8_t *)(&remote->Crcid), 30)) 
	{
		return FALSE; 
	}
	
	//第二块数据区比对
	FLASH_ReadMoreWord(REMOTE_ID_ADDR_HOME+32,(uint32_t *)Tmp, 8);
	if(memcmp(Tmp, (uint8_t *)(&remote->Crcid), 30)) 
	{
		return FALSE; 
	}

	return TRUE;
}


/*******************************************************************************
* Function Name  : FLASH_WriteRemoteLOQ
* Description    : 存储遥控LOQ值等数据到指定FLASH地址
* Input          : remote - 遥控器属性数组指针，只获取其中16Byte数据
* Output         : None
* Return         : bool - TRUE, FALSE
*******************************************************************************/
bool FLASH_WriteRemoteLOQ(REMOTE_STATUS* remote)
{
	uint8_t Tmp[32];
	uint32_t addr;
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

	//应该要禁止中断。。。。。。。
	
	FLASH_Unlock();	//解锁FLASH写保护
	
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

	//每次写入，占用32字节的FLASH空间
	for(addr = REMOTE_LOQ_ADDR_HOME; ;addr += 32) // (本划分区域为3072字节) 0x0800F400
	{ 
		if(addr >= REMOTE_LOQ_ADDR_END)  									//已经写满,则擦除全部，再从起始地址开始写
		{
			FLASHStatus = FLASH_ErasePage(REMOTE_LOQ_ADDR_HOME);	  		//第一块擦除 (每块为1024字节)
			
			if(FLASHStatus == FLASH_COMPLETE)  
			{
				FLASHStatus = FLASH_ErasePage(REMOTE_LOQ_ADDR_HOME + 1024);	//第二块擦除
			}
			
			if(FLASHStatus == FLASH_COMPLETE)  
			{
				FLASHStatus = FLASH_ErasePage(REMOTE_LOQ_ADDR_HOME + 2048);	//第三块擦除
			}
			
			if(FLASHStatus != FLASH_COMPLETE) //设置FLASH写保护
			{ 
				FLASH_Lock();	
				return FALSE;
			}

			//再从起始地址开始写一小块
			addr = REMOTE_LOQ_ADDR_HOME;
		}

		//从起始地址开始，检查每32个字节的前4个字节的值,直到找到空白小块(32 bytes)
		
		if((*(vu32 *)addr & 0x00FF0000) != 0x00EC0000)  //未写区域
		{
			//每个32字节的小块的 前4个字节 中的第3个字节不是0xEC,则表示该小块未被写过。
			
			break;
		}
	}

	remote->Ecar = 0xEC;							//未写标志(因为已全局擦除过了，所以可以直接写入)
	remote->Crcloq = Cal_crc8(&remote->Index, 19); 	//16Byte
	
	FLASH_ProgramMoreWord(addr,   (uint32_t *)(&remote->Crcloq), 5); //保存第一块数据区
	
	FLASH_ProgramMoreWord(addr+32,(uint32_t *)(&remote->Crcloq), 5); //保存第二块数据区  重复保存2次 ??????????

	FLASH_Lock();//设置FLASH写保护

	FLASH_ReadMoreWord(addr,   (uint32_t *)Tmp, 5);
	if(memcmp(Tmp, (uint8_t *)(&remote->Crcloq), 20)) //第一块数据区比对
	{
		return FALSE;
	}
	
	FLASH_ReadMoreWord(addr+32,(uint32_t *)Tmp, 5); //?????????  +16  ??????????????????????????????????????
	
	if(memcmp(Tmp, (uint8_t *)(&remote->Crcloq), 20)) //第二块数据区比对
	{
		return FALSE;
	}
	
	return TRUE;
}




/*******************************************************************************
* Function Name  : FLASH_ReadRemoteID
* Description    : 从指定FLASH地址读取遥控ID值等数据
* Input          : remote - 遥控器属性数组指针，只获取其中20Byte数据
* Output         : None
* Return         : bool - TRUE, FALSE
*******************************************************************************/
bool FLASH_ReadRemoteID(REMOTE_STATUS* remote) 
{
	FLASH_ReadMoreWord(REMOTE_ID_ADDR_HOME, (uint32_t *)(&remote->Crcid), 8);  			//读第一块数据区数据 (6*4=24 bytes)
	
	if(remote->Crcid != Cal_crc8(&remote->Totle, 29))  								//数据CRC错误 
	{
		FLASH_ReadMoreWord(REMOTE_ID_ADDR_HOME + 32, (uint32_t *)(&remote->Crcid), 8); 	//读第二块数据区数据 (6*4=24 bytes)
		if(remote->Crcid != Cal_crc8(&remote->Totle, 29))  							//数据CRC错误 
		{
			memset(&remote->Crcid, 0x00, 29); //两块数据都错误，清空数据区
			return FALSE;
		}
	}
  
  	return TRUE;
}

/*******************************************************************************
* Function Name  : FLASH_ReadRemoteLOQ
* Description    : 从指定FLASH地址读取遥控LOQ值等数据
* Input          : remote - 遥控器属性数组指针，只获取其中16Byte数据
* Output         : None
* Return         : bool - TRUE, FALSE
*******************************************************************************/
bool FLASH_ReadRemoteLOQ(REMOTE_STATUS* remote) 
{
	uint32_t addr = REMOTE_LOQ_ADDR_END;
	
	while(1)   //从结尾一直查询到开始
	{
		addr -= 32;
		if((*(vu32 *)addr & 0x00FF0000) == 0x00EC0000)  //未写区域 比较起始地址的第2个字节 LOW ---HIGH (0,1,'2',3)
		{
			//查到了最新写入的LDQ
			break;
		}
		
		if(addr <= REMOTE_LOQ_ADDR_HOME)   //整大片LDQ区域都从未写过
		{
			memset(&remote->Crcloq, 0x00, 20);  //清空数据区，赋值LDQ值为0。
			return FALSE;
		}
	}
	
	FLASH_ReadMoreWord(addr, (uint32_t *)(&remote->Crcloq), 5);  //读第一块数据区数据
	if(remote->Crcloq != Cal_crc8(&remote->Index, 19))   	//数据CRC错误 
	{  
		FLASH_ReadMoreWord(addr + 32, (uint32_t *)(&remote->Crcloq), 5); //读第二块数据区数据

		if(remote->Crcloq != Cal_crc8(&remote->Index, 19))   		//数据CRC错误
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

























