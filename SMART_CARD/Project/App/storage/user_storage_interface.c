
//static head file
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

//porting head files
#include "user_rtc_app.h"
#include "user_storage_interface.h"

#include "user_debug_app.h"

#include "os.h"
#include "os_cfg_app.h"
#include "DeviceManage.h"
#include "drivers.h"

//fatfs
#include "ff.h"
#include "ffconf.h"
#include "diskio.h"

//#define sdio_interface_init_print

FATFS emmc_FatFs;

#ifdef sdio_interface_init_print

#define DMA_SIZE        (1000UL)
#define DMA_SRC			LPC_PERI_RAM_BASE		/* 	This is the area original data is stored
													or data to be written to the SD/MMC card. */
													
#define DMA_DST			(DMA_SRC+DMA_SIZE)		/* 	This is the area, after writing to the SD/MMC,
													data read from the SD/MMC card. */

#define WRITE_BLOCK_NUM	 (4)
#define WRITE_LENGTH    (BLOCK_LENGTH*WRITE_BLOCK_NUM)
/* treat WriteBlock as a constant address */
volatile uint8_t *WriteBlock = (uint8_t *)(DMA_SRC);

/* treat ReadBlock as a constant address */
volatile uint8_t *ReadBlock  = (uint8_t *)(DMA_DST);

uint8_t wrBuf[WRITE_LENGTH]; 
uint8_t rdBuf[WRITE_LENGTH];
#endif

#if MCI_DMA_ENABLED
/******************************************************************************
**  DMA Handler
******************************************************************************/
void DMA_IRQHandler (void)
{
   MCI_DMA_IRQHandler();
}
#endif


static void emmc_pwr3v3_on (void)
{
	int i;
	
	//需留意原理图的控制逻辑!!!!
	GPIO_Init();
	
	PINSEL_ConfigPin(5, 2, 0);
	GPIO_SetDir(5, (1<<2), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(5, (1<<2), 1);
	
	for ( i = 0; i < 1000; i++ ) asm("nop");

	PINSEL_ConfigPin(0, 21, 0);
	GPIO_SetDir(0, (1<<21), GPIO_DIRECTION_OUTPUT);
	GPIO_OutputValue(0, (1<<21), 0);
}



/*
	reurn :
			0 : ok
	       <0 : fail
*/
static int sdio_interface_init(void)
{
	#ifdef sdio_interface_init_print
	uint32_t i;//, j;
	uint32_t j;//, j;
	uint32_t rcAddress;
	uint32_t errorState;
	#endif
	int32_t retVal = 0;
	uint8_t error = 0;

	st_Mci_CardId cidval;
	en_Mci_CardType cardType;
	//uint32_t rcAddress;
	uint32_t csdVal[4];
	//uint32_t errorState;

	// Initialize buffers for testing later
	#ifdef sdio_interface_init_print
	for(i = 0; i < WRITE_LENGTH; i++)
	{
		wrBuf[i] = i / (4*WRITE_BLOCK_NUM);
		rdBuf[i] = 0;
	}
	#endif

	emmc_pwr3v3_on();
	
	#if MCI_DMA_ENABLED
	/* on DMA channel 0, source is memory, destination is MCI FIFO. */
	/* On DMA channel 1, source is MCI FIFO, destination is memory. */
	GPDMA_Init();
	#endif

	/* For the SD card I tested, the minimum required block length is 512 */
	/* For MMC, the restriction is loose, due to the variety of SD and MMC
	card support, ideally, the driver should read CSD register to find the
	right speed and block length for the card, and set them accordingly.
	In this driver example, it will support both MMC and SD cards, and it
	does read the information by send SEND_CSD to poll the card status,
	however, to simplify the example, it doesn't configure them accordingly
	based on the CSD register value. This is not intended to support all
	the SD and MMC cards. */
	if(MCI_Init(0) != MCI_FUNC_OK) //BRD_MCI_POWERED_ACTIVE_LEVEL
	{
		return -1;
	}

	cardType = MCI_GetCardType();
	//cardType = MCI_SDSC_V2_CARD;

	switch (cardType)
	{
		case MCI_SDHC_SDXC_CARD:
			#ifdef sdio_interface_init_print
			InfoPrintf("Currently the SDXC/SDHC CARD ver2.0 is being used");
			#endif
			break;
		case MCI_SDSC_V2_CARD:
			#ifdef sdio_interface_init_print
			InfoPrintf("Currently the SD CARD ver2.0 is being used");
			#endif
			break;
		case MCI_SDSC_V1_CARD:
			#ifdef sdio_interface_init_print
			InfoPrintf("Currently the SD CARD ver1.0 is being used");
			#endif
			break;

		case MCI_MMC_CARD:
			#ifdef sdio_interface_init_print
			InfoPrintf("Currently the MMC CARD is being used");
			#endif
			break;

		case MCI_CARD_UNKNOWN:
			InfoPrintf("No CARD is being plugged, Please check!!!");
			error = 1;
			break;
	}
	
	if(error)
	{
		return -2;
	}
		
	if (MCI_GetCID(&cidval) != MCI_FUNC_OK)
	{
		#ifdef sdio_interface_init_print
		InfoPrintf("Get CID Failed");
		#endif
		return -3;
	}
	#ifdef sdio_interface_init_print
	else
	{
		InfoPrintf("\t- Product Serial Number: 0x%X\r\n",cidval.PSN);
	}
	#endif

	/*---- Card is 'ident' state ----*/
	retVal = MCI_SetCardAddress();
	if(retVal != MCI_FUNC_OK)
	{
		#ifdef sdio_interface_init_print
		InfoPrintf("Set Card Address FAILED, retVal = %d\r\n",retVal);
		#endif
		return -4;
	}
	else
	{
		MCI_GetCardAddress();
        //rcAddress = MCI_GetCardAddress();
		#ifdef sdio_interface_init_print
		InfoPrintf("Set CARD ADDRESS OK with address =0x%X\r\n",rcAddress); 
		#endif
	}

	retVal = MCI_GetCSD(csdVal);
	if(retVal != MCI_FUNC_OK)
	{
		#ifdef sdio_interface_init_print
		InfoPrintf("Get CSD FAILED, retVal  =0x%X\r\n",retVal); 
		#endif
		return -6;
	}
	#ifdef sdio_interface_init_print
	else
	{
		InfoPrintf("Get Card Specific Data (CSD) Ok:");
		InfoPrintf("\t[0] =0x%X\r\n",csdVal[0]); 
		InfoPrintf("\t[1] =0x%X\r\n",csdVal[1]); 
		InfoPrintf("\t[2] =0x%X\r\n",csdVal[2]); 
		InfoPrintf("\t[3] =0x%X\r\n",csdVal[3]); 
	}
	#endif
	
	retVal = MCI_Cmd_SelectCard();

	if(retVal != MCI_FUNC_OK)
	{
		#ifdef sdio_interface_init_print
		InfoPrintf("Card Selection FAILED, retVal =0x%X\r\n",retVal); 
		#endif
		return -7;
	}
	#ifdef sdio_interface_init_print
	else
	{
		InfoPrintf("Card has been selected successfully!!!\n\r");
	}
	#endif
	
	MCI_Set_MCIClock( MCI_NORMAL_RATE );
	
	if(cardType == MCI_SDSC_V1_CARD||
		cardType == MCI_SDSC_V2_CARD||
		cardType == MCI_SDHC_SDXC_CARD)
	{
        retVal = MCI_SetBusWidth( SD_4_BIT );
		if (retVal != MCI_FUNC_OK )
		{
			#ifdef sdio_interface_init_print
			InfoPrintf("Set BandWidth FAILED, retVal =0x%X\r\n",retVal); 
			#endif
			return -9;
		}
		#ifdef sdio_interface_init_print
		else
		{
			InfoPrintf("SET Bandwidth!!!\n\r");
		}
		#endif
	}

	retVal = MCI_SetBlockLen(BLOCK_LENGTH);
	if(retVal != MCI_FUNC_OK)
	{
		#ifdef sdio_interface_init_print
		InfoPrintf("Set Block Length FAILED, retVal =0x%X\r\n",retVal); 
		#endif
		return -10;
	}
	#ifdef sdio_interface_init_print
	else
	{
		InfoPrintf("Block Length is SET successfully!!!\n\r");
	}
	#endif

	//-----read write test-----------------------------------------------------------------------

	#if 0
	
	retVal = MCI_WriteBlock(wrBuf, 0, WRITE_BLOCK_NUM);
	if(retVal != MCI_FUNC_OK)
	{
		#ifdef sdio_interface_init_print
		InfoPrintf("Write Block FAILED, retVal =0x%X\r\n",retVal); 
		#endif
		return -11;
	}
	else
	{
		//while(MCI_GetBlockXferEndState() != 0);
		while(MCI_GetDataXferEndState() != 0);
		errorState = MCI_GetXferErrState();		
		#if (WRITE_BLOCK_NUM == 1)        
		if(errorState)
		#endif
		{
			retVal = MCI_Cmd_StopTransmission();
			if(retVal != MCI_FUNC_OK)
			{
				#ifdef sdio_interface_init_print
			    InfoPrintf("Stop transmission FAILED, =0x%X\r\n",retVal); 
				#endif
			    return -12;
			}
		}
		
        if(errorState)
        {
        	#ifdef sdio_interface_init_print
            //_DBG("Write ");_DBD(WRITE_BLOCK_NUM);_DBG(" Block(s) FAILED (");_DBH32(errorState);_DBG_(")");
            InfoPrintf("write data fail\r\n"); 
			#endif
           	return -13;
        }
		#ifdef sdio_interface_init_print
        else
        {
            //_DBG("Write ");_DBD(WRITE_BLOCK_NUM);_DBG(" Block(s) successfully!!!\n\r");
			InfoPrintf("write data ok\r\n");
        }
		#endif
	}

	retVal = MCI_ReadBlock(rdBuf, 0, WRITE_BLOCK_NUM);
	if(retVal != MCI_FUNC_OK)
	{
		#ifdef sdio_interface_init_print
		InfoPrintf("Read Block FAILED, retVal =0x%X\r\n",retVal); 
		#endif
		return -14;
	}
	else
	{
		//while(MCI_GetBlockXferEndState() != 0);
		while(MCI_GetDataXferEndState() != 0);
		errorState = MCI_GetXferErrState();		
		#if (WRITE_BLOCK_NUM == 1)        
		if(errorState)
		#endif        
		{
			retVal = MCI_Cmd_StopTransmission();
			if(retVal != MCI_FUNC_OK)
			{
				#ifdef sdio_interface_init_print
			    InfoPrintf("Stop transmission FAILED, =0x%X\r\n",retVal); 
				#endif
			   	return -15;
			}
		}
		
        if(errorState)
        {
        	#ifdef sdio_interface_init_print
            //_DBG("Read ");_DBD(WRITE_BLOCK_NUM);_DBG(" Block(s) FAILED (");_DBH32(errorState);_DBG_(")");
            InfoPrintf("read data fail\r\n"); 
			#endif
            return -16;
        }
		#ifdef sdio_interface_init_print
        else
        {
            //_DBG("Read ");_DBD(WRITE_BLOCK_NUM);_DBG(" Block(s) successfully!!!\n\r");
            InfoPrintf("read data ok\r\n"); 
        }
		#endif
	}

	retVal = MCI_FUNC_OK;

	for (j = 0; j < WRITE_LENGTH; j++)
	{
		if(rdBuf[j] != wrBuf[j])
		{
			#ifdef sdio_interface_init_print
			InfoPrintf("ERROR on Read and Write at position:=0x%X\r\n",j); 
			#endif
			retVal = MCI_FUNC_FAILED;
			return -17;
		}
	}

	#ifdef sdio_interface_init_print
	if(retVal == MCI_FUNC_OK)
	{
		InfoPrintf("CHECKING is done! Read and Write correct!!!\n\r");
	}

	InfoPrintf("\n\r>>> EXAMPLES is ENDED ");
	#endif
	#endif

	return MCI_FUNC_OK;

}

int sto_user_makedir(char *dir)
{
	FRESULT fs_ret;
	int ret_val=-99;
	char buf[128+2]={0};

	if(strlen(dir)>128)
	{
		return -1;
	}

	//+2
	buf[0]='0';
	buf[1]=':';

	strcpy(&buf[2],dir);
	
	fs_ret = f_mkdir((const TCHAR*)buf);	
	if(fs_ret==FR_OK)
	{
		InfoPrintf("usr make dir ok,-%s-\r\n",dir);
		ret_val= 0;
	}
	else
	{
		ret_val= -2;
	}

	return ret_val;
}

int sto_user_deldir(char *dir)
{
	return 0;
}

int sto_user_creat_file(char *file_name)
{
	FIL *p_fil;
	FRESULT ret;
	int ret_val=-99;
	
	if(strlen(file_name)>128)
	{
		return -1;
	}
	
	p_fil=user_malloc(sizeof(FIL),__FUNCTION__);
	if(p_fil!=NULL)
	{
		ret = f_open(p_fil,(const TCHAR*)file_name, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);	
		if(ret==FR_OK)
		{
			InfoPrintf("usr make dir ok,-%s-\r\n",file_name);
			ret_val= 0;
		}
		else
		{
			ret_val= -2;
		}

		user_free(p_fil,__FUNCTION__);
	}
	else
	{
		ret_val= -3;
	}

	return ret_val;
}

int sto_user_del_file(char *dir)
{
	return 0;
}

/*
	write data from tail of the file,

	the function will auto search file tail
*/
int sto_user_wr_file(uint8_t *file_path_and_name,uint8_t *buff,uint32_t len)
{
	FIL Fat_file;
	FRESULT fs_ret;
	int ret_val;
	DWORD offset;
	
	UINT len_written=0;
	
	fs_ret = f_open(&Fat_file,file_path_and_name, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	if(fs_ret==FR_OK)
	{
		offset=Fat_file.fsize;
		fs_ret= f_lseek (	&Fat_file,	/* Pointer to the file object */
							offset		/* File pointer from top of file */
						);	
		if(fs_ret==FR_OK)
		{
			
			fs_ret= f_write(&Fat_file,			/* Pointer to the file object */
							(const void *)buff,	/* Pointer to the data to be written */
							len,				/* Number of bytes to write */
							&len_written		/* Pointer to number of bytes written */
							);
			
			if(fs_ret==FR_OK)
			{
				if(len_written==len)
				{
					fs_ret= f_close(&Fat_file);
					if(fs_ret==FR_OK)
					{
						ret_val=0;
					}
					else
					{
						ret_val=-5;
					}
				}
				else
				{
					ret_val=-4;
				}
			}
			else
			{
				ret_val=-3;
			}
		}	
		else
		{
			ret_val=-2;
		}
	}
	else
	{
		ret_val= -1;	
	}


	if(ret_val<-1)
	{
		fs_ret= f_close(&Fat_file);
	}
	
	return ret_val;
}







static void StorageTaskMsg_handle()
{

} 






void StorageTask(void *p_arg)
{
	//OS_ERR os_err;
	p_arg=p_arg;
	int ret;
	FRESULT FS_ret;

	//gps led
	//PINSEL_ConfigPin(0, 13, 0);
	//GPIO_SetDir(0, (1<<13), GPIO_DIRECTION_OUTPUT);
	//GPIO_OutputValue(0, (1<<13), 1);

	//OSSemCreate(&gps_lock,"gps_lock",1,&os_err);
	//if(os_err != OS_ERR_NONE)
	//{
	//	InfoPrintf ("create gps_lock failed !!\r\n");
	//}

	//SDIO init 
	ret=sdio_interface_init();
	if(ret==0)
	{
		InfoPrintf("emmc init ok\r\n");
		
		//fatfs check
		FS_ret = f_mount(&emmc_FatFs, "0:", 1);
		if(FS_ret == FR_OK)
		{
			InfoPrintf("f_mount() ok\r\n");
		}
		else
		{
			InfoPrintf("f_mount() fail, will format !!!\r\n");
			FS_ret = f_mkfs("0:", 0, 4096);
			if(FS_ret==FR_OK)
			{
	            InfoPrintf("format() ok\r\n");
			}
			else
			{
				InfoPrintf("format fail!!! -%d-\r\n",FS_ret);
			}
		}
		
	}
	else
	{
		InfoPrintf("emmc error !!!\r\n");
	}

	
	

	while(1)
	{
		StorageTaskMsg_handle();
		user_delay_ms(3000);
		//OSSchedRoundRobinYield(&os_err);
		InfoPrintf("StorageTask loop...\r\n");
	}

}


