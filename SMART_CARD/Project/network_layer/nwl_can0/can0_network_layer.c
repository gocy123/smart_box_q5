// porting head file
#include  "os.h"
#include "drivers.h"


//static head file  
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "ringbuffer.h"
#include "can0_network_layer.h"
#include "user_debug_app.h"


#if 1

//canÇý¶¯³õÊ¼»¯

/**********************************************************************
* $Id$		Can_Aflut.c	2011-06-02
*//**
* @file		Can_Aflut.c
* @brief	This example used to test acceptance filter operation and
* 			functions that support load/remove AFLUT entry dynamically
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/
#include "lpc_types.h"
#include "lpc177x_8x_can.h"
#include "lpc177x_8x_pinsel.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup CAN_Aflut	CAN AFLUT
 * @ingroup CAN_Examples
 * @{
 */

/************************** PRIVATE DEFINTIONS*************************/
#define MAX_ENTRY			512
#define CAN_TX_MSG_CNT		10
#define CAN_RX_MSG_CNT		5


#define RECVD_CAN_NO		(CAN_1)

#if (RECVD_CAN_NO == CAN_2)
#define RECVD_CAN_CNTRL		(CAN2_CTRL)
#else
#define RECVD_CAN_CNTRL		(CAN1_CTRL)
#endif

/************************** PRIVATE VARIABLES *************************/

//messages for test Acceptance Filter mode
__IO CAN_MSG_Type AFTxMsg[CAN_TX_MSG_CNT], AFRxMsg[CAN_RX_MSG_CNT];
__IO  uint32_t CANRxCount = 0, CANTxCount = 0;
uint32_t CANErrCount = 0;

AF_SectionDef AFTable;
FullCAN_Entry FullCAN_Table[6];
SFF_Entry SFF_Table[6];
SFF_GPR_Entry SFF_GPR_Table[6];
EFF_Entry EFF_Table[6];
EFF_GPR_Entry EFF_GPR_Table[6];

/************************** PRIVATE FUNCTIONS *************************/
void CAN_SetupAFTable(void);
void CAN_InitAFMessage(void);
void PrintMessage(CAN_MSG_Type* msg);
void print_menu(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		CAN IRQ Handler
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_IRQHandlerxxx(void)
{//nxp lib
	uint8_t IntStatus;

	//check FullCAN interrupt enable or not
	if(CAN_FullCANIntGetStatus()== SET)
	{
		//check is FullCAN interrupt occurs or not
		if ((CAN_FullCANPendGetStatus(FULLCAN_IC0))
				||(CAN_FullCANPendGetStatus(FULLCAN_IC1)))
		{
			//read received FullCAN Object in Object Section
			FCAN_ReadObj((CAN_MSG_Type *)&AFRxMsg[CANRxCount]);

			CANRxCount++;
		}
	}

	/* get interrupt status
	 * Note that: Interrupt register CANICR will be reset after read.
	 * So function "CAN_IntGetStatus" should be call only one time
	 */
	IntStatus = CAN_IntGetStatus(RECVD_CAN_NO);

	//check receive interrupt
	if((IntStatus >> 0) & 0x01)
	{
		CAN_ReceiveMsg(RECVD_CAN_NO, (CAN_MSG_Type *)&AFRxMsg[CANRxCount]);

		CANRxCount++;
	}
}

/*-------------------------PRIVATE FUNCTIONS----------------------------*/
/*********************************************************************//**
 * @brief		Print Message via COM1
 * param[in]	msg: point to CAN_MSG_Type object that will be printed
 * @return 		none
 **********************************************************************/
void PrintMessage(CAN_MSG_Type* CAN_Msg)
{
	uint8_t i;

	InfoPrintf("\r\nCAN MSG: ID=0x%04X,",CAN_Msg->id);
	InfoPrintf("ID fmt:");
	if(CAN_Msg->format==STD_ID_FORMAT)
	{
		InfoPrintf("STD,");
	}
	else
	{
		InfoPrintf("EXT,");
	}
	InfoPrintf("len=%d,",CAN_Msg->len);

	InfoPrintf("type=");
	if(CAN_Msg->type==DATA_FRAME)
	{
		InfoPrintf("DATA");
	}
	else
	{
		InfoPrintf("REMOTE");
	}

	InfoPrintf("\r\ndata: ");

	for(i=0;i<8;i++)
	{
		if(i<4)
		InfoPrintf("%02X,",CAN_Msg->dataA[i]);
		else
		InfoPrintf("%02X,",CAN_Msg->dataB[i-4]);
		
	}
	InfoPrintf("\r\n");
}

/*********************************************************************//**
 * @brief		Init AF-Look Up Table Sections entry value
 * 				We setup entries for 5 sections:
 * 				- 6 entries for FullCAN Frame Format Section
 * 				- 6 entries for Explicit Standard ID Frame Format Section
 * 				- 6 entries for Group of Standard ID Frame Format Section
 * 				- 6 entries for Explicit Extended ID Frame Format Section
 * 				- 6 entries for Group of Extended ID Frame Format Section
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_SetupAFTable(void)
{
	FullCAN_Table[0].controller = RECVD_CAN_CNTRL;
	FullCAN_Table[0].disable = MSG_ENABLE;
	FullCAN_Table[0].id_11 = 0x01;

	FullCAN_Table[1].controller = RECVD_CAN_CNTRL;
	FullCAN_Table[1].disable = MSG_ENABLE;
	FullCAN_Table[1].id_11 = 0x02;

	FullCAN_Table[2].controller = RECVD_CAN_CNTRL;
	FullCAN_Table[2].disable = MSG_ENABLE;
	FullCAN_Table[2].id_11 = 0x03;

	FullCAN_Table[3].controller = RECVD_CAN_CNTRL;
	FullCAN_Table[3].disable = MSG_ENABLE;
	FullCAN_Table[3].id_11 = 0x06;

	FullCAN_Table[4].controller = RECVD_CAN_CNTRL;
	FullCAN_Table[4].disable = MSG_ENABLE;
	FullCAN_Table[4].id_11 = 0x0C;

	FullCAN_Table[5].controller = RECVD_CAN_CNTRL;
	FullCAN_Table[5].disable = MSG_ENABLE;
	FullCAN_Table[5].id_11 = 0x0D;

	SFF_Table[0].controller = RECVD_CAN_CNTRL;
	SFF_Table[0].disable = MSG_ENABLE;
	SFF_Table[0].id_11 = 0x08;

	SFF_Table[1].controller = RECVD_CAN_CNTRL;
	SFF_Table[1].disable = MSG_ENABLE;
	SFF_Table[1].id_11 = 0x09;

	SFF_Table[2].controller = RECVD_CAN_CNTRL;
	SFF_Table[2].disable = MSG_ENABLE;
	SFF_Table[2].id_11 = 0x0A;

	SFF_Table[3].controller = RECVD_CAN_CNTRL;
	SFF_Table[3].disable = MSG_ENABLE;
	SFF_Table[3].id_11 = 0x0B;

	SFF_Table[4].controller = RECVD_CAN_CNTRL;
	SFF_Table[4].disable = MSG_ENABLE;
	SFF_Table[4].id_11 = 0x0E;

	SFF_Table[5].controller = RECVD_CAN_CNTRL;
	SFF_Table[5].disable = MSG_ENABLE;
	SFF_Table[5].id_11 = 0x0F;

	SFF_GPR_Table[0].controller1 = SFF_GPR_Table[0].controller2 = RECVD_CAN_CNTRL;
	SFF_GPR_Table[0].disable1 = SFF_GPR_Table[0].disable2 = MSG_ENABLE;
	SFF_GPR_Table[0].lowerID = 0x10;
	SFF_GPR_Table[0].upperID = 0x20;

	SFF_GPR_Table[1].controller1 = SFF_GPR_Table[1].controller2 = RECVD_CAN_CNTRL;
	SFF_GPR_Table[1].disable1 = SFF_GPR_Table[1].disable2 = MSG_ENABLE;
	SFF_GPR_Table[1].lowerID = 0x20;
	SFF_GPR_Table[1].upperID = 0x25;

	SFF_GPR_Table[2].controller1 = SFF_GPR_Table[2].controller2 = RECVD_CAN_CNTRL;
	SFF_GPR_Table[2].disable1 = SFF_GPR_Table[2].disable2 = MSG_ENABLE;
	SFF_GPR_Table[2].lowerID = 0x30;
	SFF_GPR_Table[2].upperID = 0x40;

	SFF_GPR_Table[3].controller1 = SFF_GPR_Table[3].controller2 = RECVD_CAN_CNTRL;
	SFF_GPR_Table[3].disable1 = SFF_GPR_Table[3].disable2 = MSG_ENABLE;
	SFF_GPR_Table[3].lowerID = 0x40;
	SFF_GPR_Table[3].upperID = 0x50;

	SFF_GPR_Table[4].controller1 = SFF_GPR_Table[4].controller2 = RECVD_CAN_CNTRL;
	SFF_GPR_Table[4].disable1 = SFF_GPR_Table[4].disable2 = MSG_ENABLE;
	SFF_GPR_Table[4].lowerID = 0x50;
	SFF_GPR_Table[4].upperID = 0x60;

	SFF_GPR_Table[5].controller1 = SFF_GPR_Table[5].controller2 = RECVD_CAN_CNTRL;
	SFF_GPR_Table[5].disable1 = SFF_GPR_Table[5].disable2 = MSG_ENABLE;
	SFF_GPR_Table[5].lowerID = 0x60;
	SFF_GPR_Table[5].upperID = 0x70;

	EFF_Table[0].controller = RECVD_CAN_CNTRL;
	EFF_Table[0].ID_29 = (1 << 11);

	EFF_Table[1].controller = RECVD_CAN_CNTRL;
	EFF_Table[1].ID_29 = (2 << 11);

	EFF_Table[2].controller = RECVD_CAN_CNTRL;
	EFF_Table[2].ID_29 = (3 << 11);

	EFF_Table[3].controller = RECVD_CAN_CNTRL;
	EFF_Table[3].ID_29 = (4 << 11);

	EFF_Table[4].controller = RECVD_CAN_CNTRL;
	EFF_Table[4].ID_29 = (0x0e << 11);

	EFF_Table[5].controller = RECVD_CAN_CNTRL;
	EFF_Table[5].ID_29 = (0x0f << 11);

	EFF_GPR_Table[0].controller1 = EFF_GPR_Table[0].controller2 = RECVD_CAN_CNTRL;
	EFF_GPR_Table[0].lowerEID = (5 << 11);
	EFF_GPR_Table[0].upperEID = (6 << 11);

	EFF_GPR_Table[1].controller1 = EFF_GPR_Table[1].controller2 = RECVD_CAN_CNTRL;
	EFF_GPR_Table[1].lowerEID = (7 << 11);
	EFF_GPR_Table[1].upperEID = (8 << 11);

	EFF_GPR_Table[2].controller1 = EFF_GPR_Table[2].controller2 = RECVD_CAN_CNTRL;
	EFF_GPR_Table[2].lowerEID = (9 << 11);
	EFF_GPR_Table[2].upperEID = (0x0a << 11);

	EFF_GPR_Table[3].controller1 = EFF_GPR_Table[3].controller2 = RECVD_CAN_CNTRL;
	EFF_GPR_Table[3].lowerEID = (0x0b << 11);
	EFF_GPR_Table[3].upperEID = (0x0c << 11);

	EFF_GPR_Table[4].controller1 = EFF_GPR_Table[4].controller2 = RECVD_CAN_CNTRL;
	EFF_GPR_Table[4].lowerEID = (0x11 << 11);
	EFF_GPR_Table[4].upperEID = (0x12 << 11);

	EFF_GPR_Table[5].controller1 = EFF_GPR_Table[5].controller2 = RECVD_CAN_CNTRL;
	EFF_GPR_Table[5].lowerEID = (0x13 << 11);
	EFF_GPR_Table[5].upperEID = (0x14 << 11);

	//AFTable.FullCAN_Sec = &FullCAN_Table[0];
	//AFTable.FC_NumEntry = 6;

	AFTable.SFF_Sec = &SFF_Table[0];
	AFTable.SFF_NumEntry = 6;

	AFTable.SFF_GPR_Sec = &SFF_GPR_Table[0];
	AFTable.SFF_GPR_NumEntry = 6;

	AFTable.EFF_Sec = &EFF_Table[0];
	AFTable.EFF_NumEntry = 6;

	AFTable.EFF_GPR_Sec = &EFF_GPR_Table[0];
	AFTable.EFF_GPR_NumEntry = 6;
}

/*********************************************************************//**
 * @brief		Change AFLUT table
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_ChangeAFTable(void)
{
	CAN_LoadFullCANEntry(RECVD_CAN_NO, 4);

	CAN_LoadExplicitEntry(RECVD_CAN_NO, 5, STD_ID_FORMAT);

	CAN_LoadGroupEntry(RECVD_CAN_NO,0x25,0x30, STD_ID_FORMAT);

	CAN_LoadExplicitEntry(RECVD_CAN_NO, (3<<11)+0x05, EXT_ID_FORMAT);

	CAN_LoadGroupEntry(RECVD_CAN_NO,(0x0a<<11),(0x0b<<11), EXT_ID_FORMAT);

	CAN_RemoveEntry(FULLCAN_ENTRY, 0);

	CAN_RemoveEntry(EXPLICIT_STANDARD_ENTRY, 0);

	CAN_RemoveEntry(GROUP_STANDARD_ENTRY, 0);

	CAN_RemoveEntry(EXPLICIT_EXTEND_ENTRY, 2);

	CAN_RemoveEntry(GROUP_EXTEND_ENTRY, 2);

}

/*********************************************************************//**
 * @brief		Init Transmit Message
 * 				We use 10 message to test Acceptance Filter operation, include:
 * 				- 5 messages that ID exit in 5 AF Sections -> they will be receive
 * 				- 5 messages that ID not exit in 5 AF Sections -> they will be ignored
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_InitAFMessage(void)
{
	/* 1st Message with 11-bit ID which exit in AF Look-up Table in FullCAN Section */
	AFTxMsg[0].id = 0x01;
	AFTxMsg[0].len = 0x08;
	AFTxMsg[0].type = DATA_FRAME;
	AFTxMsg[0].format = STD_ID_FORMAT;
	AFTxMsg[0].dataA[0] = AFTxMsg[0].dataA[1] = AFTxMsg[0].dataA[2]= AFTxMsg[0].dataA[3]= 0x78;
	AFTxMsg[0].dataB[0] = AFTxMsg[0].dataB[1] = AFTxMsg[0].dataB[2]= AFTxMsg[0].dataB[3]= 0x21;

	/* 2nd Message with 11-bit ID which not exit in AF Look-up Table */
	AFTxMsg[1].id = 0x04;
	AFTxMsg[1].len = 0x08;
	AFTxMsg[1].type = DATA_FRAME;
	AFTxMsg[1].format = STD_ID_FORMAT;
	AFTxMsg[1].dataA[0] = AFTxMsg[1].dataA[1] = AFTxMsg[1].dataA[2]= AFTxMsg[1].dataA[3]= 0x23;
	AFTxMsg[1].dataB[0] = AFTxMsg[1].dataB[1] = AFTxMsg[1].dataB[2]= AFTxMsg[1].dataB[3]= 0x45;

	/* 3th Message with 11-bit ID which exit in AF Look-up Table in SFF Section*/
	AFTxMsg[2].id = 0x08;
	AFTxMsg[2].len = 0x08;
	AFTxMsg[2].type = DATA_FRAME;
	AFTxMsg[2].format = STD_ID_FORMAT;
	AFTxMsg[2].dataA[0] = AFTxMsg[2].dataA[1] = AFTxMsg[2].dataA[2]= AFTxMsg[2].dataA[3]= 0x15;
	AFTxMsg[2].dataB[0] = AFTxMsg[2].dataB[1] = AFTxMsg[2].dataB[2]= AFTxMsg[2].dataB[3]= 0x36;

	/* 4th Message with 11-bit ID which not exit in AF Look-up Table */
	AFTxMsg[3].id = 0x05;
	AFTxMsg[3].len = 0x08;
	AFTxMsg[3].type = DATA_FRAME;
	AFTxMsg[3].format = STD_ID_FORMAT;
	AFTxMsg[3].dataA[0] = AFTxMsg[3].dataA[1] = AFTxMsg[3].dataA[2]= AFTxMsg[3].dataA[3]= 0x78;
	AFTxMsg[3].dataB[0] = AFTxMsg[3].dataB[1] = AFTxMsg[3].dataB[2]= AFTxMsg[3].dataB[3]= 0x21;

	/* 5th Message with 11-bit ID which exit in AF Look-up Table in Group SFF Section*/
	AFTxMsg[4].id = 0x15;
	AFTxMsg[4].len = 0x08;
	AFTxMsg[4].type = DATA_FRAME;
	AFTxMsg[4].format = STD_ID_FORMAT;
	AFTxMsg[4].dataA[0] = AFTxMsg[4].dataA[1] = AFTxMsg[4].dataA[2]= AFTxMsg[4].dataA[3]= 0x65;
	AFTxMsg[4].dataB[0] = AFTxMsg[4].dataB[1] = AFTxMsg[4].dataB[2]= AFTxMsg[4].dataB[3]= 0x37;

	/* 6th Message with 11-bit ID which not exit in AF Look-up Table */
	AFTxMsg[5].id = 0x26;
	AFTxMsg[5].len = 0x08;
	AFTxMsg[5].type = DATA_FRAME;
	AFTxMsg[5].format = STD_ID_FORMAT;
	AFTxMsg[5].dataA[0] = AFTxMsg[5].dataA[1] = AFTxMsg[5].dataA[2]= AFTxMsg[5].dataA[3]= 0x76;
	AFTxMsg[5].dataB[0] = AFTxMsg[5].dataB[1] = AFTxMsg[5].dataB[2]= AFTxMsg[5].dataB[3]= 0x32;

	/* 7th Message with 29-bit ID which exit in AF Look-up Table in EFF Section */
	AFTxMsg[6].id = (3 << 11); //0x00001800
	AFTxMsg[6].len = 0x08;
	AFTxMsg[6].type = DATA_FRAME;
	AFTxMsg[6].format = EXT_ID_FORMAT;
	AFTxMsg[6].dataA[0] = AFTxMsg[6].dataA[1] = AFTxMsg[6].dataA[2]= AFTxMsg[6].dataA[3]= 0x45;
	AFTxMsg[6].dataB[0] = AFTxMsg[6].dataB[1] = AFTxMsg[6].dataB[2]= AFTxMsg[6].dataB[3]= 0x87;

	/* 8th Message with 29-bit ID which not exit in AF Look-up Table */
	AFTxMsg[7].id = (3 << 11) + 0x05; //0x00001801
	AFTxMsg[7].len = 0x08;
	AFTxMsg[7].type = DATA_FRAME;
	AFTxMsg[7].format = EXT_ID_FORMAT;
	AFTxMsg[7].dataA[0] = AFTxMsg[7].dataA[1] = AFTxMsg[7].dataA[2]= AFTxMsg[7].dataA[3]= 0x78;
	AFTxMsg[7].dataB[0] = AFTxMsg[7].dataB[1] = AFTxMsg[7].dataB[2]= AFTxMsg[7].dataB[3]= 0x21;

	/* 9th Message with 29-bit ID which exit in AF Look-up Table in Group of EFF Section*/
	AFTxMsg[8].id = (9 << 11) + 0x01; //0x00004801
	AFTxMsg[8].len = 0x08;
	AFTxMsg[8].type = DATA_FRAME;
	AFTxMsg[8].format = EXT_ID_FORMAT;
	AFTxMsg[8].dataA[0] = AFTxMsg[8].dataA[1] = AFTxMsg[8].dataA[2]= AFTxMsg[8].dataA[3]= 0x52;
	AFTxMsg[8].dataB[0] = AFTxMsg[8].dataB[1] = AFTxMsg[8].dataB[2]= AFTxMsg[8].dataB[3]= 0x06;

	/* 10th Message with 29-bit ID which not exit in AF Look-up Table */
	AFTxMsg[9].id = (0x0A << 11) + 0x01; //0x00005001
	AFTxMsg[9].len = 0x08;
	AFTxMsg[9].type = DATA_FRAME;
	AFTxMsg[9].format = EXT_ID_FORMAT;
	AFTxMsg[9].dataA[0] = AFTxMsg[9].dataA[1] = AFTxMsg[9].dataA[2]= AFTxMsg[9].dataA[3]= 0x85;
	AFTxMsg[9].dataB[0] = AFTxMsg[9].dataB[1] = AFTxMsg[9].dataB[2]= AFTxMsg[9].dataB[3]= 0x27;

	AFRxMsg[0].id = AFRxMsg[1].id = AFRxMsg[2].id = AFRxMsg[3].id = AFRxMsg[4].id = 0x00;
	AFRxMsg[0].len = AFRxMsg[1].len = AFRxMsg[2].len = AFRxMsg[3].len = AFRxMsg[4].len = 0x00;
	AFRxMsg[0].type = AFRxMsg[1].type = AFRxMsg[2].type = AFRxMsg[3].type = AFRxMsg[4].type = 0x00;
	AFRxMsg[0].format = AFRxMsg[1].format = AFRxMsg[2].format = AFRxMsg[3].format = AFRxMsg[4].format = 0x00;
	AFRxMsg[0].dataA[0] = AFRxMsg[1].dataA[0] = AFRxMsg[2].dataA[0] = AFRxMsg[3].dataA[0] = AFRxMsg[4].dataA[0] = 0x00;
	AFRxMsg[0].dataA[1] = AFRxMsg[1].dataA[1] = AFRxMsg[2].dataA[1] = AFRxMsg[3].dataA[1] = AFRxMsg[4].dataA[1] = 0x00;
	AFRxMsg[0].dataA[2] = AFRxMsg[1].dataA[2] = AFRxMsg[2].dataA[2] = AFRxMsg[3].dataA[2] = AFRxMsg[4].dataA[2] = 0x00;
	AFRxMsg[0].dataA[3] = AFRxMsg[1].dataA[3] = AFRxMsg[2].dataA[3] = AFRxMsg[3].dataA[3] = AFRxMsg[4].dataA[3] = 0x00;

	AFRxMsg[0].dataB[0] = AFRxMsg[1].dataB[0] = AFRxMsg[2].dataB[0] = AFRxMsg[3].dataB[0] = AFRxMsg[4].dataB[0] = 0x00;
	AFRxMsg[0].dataB[1] = AFRxMsg[1].dataB[1] = AFRxMsg[2].dataB[1] = AFRxMsg[3].dataB[1] = AFRxMsg[4].dataB[1] = 0x00;
	AFRxMsg[0].dataB[2] = AFRxMsg[1].dataB[2] = AFRxMsg[2].dataB[2] = AFRxMsg[3].dataB[2] = AFRxMsg[4].dataB[2] = 0x00;
	AFRxMsg[0].dataB[3] = AFRxMsg[1].dataB[3] = AFRxMsg[2].dataB[3] = AFRxMsg[3].dataB[3] = AFRxMsg[4].dataB[3] = 0x00;
}



/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main CAN program body
 * @param[in]	none
 * @return 		none
 **********************************************************************/
static void can0_init(void)
{
	volatile uint32_t cnt;
	CAN_ERROR error;

	CANRxCount = CANTxCount = 0;

	/* Pin configuration
	 * CAN1: select P0.0 as RD1. P0.1 as TD1
	 * CAN2: select P0.4 as RD2, P0.5 as RD2
	 */
	PINSEL_ConfigPin (0, 0, 1);
	PINSEL_ConfigPin (0, 1, 1);

	#if (RECVD_CAN_NO != CAN_1)
	PINSEL_ConfigPin (0, 4, 2);

	PINSEL_ConfigPin (0, 5, 2);
	#endif

	//Initialize CAN1 & CAN2
	CAN_Init(CAN_1, 500000);

	#if (RECVD_CAN_NO != CAN_1)
	CAN_Init(RECVD_CAN_NO, 125000);
	#endif

	//Enable Receive Interrupt
	CAN_IRQCmd(RECVD_CAN_NO, CANINT_FCE, ENABLE);
	CAN_IRQCmd(RECVD_CAN_NO, CANINT_RIE, ENABLE);
	
	CAN_IRQCmd(RECVD_CAN_NO, CANINT_TIE1, ENABLE);	
	CAN_IRQCmd(RECVD_CAN_NO, CANINT_TIE2, ENABLE);	
	CAN_IRQCmd(RECVD_CAN_NO, CANINT_TIE3, ENABLE);

	//Enable CAN Interrupt
	NVIC_EnableIRQ(CAN_IRQn);

	/* First, we send 10 messages:
	 * - message 0,2,4,6,8 have id in AFLUT >>> will be received
	 * - message 1,3,5,7,9 don't have id in AFLUT >>> will be ignored
	 * Then, we change AFLUT by load/remove entries in AFLUT and re-send messages
	 * - message 1,3,5,7,9 have id in AFLUT >>> will be received
	 * - message 0,2,4,6,8 don't have id in AFLUT >>> will be ignored
	 * Note that: FullCAN Object must be read from FullCAN Object Section next to AFLUT
	 */
	/*-------------------------Init Message & AF Look-up Table------------------------*/
	/* initialize Transmit Message */
	CAN_InitAFMessage();

	/* initialize AF Look-up Table sections*/
	CAN_SetupAFTable();

	/* install AF Look-up Table */
	error = CAN_SetupAFLUT(&AFTable);
	if (error != CAN_OK)
	{
		InfoPrintf("Setup AF: ERROR...\r\n");
		while (1); // AF Table has error
	}
	else
	{
		InfoPrintf("Setup AF: SUCCESSFUL!!!\r\n");
	}


	CAN_ChangeAFTable();

	InfoPrintf("Change AFLUT: FINISHED!!!\r\n");

	CAN_SetAFMode(CAN_EFCAN);

	CAN_InitAFMessage();
}

#endif

#define ___________CAN0__PORT_AREA____START______

static int can0_init_link(void);
inline int can0_wr_rx_link_buffer(uint8_t *buf, int len);
static int can0_trig_a_tx_interrupt(void);

#define CAN0_SEND_DATA_RETRY_TIMES  (1000)

enum can0_send_state{
	CAN0_SEND_IDLE=0,
	CAN0_SEND_SENDING
};
//rx ringbuffer
#define CAN0_RX_RINGBUF_LEN    (256*sizeof(can0_data_package_t))
static struct rt_ringbuffer can0_rx_ringbuffer_controller={0};
static uint8_t can0_rx_ringbuf[CAN0_RX_RINGBUF_LEN]={0};

//tx ringbuffer
#define CAN0_TX_RINGBUF_LEN    (128*sizeof(can0_data_package_t))
static struct rt_ringbuffer can0_tx_ringbuffer_controller={0};
static uint8_t can0_tx_ringbuf[CAN0_TX_RINGBUF_LEN]={0};

//sending status flag
static enum can0_send_state can0_send_sta=CAN0_SEND_IDLE;


static void can0_retry_delay(void)
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


void user_can0_open(void)
{
	//--
	can0_init();
	//config link layer
	can0_init_link();
}

void user_can0_close(void)
{

}

void user_can0_wakeup(void)
{

}


void user_can0_sleep(void)
{

}

void user_can0_clear_err_counter(void)
{
	
}


void user_can0_reset(void)
{
	user_can0_close();
	user_can0_open();
}

static void user_can0_en_tx_int(void)
{
	CAN_IRQCmd(RECVD_CAN_NO, CANINT_TIE1, ENABLE);	
	CAN_IRQCmd(RECVD_CAN_NO, CANINT_TIE2, ENABLE);	
	CAN_IRQCmd(RECVD_CAN_NO, CANINT_TIE3, ENABLE);
}

static void user_can0_dis_tx_int(void)
{
	CAN_IRQCmd(RECVD_CAN_NO, CANINT_TIE1, DISABLE);	
	CAN_IRQCmd(RECVD_CAN_NO, CANINT_TIE2, DISABLE);	
	CAN_IRQCmd(RECVD_CAN_NO, CANINT_TIE3, DISABLE);
}



/*
	brief:  write one minimum data package to peripheral TX regs

	input: 
			databyte: one msg will be sent
	return:
			0: send fail
			1: send ok
*/
static int user_can0_transmit_one_data_package(can0_data_package_t package)
{
	int ret;
	ret=CAN_SendMsg(0, (CAN_MSG_Type *)&package);
	return ret;
}

/*
	brief: read one minimum data package from the peripheral RX regs

	input: 
			databyte: pointer that point to the rx databyte
	return:
			0: receive one byte fail
			1: receive one byte ok
*/
static int user_can0_receive_one_data_package(can0_data_package_t *package)
{
	int ret;
		
	ret=CAN_ReceiveMsg(RECVD_CAN_NO, package);
	
	return ret;
}


void CAN_IRQHandler(void)
{
	uint8_t IntStatus;
	
	can0_data_package_t temp_msg;

	//check FullCAN interrupt enable or not    ------not use this function ,zgc
	if(CAN_FullCANIntGetStatus()== SET)
	{
		//check is FullCAN interrupt occurs or not
		if ((CAN_FullCANPendGetStatus(FULLCAN_IC0))
				||(CAN_FullCANPendGetStatus(FULLCAN_IC1)))
		{
			//read received FullCAN Object in Object Section
			FCAN_ReadObj((CAN_MSG_Type *)&AFRxMsg[CANRxCount]);

			CANRxCount++;
		}
	}

	/* get interrupt status
	 * Note that: Interrupt register CANICR will be reset after read.
	 * So function "CAN_IntGetStatus" should be call only one time
	 */
	IntStatus = CAN_IntGetStatus(RECVD_CAN_NO);

	//check receive interrupt
	if((IntStatus >> 0) & 0x01)
	{
		if(user_can0_receive_one_data_package(&temp_msg)==1)
		{
			//PrintMessage(&temp_msg);
			if(can0_wr_rx_link_buffer((uint8_t *)&temp_msg,sizeof(temp_msg))!=1)
			{
				//InfoPrintf("CAN0 RX link buffer full\r\n");
			}
		}
	}

	//tx interupt
	if((((IntStatus>>1)&0x01)!=0)||(((IntStatus>>9)&0x01)!=0)||(((IntStatus>>10)&0x01)!=0))
    {
		can0_trig_a_tx_interrupt();
    }
}

#define __________CAN0__PORT_AREA____END________





/*
	return:
			0: idle
			1: busy,can`t write data to tx buf
*/
int can0_get_busy_state(void)
{
	return can0_send_sta;
}


static int can0_init_link(void)
{
	rt_ringbuffer_init(&can0_rx_ringbuffer_controller,
						can0_rx_ringbuf,
						CAN0_RX_RINGBUF_LEN);
	
	rt_ringbuffer_init(&can0_tx_ringbuffer_controller,
							can0_tx_ringbuf,
							CAN0_TX_RINGBUF_LEN);

	can0_send_sta=CAN0_SEND_IDLE;

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
static int can0_rd_one_package_from_tx_ringbuffer(can0_data_package_t *package)
{
	uint16_t package_size=sizeof(can0_data_package_t);
	uint32_t ret_len;

	if(rt_ringbuffer_data_len(&can0_tx_ringbuffer_controller)<package_size)
	{
		return 0;
	}

	ret_len=rt_ringbuffer_get(	&can0_tx_ringbuffer_controller,
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
static int can0_trig_a_tx_interrupt(void)
{
	int ret;
	can0_data_package_t tx_data_package;
	
	ret = can0_rd_one_package_from_tx_ringbuffer(&tx_data_package);
	if(ret==1)
	{
		if(user_can0_transmit_one_data_package(tx_data_package)==1)
		{
			user_can0_en_tx_int();
			can0_send_sta=CAN0_SEND_SENDING;
			return 1;
		}
	}
	else if(ret==0)
	{
		//no data in ringbuffer
		user_can0_dis_tx_int();
		can0_send_sta=CAN0_SEND_IDLE;
		return 0;
	}
	else if(ret==-1)
	{
		//error data in tx ringbuffer ,neet to be reset
		user_can0_dis_tx_int();
		can0_send_sta=CAN0_SEND_IDLE;
		return -2;
	}

	user_can0_dis_tx_int();
	can0_send_sta=CAN0_SEND_IDLE;
	return 0;
}


/*
	write (1 ~~ CAN0_TX_RINGBUF_LEN) bytes to uart ringbuf

	input:
			buf: input data
			len: bytes amount need to write
	return:
			the real amount that written to ringbuf,
			it must be the 0 or len !!!!!!

	note : this function is called by can0_send_data() only, other app 
	       function should not call this function
*/
static int can0_wr_tx_link_buf_max_len(uint8_t *buf, uint32_t len)
{	
	int empty_len;
	int ret_len=0;
	//int ret;

	if(len>CAN0_TX_RINGBUF_LEN)
	{
		return 0;
	}
	else
	{
		empty_len=rt_ringbuffer_space_len(&can0_tx_ringbuffer_controller);
		if(empty_len>=len)
		{
			ret_len = rt_ringbuffer_put(&can0_tx_ringbuffer_controller,buf,len);
			
			if(can0_send_sta==CAN0_SEND_IDLE)
			{				
				can0_trig_a_tx_interrupt();
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
			databyte: UART1 rx reg data
	return:
			0:failed
			1:success
*/
int can0_wr_rx_link_buffer(uint8_t *buf, int len)
{	  
	return rt_ringbuffer_put(&can0_rx_ringbuffer_controller,buf,len);
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
int can0_send_data(uint8_t *buf, uint32_t len)
{	
	int ret=0;
	int ret_len=0;
	int i=0,j=0,k=0;
	uint16_t retry_times;
	uint16_t retry_times_2;
	
	uint8_t *ptr;
	
	ptr=buf;

	if(len>CAN0_TX_RINGBUF_LEN)
	{
		i=(len/CAN0_TX_RINGBUF_LEN); //
		j=(len%CAN0_TX_RINGBUF_LEN); //
		
		retry_times=CAN0_SEND_DATA_RETRY_TIMES;
		for(k=0;k<i;)
		{
			if(retry_times<=0)
			{
				break;
			}
			retry_times--;
			
			retry_times_2=CAN0_SEND_DATA_RETRY_TIMES;
			while(can0_get_busy_state()==CAN0_SEND_SENDING)
			{
				can0_retry_delay();
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
			
			ret=can0_wr_tx_link_buf_max_len(ptr,CAN0_TX_RINGBUF_LEN);
			ret_len+=ret;
			if(ret == CAN0_TX_RINGBUF_LEN)
			{
				retry_times=CAN0_SEND_DATA_RETRY_TIMES;
				ptr+=CAN0_TX_RINGBUF_LEN;
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
				retry_times=CAN0_SEND_DATA_RETRY_TIMES;
				while(1)
				{
					ret=can0_wr_tx_link_buf_max_len(ptr,j);
					if(ret==0)
					{
						can0_retry_delay();
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
		retry_times=CAN0_SEND_DATA_RETRY_TIMES;
		while(1)
		{
			ret=can0_wr_tx_link_buf_max_len(buf,len);
			if(ret==0)
            {
              can0_retry_delay();
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
int can0_receive_data(uint8_t *buf,int read_len)
{  
	uint16_t package_size=sizeof(can0_data_package_t);
	//uint32_t ret_len;
	uint32_t ringbuf_data_len;

	if(read_len>CAN0_RX_RINGBUF_LEN)
	{
		return -1;
	}

	if(read_len%package_size!=0)
	{
		return -1;//illegal read length
	}

	ringbuf_data_len=rt_ringbuffer_data_len(&can0_rx_ringbuffer_controller);
	
	if(ringbuf_data_len<package_size)
	{
		return 0;
	}

	if(read_len<=ringbuf_data_len)
	{
		return rt_ringbuffer_get(  &can0_rx_ringbuffer_controller,
									buf,
								   (uint16_t)read_len
								);
	}
	else
	{
		return rt_ringbuffer_get(  &can0_rx_ringbuffer_controller,buf,
		(((uint16_t)ringbuf_data_len/sizeof(can0_data_package_t))*sizeof(can0_data_package_t)));
	}
}



















