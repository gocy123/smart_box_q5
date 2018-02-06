//static head file
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

//porting head files
#include "heads.h"

#include "stm32f10x_iwdg.h"


#include "user_debug_app.h"
#include "user_internal_flash.h"
#include "os.h"
#include "os_cfg_app.h"
#include "user_rf_app.h"

#include "Encoding.h"
#include "Encoding_v8.h"





//#define UHFFILTER             	(0x0014)  //40us
#define UHFFILTER             	(0x0014)  //40us
#define UHFPLUSE_V7           	(0x0127)  //590us
#define UHFMAST_V7            	(0x00C4)  //

#define REMOTE_MARK_V8			(0xE3)
#define REMOTE_MARK_V7			(0xC1)

volatile bool TIM2_counting_now_flag = FALSE; //TIM2��ʱ��������־
static u8 	Data[15];        		//RF�������ݱ���
static REMOTE_STATUS 		Remote={0}; 
static u8 ClearKeyValueCount = 0;
static u8 RemoteLastData = 0;  //ң�������ݳ�֡���֡�Ĺ�ͬ����
static u16 RemoteKeyValue = 0; //ң������ֵ
static CAR_POWER_STATUS 	car_power=CAR_OFF;
static u8  smart_card_need_dly_pwr_off_flag=0;
static u8  smart_card_pwr_status=0;
static u32 remote_pwr_on_card_timestamp=0;
static u8 key_sn=0;
static uint32_t MCURegID=0;
static uint32_t factory_trace_id=0;
static u8 on_change_flag=0xFF;
static uint8_t on_status=0;
static u8 remote_started_flag=0;


static uint32_t KEY_TRUNK_L_time_stamp=0;
static uint32_t KEY_UNLOCK_L_time_stamp=0;
static uint32_t KEY_LOCK_L_time_stamp=0;


#if 1
#define  CHECK_BUF_LEN 20
static uint8_t	 check_bat_vol_ok_flag=0xFF;
static uint16_t check_bat_vol_buf[CHECK_BUF_LEN]={0};
static uint8_t	 check_bat_vol_buf_offset=0;
static uint8_t	 bat_cali_ok_flag=0;
static uint32_t bat_off_average=0;
static uint32_t bat_engined_average=0;
static uint32_t bat_current=0;
static uint32_t start_calibrate_bat_timestamp=0;
static uint32_t bat_dif_val=0;
#endif






void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update)!=RESET) 
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		TIM_Cmd(TIM2, DISABLE);//���ܶ�ʱ��
		TIM2_counting_now_flag = FALSE;
	}
}

void DelayXms(vu32 nTime)
{
	OS_ERR os_err;
	OSTimeDly(nTime, OS_OPT_TIME_DLY, &os_err);
}

void power_on_smart_card(void)
{
	OS_ERR os_err;
	
	InfoPrintf("power_on_smart card()\r\n");
	KEY_PWR_ON;
	smart_card_pwr_status=1;
	remote_pwr_on_card_timestamp=OSTimeGet(&os_err);
	smart_card_need_dly_pwr_off_flag=1;
}

void power_off_smart_card(void)
{
	InfoPrintf("power_off_smart card()\r\n");
	KEY_PWR_OFF;
	smart_card_need_dly_pwr_off_flag=0;
	smart_card_pwr_status=0;
}


/*******************************************************************************
* Function Name  : DriveAlarm
* Description    : �豸����������
* Input          : index - ���д���
                   time - ���г���ʱ��
* Output         : None
* Return         : None 
*******************************************************************************/
void DriveAlarm(u8 index, u16 time) 
{
	while(index) 
	{
		BEEP_ON;   
		DelayXms(time);  
		BEEP_OFF; 
		
		if(--index == 0) 
		{
			break; //���н���
		}
		else
		{
			DelayXms(time);  //��ʱ���´�����
		}
		IWDG_ReloadCounter();
	}
}

void AddTimeDriveAlarm(u8 index, u16 time) 
{
	while(index) 
	{
		BEEP_ON; 
		//LED_ON;
		DelayXms(time);  
		BEEP_OFF; 
		//LED_OFF;
		
		if(--index == 0) 
		{
			break; //���н���
		}
		else
		{
			DelayXms(time);  //��ʱ���´�����
		}
	}
}

void LED_flash(u8 index, u16 time) 
{
	while(index) 
	{
		//LED_ON;
		DelayXms(time);  
		//LED_OFF;
		
		if(--index == 0) 
		{
			break; //���н���
		}
		else
		{
			DelayXms(time);  //��ʱ���´�����
		}
	}
}

void control_car_power(CAR_POWER_STATUS ctl) 
{
	OS_ERR os_err;
	
	InfoPrintf("control_car_power(%d)\r\n",ctl);
	switch(ctl)
	{
		case CAR_OFF: //OFF��
			CAR_START_KEY_ON;
			DelayXms(1000);
			CAR_START_KEY_OFF;
			DelayXms(100);
			CAR_START_KEY_ON;
			DelayXms(300);
			CAR_START_KEY_OFF;
			break; 
		case CAR_ACC: //ACC��
			//START_OFF; 
			//ON_OFF; 
			//ACC_ON; 
			DelayXms(50);
			break;  
		case CAR_ON : //ON��
			//ACC_ON; 
			//ON_ON;  
			DelayXms(700);
			//START_OFF; 
			break;  
		case CAR_RUN: //���
			CAR_START_KEY_ON;
			DelayXms(800);
			CAR_START_KEY_OFF;
			DelayXms(100);
			break; 
		default: 
			break;
  	}
}


/*******************************************************************************
* Function Name  : ctl_car_remote_start_or_stop
* Description    : ��������ң�ص��
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
void ctl_car_remote_start(void) 
{	
	OS_ERR os_err;
	InfoPrintf("ctl_car_remote_start\r\n");

	if(car_power != CAR_RUN)  	//δ����
	{
		if(smart_card_pwr_status!=1)
		{
			power_on_smart_card();//���ܿ�2���Ӻ�Ҫ�ϵ磬��ֹ�Ҳ���͵������
		}
		
		DelayXms(300);
		InfoPrintf("����δ����\r\n");		
		InfoPrintf("ģ���ɲ��\r\n");
		BREAK_ON;
		KEY_LOCK_ON;
		DelayXms(600);
		KEY_LOCK_OFF;
		InfoPrintf("Զ�̷���!!!!!!\r\n");
		control_car_power(CAR_RUN);
		InfoPrintf("ģ���ɲ���ͷ�!!\r\n");
		DelayXms(400);//�����ʱ������
		BREAK_OFF;
		InfoPrintf("Զ������OK\r\n\r\n");
		remote_pwr_on_card_timestamp=OSTimeGet(&os_err);
	}
	else
	{
		InfoPrintf("�����������������ٴ�����\r\n\r\n");
	}
}

void ctl_car_remote_stop(void) 
{	
	InfoPrintf("ctl_car_remote_stop...\r\n");
	control_car_power(CAR_OFF);
	DelayXms(200);	
	InfoPrintf("Ϩ��ɹ�\r\n");

}



void GPIO_Configuration(void)
{
	#if 1
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //ʧ��JTAG,ʹ��SWD

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 


	//A�� ����
	#ifdef EN_ADC1_CHN1
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BAT_CHECK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	#else
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ON_DETECT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	#endif
	//A�� ��������
	GPIO_InitStructure.GPIO_Pin =  GPIO_PIN_RX_MISO ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//A�� ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RX_RX|GPIO_PIN_CHECK_BREAK; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//A�� �������
	GPIO_InitStructure.GPIO_Pin= GPIO_PIN_RX_SCK
								|GPIO_PIN_RX_SEL
								|GPIO_PIN_RX_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	//A�� ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RX_RX; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//A�� �����������


	

	//B�� ��������
	//GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STARTKEY;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	//GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//B�� ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ACC_DETECT|GPIO_PIN_STUDYKEY;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//B�� ��������
	
	//B�� ��©���
	//GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	//GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	//B�� �������
	GPIO_InitStructure.GPIO_Pin =   GPIO_PIN_START_KEY|GPIO_PIN_BREAK_OUT |GPIO_PIN_BEEP|
	                                GPIO_PIN_KEY_PWR|KEY_PIN_UNLOCK|KEY_PIN_LOCK|KEY_PIN_TRUNK|KEY_PIN_ALARM;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//B�� �����������
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_125KL| GPIO_PIN_125KR;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	KEY_UNLOCK_OFF;
	KEY_LOCK_OFF;;
	KEY_TRUNK_OFF;
	KEY_ALARM_OFF;
	CAR_START_KEY_OFF;
	BEEP_OFF;
	#endif
}


uc8 BasePlus[42] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,0,0,1,0,1,1,0,0,1,1,0,0,1,0};	//LF�ź�����
u16 lf_dst_WUPA=0x4148;
u16 lf_dst_WUPB=0x0000;
u8  main_key_sn=0;
u8 enable_lf_tx_flag=1;
/*******************************************************************************
* Function Name : LFSendData
* Description 	: ��Ƶ���뷢������㺯��
* Input         : lfdata - ���͵�����ָ��
                  lfcount - ���͵������ֽ���
                  outchannel - ѡ���͵�ͨ����
* Output        : None
* Return        : None
*******************************************************************************/
void LFSendData(u8 *lfdata, u8 lfcount, u16 outchannel) 
{
	//#define LFSendData_print
	u8 index = 0;
	u16 t2cycle = 64;
	u8  DataPlus[96]={0};
	//u32 i;
	u8 a,b;
	OS_ERR os_err;

	//return;

	OSSchedLock(&os_err);

	#ifdef LFSendData_print
	InfoPrintf("LFSendData(");
	for(a=0;a<lfcount;a++)
	{
		InfoPrintf("%02X,",lfdata[a]);
	}
	InfoPrintf(")\r\n");
	#else
	//InfoPrintf(".");
	#endif
	
	/*
	�ֽ����Ǵ��(��λ��ǰ)��ʽ���ͣ��ֽڼ���С�˷�ʽ
	*/

	//ÿ���ֽڵ�������Ҫ��16λ��bit������ʾ,������Ҫ����16
	//(����:һλ��Ҫһ��һ�͵ĵ�ƽ,����һ���ֽ�8λ,8x2==16); zgc add
	lfcount = lfcount << 4; 			
	for(index = 0; index < lfcount; ) 	//�����͵����ݣ�ת���ɱ���������
	{
		a=lfdata[index >> 4];
		b=(1 << (7 - ((index >> 1) & 0x07)));
	
		//if((lfdata[index >> 4] & (1 << (7 - ((index >> 1) & 0x07)))) != 0) 
		if((a & b) != 0) 
		{
			DataPlus[index++] = 0;
			DataPlus[index++] = 1;
		}
		else 
		{
			DataPlus[index++] = 1;
			DataPlus[index++] = 0;
		}
	}
	
	TIM2_counting_now_flag = TRUE;
	TIM_Cmd(TIM2, ENABLE);
	//NVIC_SETFAULTMASK();
	TIM_Cmd(LF125K_TIMx, ENABLE);
	TIM_CtrlPWMOutputs(LF125K_TIMx, ENABLE);	//������ʱ������
	TIM_CCxCmd(LF125K_TIMx, LF125KR_Channelx, TIM_CCx_Disable);
	TIM_CCxCmd(LF125K_TIMx, LF125KL_Channelx, TIM_CCx_Disable);
	TIM_SetCounter(TIM2, 0);//����  
	__disable_irq();
	
	//������������
	for(index = 0; index < 42; index++) 	
	{
		if(index == 0) 
		{
			TIM_CCxCmd(LF125K_TIMx, outchannel, BasePlus[index] == 0 ? TIM_CCx_Disable : TIM_CCx_Enable);
		}
		else 
		{
			if(BasePlus[index] == 0) 
			{
				TIM_CCxCmd(LF125K_TIMx, outchannel, TIM_CCx_Disable);
			}
			else if(BasePlus[index - 1] == 0) //��������1���м䲻����������PWM����ֹ���λ���
			{
				TIM_CCxCmd(LF125K_TIMx, outchannel, TIM_CCx_Enable);
			}
		}
		
		while(TIM_GetCounter(TIM2) < t2cycle && TIM2_counting_now_flag == TRUE) 
		{
		}
		
		t2cycle += 64;  //2*64=128us
	}

	#if 1
	//������������
	for(index = 0; index < lfcount; index++) 	
	{
		if(index == 0) 
		{
			TIM_CCxCmd(LF125K_TIMx, outchannel, DataPlus[index] == 0 ? TIM_CCx_Disable : TIM_CCx_Enable);
		}
		else 
		{
			if(DataPlus[index] == 0) 
			{
				TIM_CCxCmd(LF125K_TIMx, outchannel, TIM_CCx_Disable);
			}
			else if(DataPlus[index - 1] == 0)//��������1���м䲻����������PWM����ֹ���λ��� 
			{
				TIM_CCxCmd(LF125K_TIMx, outchannel, TIM_CCx_Enable);
			}
		}
		
		while(TIM_GetCounter(TIM2) < t2cycle && TIM2_counting_now_flag == TRUE) 
		{
		}
		
		t2cycle += 64;	//2*64=128us
	}
	#endif
	__enable_irq();
	
	TIM_CCxCmd(LF125K_TIMx, outchannel, TIM_CCx_Disable);
	TIM_Cmd(TIM2, DISABLE);
	//while(1);


	
	TIM_Cmd(LF125K_TIMx, DISABLE);
	TIM2_counting_now_flag = FALSE;

	
	
	OSSchedUnlock(&os_err);

	
}

void SendAwakeLF(void) 
{
	u8 LFData[6];
	#if 0
	//static u8 cnt=0;
	//cnt++;
	//if(cnt%3!=0)return;
	
	if(enable_lf_tx_flag==0)
	{
		InfoPrintf(".");
		return;
	}
	
	if((READ_STUDYKEY == Bit_RESET))//&&(StudyTime<STUDY_TIME*2))   //��סѧϰ����ͣ��LF
	{
		return;
	}
	
	if(car_power == CAR_RUN)// && Car->remote_started == FALSE)  //��������״̬,ͣ��LF
	{
		//InfoPrintf("���ѱ�������������WAKE UP ֡\r\n");
		return;
	}
	
	if(Remote.Index > 2) 
	{
		Remote.Index = 0;
	}
	#endif

	//InfoPrintf("���͵�Ƶ�������:\r\n");
	LFData[0] = (u8)(lf_dst_WUPA>>8);
	LFData[1] = (u8)lf_dst_WUPA;
	//LFData[0] = (u8)(0x5555>>8);
	//LFData[1] = (u8)0x5555;	
	LFData[2] = main_key_sn;
	LFData[3] = 0x88;

	//LFData[4] = 0x55;
	//LFData[5] = 0xa5;

	{
	static u8 cnt=0;
	cnt++;
	if(cnt%8==0)
	InfoPrintf("AWAKE:(KEY total:%d)(ID:%02X,%02X)(DATA:%02X,%02X)\r\n",Remote.Totle,LFData[0],LFData[1],LFData[2],LFData[3]);	
	}

	LFSendData(LFData, 4, LF125KR_Channelx);	//LFͨ��1���ͣ�2�ֽ�
	//LFSendData(LFData, 4, LF125KR_Channelx);	//LFͨ��1���ͣ�2�ֽ�
	//DelayXms(5);
	//LFSendData(LFData, 4, LF125KL_Channelx);	//LFͨ��2���ͣ�2�ֽ�
	LFSendData(LFData, 4, LF125KL_Channelx);	//LFͨ��2���ͣ�2�ֽ�
	//DelayXms(5);
}

/*
ͨ��0000ID ���ͻ���ID��Կ�ף�Կ�׽��յ��˻���ID�󣬱��棬�����ô�ID��channel 1,
֮�����彫ѭ�����ʹ�ID���Ի���Կ�ס�
*/
u8 SendStudyLF(void) 
{
	u8 LFData[6];
	u8 sn;
	//InfoPrintf("-");	
	IWDG_ReloadCounter();	//ι��
	if(car_power == CAR_RUN )//&& Car->remote_started == FALSE)  //��������״̬,ͣ��LF
	{
		InfoPrintf("cancel B\r\n");
		return 1;
	}
	
	if(Remote.Index > 2) 
	{
		Remote.Index = 0;
	}

	LFData[0]=0x00;
	LFData[1]=0x00;
	LFData[2] = (u8)(lf_dst_WUPA>>8);
	LFData[3] = (u8)lf_dst_WUPA;
	
	LFData[4] = key_sn;	

	LFData[5] = ~key_sn;	
	
	InfoPrintf("���͵�Ƶѧϰ��������:(%02X,%02X,%02X,%02X,%02X)\r\n",LFData[0],LFData[1],LFData[2],LFData[3],LFData[4]);
	LFSendData(LFData, 5, LF125KR_Channelx);	//LFͨ��1���ͣ�2�ֽ�
	LFSendData(LFData, 5, LF125KL_Channelx);	//LFͨ��2���ͣ�2�ֽ�
	LFSendData(LFData, 5, LF125KR_Channelx);	//LFͨ��1���ͣ�2�ֽ�
	LFSendData(LFData, 5, LF125KL_Channelx);	//LFͨ��2���ͣ�2�ֽ�

	return 0;
}
u8 	RangData[4] = {0,0,0,0};//START_KEY LF�������
void RangVlue(u8* rangdata) 
{
	OS_ERR os_err;
  u32 rtcvlue = user_get_os_time();//��ȡ��ǰRTCʱ��
  
  rangdata[0] = (u8)((rtcvlue>>24) ^ (rtcvlue>>16) ^ (rtcvlue>>8) ^ rtcvlue ^ (MCURegID>>8) ^ MCURegID);
  rangdata[1] = rangdata[0] ^ rangdata[1] ^ rangdata[2] ^ 0xEC;
  rangdata[2] = Cal_crc8(rangdata, 2);
  rangdata[3] = 0x00;
}

void SendStartLF(void) 
{
	#define SendStartLF_print
	u8 LFData[6];


	LFData[0] = ~(u8)(lf_dst_WUPA>>8);
	LFData[1] = ~(u8)lf_dst_WUPA;
	
	RangVlue(RangData); //���������
	RangData[3]=main_key_sn;
	
	*(u32 *)(&LFData[2]) = *(u32 *)(&RangData[0]);
	InfoPrintf("��������У��...%02X,%02X,%02X,%02X,%02X,%02X\r\n",LFData[0],LFData[1],LFData[2],LFData[3],LFData[4],LFData[5]);	
	LFSendData(LFData, 6, LF125KR_Channelx); //LFͨ��1���ͣ�6�ֽ�
	LFSendData(LFData, 6, LF125KL_Channelx); //LFͨ��2���ͣ�6�ֽ�
}


/*******************************************************************************
* Function Name : ValueRange
* Description 	: �ж�ĳ��ֵ�Ƿ��ڸ�����ֵ֮��
* Input         : compare - ���ж���ֵ
                  original - �����Ƚϵ�ֵ
                  power - �ж����ȣ�1--50%�ڣ�2--25%�ڣ�3--12.5%��
* Output        : None
* Return        : bool - ��ָ����Χ��TRUE
*******************************************************************************/
static bool ValueRange(u16 compare, u16 original, u8 power) 
{
	u16 finally = 0;
	original = original >> 1;	//��ʱ��TIM2Ϊ2us������λ����40ms = 20000
	finally = original >> power;
	if(compare > (original + finally) || compare < (original - finally)) 
	{
		return FALSE;
	}
	else 
	{
		return TRUE;
	}
}

/*******************************************************************************
* Function Name : UhfRevData_V7
* Description   : RF���ս��룬V7��ʽ
* Input         : uhfdata - ���ս�������ݴ洢��ַ
                  checktime - �������ս���ʱ����ms
* Output        : None
* Return        : u8 - ���յ����ֽ���
*******************************************************************************/
static u8 UhfRevData_V7(u8 *uhfdata, u16 checktime) 
{
	OS_ERR os_err;
	u8  index = 0;
	u8  uhf_bit_state = 0;
	u16 uhf_pulse_h = 0;
	u16 uhf_pulse_l = 0;
	u16 filter_value;
	u16 falling_timestamp;
	u16 rising_timestamp;

	OSSchedLock(&os_err);


	rising_timestamp = U16_MAX - checktime * 500;			//2us����ʱ��
	//fallingvalue = risingvalue;

	TIM2_counting_now_flag = TRUE;
	TIM_Cmd(TIM2, ENABLE);
	TIM_SetCounter(TIM2, rising_timestamp);//���ö�ʱ����ʼ��ʱֵ,��������κ���Ч������15ms�󣬻��������ж�

	while(1)//�����������T=590us
	{
		//wait low
		while(READ_UHF_IN == Bit_SET && TIM2_counting_now_flag == TRUE )	//ѭ��������壬Ϊ�͵�ƽ����
		{
			//�״�ִ��,����һֱ�Ǹߵ�ƽ����15ms��ʱ����ʱ��Ҳ��ʧ��
		}

		//low appeared,then filtering
		filter_value = TIM_GetCounter(TIM2) + UHFFILTER;	//����ë���������ʱ����
		while(TIM_GetCounter(TIM2) < filter_value && TIM2_counting_now_flag == TRUE) 
		{
		}
		while(READ_UHF_IN == Bit_SET && TIM2_counting_now_flag == TRUE) 
		{
		}

		//low stable		
		falling_timestamp = TIM_GetCounter(TIM2);
		uhf_pulse_h = falling_timestamp - rising_timestamp; //��øߵ�ƽ�����ʱ�� ----�״����е�����ʱ�������壬��׼ȷ����Ϊ��֪���ߵ�ƽ�Ǻ�ʱ��ʼ�ġ�


		//wait high
		while(READ_UHF_IN == Bit_RESET && TIM2_counting_now_flag == TRUE) 
		{
		}

		//high appear,then filtering
		filter_value = TIM_GetCounter(TIM2) + UHFFILTER;
		while(TIM_GetCounter(TIM2) < filter_value && TIM2_counting_now_flag == TRUE) 
		{
		}
		while(READ_UHF_IN == Bit_RESET && TIM2_counting_now_flag == TRUE) 
		{
		}

		//high stable
		rising_timestamp = TIM_GetCounter(TIM2);
		uhf_pulse_l = rising_timestamp - falling_timestamp; 	//��õ͵�ƽ�����ʱ��----�״����е�����ʱ�����ʱ��׼ȷ



		//check timeout
		if(TIM2_counting_now_flag == FALSE)//��ʱ����ʱ������
		{
			OSSchedUnlock(&os_err);
			return 0;
		}
		
		//�����յ��������Ƿ���Ϲ涨ֵ�����ϼ�������1�������м��κ�һ���ж���Ч��,ֱ�����㣬���´��㿪ʼ
		if(FALSE == ValueRange(uhf_pulse_h, 295, 2) || FALSE == ValueRange(uhf_pulse_l, 295, 2)) 
		{
			index=0;
		}
		else 
		{
			index++;
		}
		
		if(index > 11)	 //�������Ϲ涨ֵ�����������11������
		{
			//InfoPrintf("=======777=======");	
			index = 0;
			break;
		}
	}

	//����ǰ���15msʱ��û���꣬���Ѿ���⵽��11������������,
	//��ôTIM2_counting_now_flag ����ΪTRUE����ô�����������ĸ�ֵ������130ms�����ж��ﱻ��ΪFALSE
	
	//����պ�����15ms����ô�ͻ�TIM2_counting_now_flag����ΪFALSE


	
	TIM_SetCounter(TIM2, 0);	//���ö�ʱ����ʱ130ms 65536 * 2 us Լ����130ms
	falling_timestamp= 0;
	rising_timestamp= 0;
	memset(uhfdata, 0x00, 15); //��ʼ������
	
	TIM2_counting_now_flag = TRUE; //zgc add,��������κθ�ֵ������130ms�󣬻ᱻ��ΪFALSE
	
	while(1)	//����������壬1�����ڣ�T=1.18ms
	{
		//wait high
		while(READ_UHF_IN == Bit_RESET && TIM2_counting_now_flag == TRUE) 
		{
		}
		//hign appear,then filtering...
		filter_value = TIM_GetCounter(TIM2) + UHFFILTER;
		while(TIM_GetCounter(TIM2) < filter_value && TIM2_counting_now_flag == TRUE) 
		{
		}
		while(READ_UHF_IN == Bit_RESET && TIM2_counting_now_flag == TRUE) 
		{
		}
		//high stable
		rising_timestamp = TIM_GetCounter(TIM2);
		uhf_pulse_l = rising_timestamp - falling_timestamp; //��øߵ�ƽ�����ʱ��

		//wait low
		while(READ_UHF_IN == Bit_SET && TIM2_counting_now_flag == TRUE)
		{ 
		}
		//low appeared
		filter_value = TIM_GetCounter(TIM2) + UHFFILTER;
		while(TIM_GetCounter(TIM2) < filter_value && TIM2_counting_now_flag == TRUE) 
		{
		}
		while(READ_UHF_IN == Bit_SET && TIM2_counting_now_flag == TRUE) 
		{
		}
		
		//low stable
		falling_timestamp = TIM_GetCounter(TIM2);
		uhf_pulse_h = falling_timestamp - rising_timestamp; //��õ͵�ƽ�����ʱ��

		if(TIM2_counting_now_flag == FALSE) 
		{
			OSSchedUnlock(&os_err);
			return 0;
		}
		
		if(ValueRange(uhf_pulse_l, 590, 2) == TRUE && ValueRange(uhf_pulse_h, 590, 2) == TRUE) 
		{
			break; //only check one period ,check ok,break!
		}
	}

	TIM_SetCounter(TIM2, 0);
	filter_value = (UHFPLUSE_V7 >> 1);
	while(TIM_GetCounter(TIM2) < filter_value && TIM2_counting_now_flag == TRUE)	//��ʱ1/2T
	{
	}

	TIM_SetCounter(TIM2, 0);//130ms�ļ�ʱ
	index = 2;
	uhf_bit_state = 0;	//����֡��ʼ��ƽ
	falling_timestamp = 0;
	rising_timestamp = 0;
	do{
		//wait pin voltage change
		while(READ_UHF_IN == (BitAction)uhf_bit_state && TIM2_counting_now_flag == TRUE) 
		{
		}
		//changed ,then filter...
		filter_value = TIM_GetCounter(TIM2) + UHFFILTER;
		while(TIM_GetCounter(TIM2) < filter_value && TIM2_counting_now_flag == TRUE) 
		{
		}
		while(READ_UHF_IN == (BitAction)uhf_bit_state && TIM2_counting_now_flag == TRUE) 
		{
		}
		//filter ok,and pin voltage changed to a stable state
		rising_timestamp = TIM_GetCounter(TIM2);
		uhf_pulse_l = rising_timestamp - falling_timestamp;	//���һ�������ʱ��
		if(rising_timestamp > 60000) 						//��Ⲩ�γ���130ms�����������㣬����
		{
			rising_timestamp = 0;
			TIM_SetCounter(TIM2, 2);			//130ms
		}
		
		falling_timestamp = rising_timestamp;
		uhf_bit_state = !uhf_bit_state;
		if(uhf_pulse_l > (UHFPLUSE_V7 << 1))		//һ֡�����꣬��⵽�����涨�����壬����
		{
			break;
		}

		if(uhf_pulse_l < UHFMAST_V7) 
		{
			index++;
			if(index & 0x01)
			{
				uhfdata[index >> 4] |= uhf_bit_state << (7 - ((index & 0x0F) >> 1));	//���ֽڣ���λ����ǰ
			}
		}
		else 
		{
			index += 2;
			uhfdata[index >> 4] |= uhf_bit_state << (7 - ((index & 0x0F) >> 1));
		}

	}while(index < 238 && TIM2_counting_now_flag == TRUE);
	
	TIM_Cmd(TIM2, DISABLE);
	TIM2_counting_now_flag = FALSE;
	OSSchedUnlock(&os_err);
	return (index >> 4);
}


//bool HandRemoteUnlock;
/*******************************************************************************
* Description   : ��֤ң���������Ƿ�Ϸ�
* Input         : None
* Output        : None
* Return        : bool - TRUE, FALSE


   0  1  2  3  4  5  6  7  8  9  10 11 
RX:50,E3,00,80,06,27,59,3A,38,58,9D,2A,
-------------------------------------RX�Ʋ���ֵ:006CDA03
-----------------------------------���ؼƲ���ֵ:006CDA03
-------------------------------------------��ֵ:00000000
�����������ڷ�Χ�� 
*******************************************************************************/
static bool CheckRemoteValueIsAllow(u32 *ptr) 
{
	#define CheckRemoteValueIsAllow_print
	u32 rx_loq;
	u32 calc_loq;
	u32 rx_rf_id;
	u8 i;


	#ifdef CheckRemoteValueIsAllow_print
	InfoPrintf("����ǰ:");
	for(i=0;i<11+1;i++)
	{
		InfoPrintf("%02X,",Data[i]);
	}
	InfoPrintf("\r\n");
	#endif	
	
	if(Data[1] == REMOTE_MARK_V8) 		//......��������
	{
		IV8_Decrypt(&Data[3]);///here
	}
	else if(Data[1] == REMOTE_MARK_V7)	//......��������
	{ 
		V7_Decrypt(&Data[3]);
	}
	else 
	{
	
	};

	#ifdef CheckRemoteValueIsAllow_print
	InfoPrintf("���ܺ�:");
	for(i=0;i<11+1;i++)
	{
		InfoPrintf("%02X,",Data[i]);
	}
	InfoPrintf("\r\n");
	#endif
	

	//������8���ֽڣ������ Data[3~~10]
	
	if((Data[10] != (Data[11]&0xF0))&&(Data[11]!=0x7F)&&(Data[11]!=0x49))  //���ܺ��ֵ�����ļ�ֵ��һ�� 
	{
		#ifdef CheckRemoteValueIsAllow_print
		InfoPrintf("���ܺ��ֵ�����ļ�ֵ��һ�� \r\n");
		#endif
		
		return FALSE;
	}

	Data[10] = 0x00;  //�Ʋ�ֵ�ĸ�8λ��0 (bit[31~~24] =00000000b)

	/*
	��ǰData[3~~10]:
	  	50,E3,00,XX,XX,XX,XX,XX,XX,XX,00,2A,

	Data[3~~6] : ID	
	Data[7~~10]: �Ʋ�ֵ ,Data[10]=0x00	 0x09627B16 
	
	*/

	//Remote.Totle=2;
	//Remote.Rfid[0]=0x09627B16;
	//Remote.Rfid[1]=0x08EF4ED9;
    
    
	if(Remote.Totle>0)
	{
		#ifdef CheckRemoteValueIsAllow_print
		InfoPrintf("---��ѧϰң������:%d---\r\n",Remote.Totle);

		for(i=0;i<Remote.Totle;i++)
		{
			InfoPrintf("LOCAL[%d]=0x%08X\r\n",i,Remote.Rfid[i]);
		}
		
		InfoPrintf("RX rfid= 0x%08X \r\n",(*(u32 *) &Data[3]));
		#endif
		rx_rf_id = *(u32 *) &Data[3];
		for(u8 i = 0; i < Remote.Totle; i++)   //����뱣���ң�������ԱȽ�
		{
			if(Remote.Rfid[i] == rx_rf_id)  //IDֵһ��
			{
				rx_loq = *(u32 *)(&Data[7]);//ע�⣬Data[10]�ѱ�ǿ�Ƹ�ֵΪ0x00
				calc_loq = (rx_loq - Remote.Rfloq[i]) & 0x00FFFFFF;
				#if 0
				InfoPrintf("----���ؼƲ���ֵ:%08X\r\n",Remote.Rfloq[i]);
				InfoPrintf("------RX�Ʋ���ֵ:%08X\r\n",rx_loq);
				InfoPrintf("------------��ֵ:%08X\r\n",calc_loq);
				#endif

				
				if(/*calc_loq > 800 ||*/ calc_loq == 0)
				//if(calc_loq == 0)  
				{
					InfoPrintf("�����������ڷ�Χ��--��ֵ: %d \r\n",calc_loq);
					
					return FALSE;
				}
				else 
				{		
					#ifdef CheckRemoteValueIsAllow_print
					InfoPrintf("ң��������Ч\r\n");
					#endif		

					if(ptr!=NULL)
					{
						*((u32*)(ptr))=rx_rf_id;
					}
					Remote.Rfloq[i] = *(u32 *) &Data[7];  	//��¼�¼���
					
					FLASH_WriteRemoteLOQ(&Remote);  		//���浽FLASH
					
					return TRUE;
				}
			}

			#ifdef CheckRemoteValueIsAllow_print	
			if(i==Remote.Totle-1)
	       	{
	       	 	InfoPrintf("ID��ƥ��!!!!!LOCAL=%08X,RX=%08X!!!!\r\n\r\n\r\n",Remote.Rfid[i],(*(u32 *) &Data[3]));
	       	}
			#endif
		}
	}
	else
   	{
   		#ifdef CheckRemoteValueIsAllow_print
   	 	InfoPrintf("δѧϰRF60��ID\r\n");
		#endif
   	}
	
	return FALSE;
}

void key_release(void)
{
	OS_ERR os_err;
	
	if(KEY_TRUNK_L_time_stamp!=0)
	{
		if(OSTimeGet(&os_err)-KEY_TRUNK_L_time_stamp>T_1S)
		{
			InfoPrintf("KEY_TRUNK_OFF\r\n");
			KEY_TRUNK_L_time_stamp=0;
			KEY_TRUNK_OFF;
		}
	}
	if(KEY_UNLOCK_L_time_stamp!=0)
	{
		if(OSTimeGet(&os_err)-KEY_UNLOCK_L_time_stamp>T_1S)
		{
			InfoPrintf("KEY_UNLOCK_OFF\r\n");
			KEY_UNLOCK_L_time_stamp=0;
			KEY_UNLOCK_OFF;
		}
	}		
	if(KEY_LOCK_L_time_stamp!=0)
	{
		if(OSTimeGet(&os_err)-KEY_LOCK_L_time_stamp>T_1S)
		{
			InfoPrintf("KEY_LOCK_OFF\r\n");
			KEY_LOCK_L_time_stamp=0;
			KEY_LOCK_OFF;
		}
	}
}

static void RemoteKeyActive(u16 key) 
{	
	OS_ERR os_err;
	static uint32_t last_short_key_trunk_timestamp=0;
	static uint32_t last_short_key_unlock_timestamp=0;
	static uint32_t last_short_key_lock_timestamp=0;

	switch(key) 
	{
		case KEY_BOX:
			InfoPrintf("���յ�β��--�̰���,��ǰcard_pwr=%d\r\n",smart_card_pwr_status);
			if(smart_card_pwr_status==0)
			{
				power_on_smart_card();
				DelayXms(600);
			}
			else
			{
				remote_pwr_on_card_timestamp=OSTimeGet(&os_err);
			}
			last_short_key_trunk_timestamp=OSTimeGet(&os_err);
			KEY_TRUNK_ON;
			DelayXms(350);
			KEY_TRUNK_OFF;
			break;
		case KEY_BOX_L:
			if(last_short_key_trunk_timestamp+T_1S>OSTimeGet(&os_err))
			{
				last_short_key_trunk_timestamp=OSTimeGet(&os_err);
				InfoPrintf("���յ�β��--������,��ǰcard_pwr=%d\r\n",smart_card_pwr_status);
				if(smart_card_pwr_status==0)
				{
					power_on_smart_card();
					DelayXms(700);
				}
				else
				{
					remote_pwr_on_card_timestamp=OSTimeGet(&os_err);
				}
				
				KEY_TRUNK_ON;
				KEY_TRUNK_L_time_stamp=OSTimeGet(&os_err);
			}
			else
			{
				InfoPrintf("���յ�(��Ч)β�䳤����\r\n");
			}
			break;
		case KEY_UNLOCK:
			InfoPrintf("���յ�����--�̰���,��ǰcard_pwr=%d\r\n",smart_card_pwr_status);
			if(smart_card_pwr_status==0)
			{
				power_on_smart_card();
				DelayXms(800);
			}
			else
			{
				remote_pwr_on_card_timestamp=OSTimeGet(&os_err);
			}
			last_short_key_unlock_timestamp=OSTimeGet(&os_err);
			KEY_UNLOCK_ON;
			smart_card_need_dly_pwr_off_flag=0;
			KEY_UNLOCK_L_time_stamp=OSTimeGet(&os_err);
			break;
		case KEY_UNLOCK_L: 
			if(last_short_key_unlock_timestamp+T_1S>OSTimeGet(&os_err))
			{
				last_short_key_unlock_timestamp=OSTimeGet(&os_err);
				InfoPrintf("���յ�����--������,��ǰcard_pwr=%d\r\n",smart_card_pwr_status);
				if(smart_card_pwr_status==0)
				{
					power_on_smart_card();
					DelayXms(800);
				}
				else
				{
					remote_pwr_on_card_timestamp=OSTimeGet(&os_err);
				}
				KEY_UNLOCK_ON;
				KEY_UNLOCK_L_time_stamp=OSTimeGet(&os_err);
				smart_card_need_dly_pwr_off_flag=0;
			}
			else
			{
				InfoPrintf("���յ�(��Ч)����������,��ǰcard_pwr=%d\r\n",smart_card_pwr_status);
			}
			break;	

			
		case KEY_LOCK:
			InfoPrintf("���յ�����--�̰���,��ǰcard_pwr=%d\r\n",smart_card_pwr_status);
			if(smart_card_pwr_status==0)
			{
				power_on_smart_card();
				DelayXms(800);
			}
			else
			{
				remote_pwr_on_card_timestamp=OSTimeGet(&os_err);
				smart_card_need_dly_pwr_off_flag=1;
			}
			last_short_key_lock_timestamp=OSTimeGet(&os_err);
			KEY_LOCK_ON;
			KEY_LOCK_L_time_stamp=OSTimeGet(&os_err);
			break;
			
		case KEY_LOCK_L: 
			if(last_short_key_lock_timestamp+T_1S>OSTimeGet(&os_err))
			{
				last_short_key_lock_timestamp=OSTimeGet(&os_err);
				InfoPrintf("���յ�����--������,��ǰcard_pwr=%d\r\n",smart_card_pwr_status);
				if(smart_card_pwr_status==0)
				{
					power_on_smart_card();
					DelayXms(800);
				}
				else
				{
					remote_pwr_on_card_timestamp=OSTimeGet(&os_err);
					smart_card_need_dly_pwr_off_flag=1;
				}
				KEY_LOCK_ON;
				KEY_LOCK_L_time_stamp=OSTimeGet(&os_err);
			}
			else
			{
				InfoPrintf("���յ�(��Ч)����������,��ǰcard_pwr=%d\r\n",smart_card_pwr_status);
			}
			break;		
			
		case REMOTE_KEY_START:
			DriveAlarm(1, 100);
			if(READ_ON_LINE==0)
			{
				InfoPrintf("���յ�Զ����������\r\n");
				ctl_car_remote_start();
				on_change_flag=0;
				on_status=READ_ON_LINE;
				remote_started_flag=1;
				
				if(on_status==0)
				{	
					DriveAlarm(10, 30);
					InfoPrintf("ON���޵�,Զ������ʧ��\r\n");
					remote_started_flag=0;	
				}
			}
			else
			{
				InfoPrintf("���յ�Զ�������������ִ��\r\n");
			}
			break;
			
		case REMOTE_KEY_STOP:
			DriveAlarm(1, 100);
			if((READ_ON_LINE==1)&&(on_change_flag==0)&&(remote_started_flag==1))
			{
				InfoPrintf("���յ�Զ��Ϩ������\r\n");
				ctl_car_remote_stop();
				on_status=READ_ON_LINE;
			}
			else
			{
				InfoPrintf("���յ�Զ��Ϩ���������ִ��\r\n");
			}
			break;
			
		case KEY_ALARM_L:
			break;
		default : 
			break;
	}
}


bool ActiveImmo(void) 
{
	u8 i,j;
	u8 index;
	u8 cnt;
	u8 err=0;
	u32 RFid=0;

  	InfoPrintf("Active Immo...,Կ������=%d\r\n",Remote.Totle);
	while(1)
	{  
		if(err%3==0)
		{
			SendStartLF();  				//����LF����ң����		
		}
		
		index = UhfRevData_V7(Data, 180);	//����V8ң�����ź� 
		if(index > 6)//���յ���Ƶ����
		{
			InfoPrintf("\r\n\r\nActiveImmo��RX(%d+1):",index);
			for(cnt=0;cnt<index+1;cnt++){InfoPrintf("%02X,",Data[cnt]);}InfoPrintf("\r\n");
		
			if(CheckRemoteValueIsAllow(&RFid)==TRUE)
			{
				if(Remote.Rfid[main_key_sn]!=RFid)
				{
					for(j=0;j<4;j++)
					{
						if(Remote.Rfid[j]==RFid)
						{
							InfoPrintf("\r\n\r\n\r\n\r\n!!!!!!!!!!!!!!!!! ��ң������ű��Ϊ:%d !!!!!!!!!!!!!!!!!\r\n\r\n\r\n\r\n",j);
							main_key_sn=j;
						}
					}
				}
				//InfoPrintf("ң����RFID=%08X,��Ա��:%d\r\n",RFid,main_key_sn);
				return TRUE;
			}
		}

		if(err++ < 30)
		{
			//InfoPrintf("CCCCCC=%d,err=%d\r\n",i,err);
			IWDG_ReloadCounter();
			continue;
		}
		else
		{
			InfoPrintf("Active Immo �ȴ�ң����Ӧ��ʱ!!!!\r\n");
			break;
		}
	}

	return FALSE;
}



/*******************************************************************************
* Function Name : OnRfRev
* Description   : ��Ƶ����ң��������
* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
static void OnRfRev(void) 
{
	u8 index = 0;
	u8 i;
	
	index = UhfRevData_V7(Data, 15);	//����V7ң�����ź�
	
	if(ClearKeyValueCount == 0) 
	{
		RemoteLastData = 0;
		RemoteKeyValue = 0;
	}
	else 
	{
		ClearKeyValueCount--;
	}
	
	if(index == 0)
	{
		return;  //δ�յ�����
	}
	

	if(Data[1] != REMOTE_MARK_V7 && Data[1] != REMOTE_MARK_V8)//������־�ֲ��� 
	//if(Data[1] != REMOTE_MARK_V8)//������־�ֲ��� 
	{
		return;
	}
	
	if(Data[2] != 0x00)//������־�ֲ���
	{
			return;
	}


	if(Data[0] == 0x50)  	//��֡
	{
		RemoteLastData = Data[3];//��¼����֡һ������
		//zgc mask
		//InfoPrintf("У�鷢�����ݵ�ң����...\r\n");
		if(CheckRemoteValueIsAllow(NULL) == FALSE) 
		{
			InfoPrintf("�Ƿ� ң���� �� ң���������\r\n\r\n");
			return;//�Ƿ�ң����
		}
		InfoPrintf("У����ȷ\r\n");
		InfoPrintf("UHF���ݰ�Ϊ:");
		for(i=0;i<index+1;i++)InfoPrintf("%02X,",Data[i]);InfoPrintf("\r\n");
		
		RemoteKeyValue = Data[11];  //��¼��ֵ
		ClearKeyValueCount = 40;
	}
	else if((Data[0] == 0x51)&&(Data[1] == 0xE3)&&(Data[2] == 0x00))//��֡
	{
		#if 1
		InfoPrintf("��������ǰ:");
		for(i=0;i<index+1;i++)
		{
			InfoPrintf("%02X,",Data[i]);
		}
		InfoPrintf("\r\n");
		#endif	
		RemoteKeyValue=Data[4];

		#if 0
		if(Data[3] != RemoteLastData || Data[4] != (u8)RemoteKeyValue)   //����֡һ�����ݲ�������ֵ����
		{
			InfoPrintf("AAAA %02X,%02X\r\n",RemoteLastData,RemoteKeyValue);
			return;	
		}
		#endif

		
		//InfoPrintf("BBB %02X,%02X\r\n",RemoteLastData,RemoteKeyValue);
		RemoteLastData++; //��֡����ֻ��Чһ��
		RemoteKeyValue |= KEY_LONG; //��¼��ֵ
	}
	else 
	{
		return;
	}
	
	InfoPrintf("RemoteKeyValue=%04X\r\n",RemoteKeyValue);
	//ִ�м�ֵ��Ӧ
	RemoteKeyActive(RemoteKeyValue);  
}

/*******************************************************************************
* Function Name : OnStudyRemote
* Description   : ѧϰң��ģʽ��
(1)
���ذ��ڼ�⵽ѧϰ��������3���Ӻ�: �����������10���ӵĵ�Ƶ�ź�(����ֱ��ѧ��3��ң������RF ID ��)��
�õ�Ƶ�ź�IDΪ0x0000,����������Я���������ϱ����"��Ƶ����ID(2Bytes)"��
���⣬�ڴ��ڼ䣬����ĸ�Ƶ���ջ��������Ƿ��յ�RF60�·���
���ͽ����󣬸�Ϊѭ������"��Ƶ����ID"�Ļ���֡��

(2)
7952�ڼ�⵽CLOSE �� OPEN�������º󣬻�ѻ���ID�Ĵ�����ֵ��Ϊ0x0000,�Ա��ܹ����յ����巢����"ѧϰ����Ƶ����"��Ȼ���
����ȡ��Я����"��Ƶ����ID"�������浽FLASH�У�Ȼ�����õ�����ID�Ĵ�����֮�󣬾��ܱ�����ĵ�Ƶ֡���Ѻ�ͨ���ˡ�

(3)
RF60�ڼ�⵽CLOSE �� OPEN �������º󣬻ᷢ�Ͱ���RF60 ID�����ݰ������壬Ŀǰ����֪�ᷢ�ͼ��Ρ�


* Input         : None
* Output        : None
* Return        : None
*******************************************************************************/
static void OnStudyRemote(void) 
{
	OS_ERR os_err;
	REMOTE_STATUS NewRemote={0};  //��ң������������
	REMOTE_STATUS NewRemote_cmp={0};  //��ң������������
	u8 IDCount = 0;
	u8 index = 0;
	u32 InitTime, LastTime;
	u8 i=0;
	u8  double_id_flag=0;
	u32 double_id_val=0;
	u8 key_max_4_flag=0;
	u8 temp;
    //u8 *ptr;
	key_sn=0;
	
	memset((u8 *)&NewRemote, 0x00, sizeof(REMOTE_STATUS));

	IWDG_ReloadCounter();
	DriveAlarm(1, 1250); //����ѧϰģʽ��ʾ
	IWDG_ReloadCounter();
	DelayXms(1250);
	IWDG_ReloadCounter();
	DelayXms(1250);
	IWDG_ReloadCounter();
	DriveAlarm(1, 1250); //����ѧϰģʽ��ʾ
	IWDG_ReloadCounter();
	
	InitTime = OSTimeGet(&os_err);
	i=0;
	while(READ_STUDYKEY == Bit_RESET) //ѧϰ��������
	{
		i++;
		if(i>3)
		{
			break;
		}
		DelayXms(10);
	}
	if(i>3)
	{
		key_max_4_flag=1;
	}
	InfoPrintf("��ʼѧϰ...\r\n");
	

	while(IDCount < 4) 	//ƥ���ң������������3
	{ 
		IWDG_ReloadCounter();
		index = UhfRevData_V7(Data, 50);
		if(index > 9)  //����V7ң�����ź�
		{
			InfoPrintf("���յ�ң��������(%d+1):",index);
			for(i=0;i<index+1;i++)InfoPrintf("%02X,",Data[i]);InfoPrintf("\r\n");
				
			if((Data[0] != 0x50)||(Data[2] != 0x00)) continue;		

			if(Data[11]!=REMOTE_KEY_STOP)continue;
			
			if(Data[1] == REMOTE_MARK_V8)  		//����ң������
			{
				IV8_Decrypt(Data + 3);
				
				InfoPrintf("���ܺ�%d+1:",index);
				for(i=0;i<index+1;i++)InfoPrintf("%02X,",Data[i]);InfoPrintf("\r\n");
				
			}
			else if(Data[1] == REMOTE_MARK_V7)  //����ң������
			{
				V7_Decrypt(Data + 3);
			}
			else 
			{
				continue;
			}
			
			if(Data[10] != (Data[11]&0xF0)) 
			{
				InfoPrintf("У�����====0x%02X===0x%02X======\r\n",Data[10],Data[11]);
				//continue;
			}

			if(Data[11]==0x2A)
			{
				//InfoPrintf("ѧϰ��ֵ����!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
				//continue;
			}

			#if 1
			if(double_id_flag==1)
			{
				if(double_id_val==*(u32 *)&Data[3])
				{
					//InfoPrintf("A-\r\n");
				}
				else
				{
					//InfoPrintf("B-\r\n");
					
					continue;
				}
			}

			if(double_id_flag==0)
			{
				double_id_val=*(u32 *)&Data[3];
				double_id_flag=1;
				//InfoPrintf("C-\r\n");
				continue;
			}

			double_id_flag=0;
			#endif
			
			
			//���RFID �ظ�������Դ˰����ݣ�����
			if((IDCount == 1) && (*(u32 *) &Data[3] == NewRemote.Rfid[0]))
			{
				InfoPrintf("RFID�ظ�A =%08X\r\n",*(u32 *) &Data[3]);
				continue;
			}
			
			//���RFID �ظ�������Դ˰����ݣ�����
			if((IDCount == 2) && ( (*(u32 *) &Data[3] == NewRemote.Rfid[0]) || (*(u32 *) &Data[3] == NewRemote.Rfid[1])) ) 
			{
				InfoPrintf("RFID�ظ�B =%08X\r\n",*(u32 *) &Data[3]);
				continue;
			}

			//���RFID �ظ�������Դ˰����ݣ�����
			if(    (IDCount == 3) 
				&& ( 
				        (*(u32 *) &Data[3] == NewRemote.Rfid[0]) 
				     || (*(u32 *) &Data[3] == NewRemote.Rfid[1])
				     || (*(u32 *) &Data[3] == NewRemote.Rfid[2])
				   ) 
		       ) 
			{
				InfoPrintf("RFID�ظ�C =%08X\r\n",*(u32 *) &Data[3]);
				continue;
			}
			
			NewRemote.Rfid[IDCount] = *(u32 *) &Data[3];
			NewRemote.Rfloq[IDCount]= *(u32 *) &Data[7] & 0x00FFFFFF;
			NewRemote.Lfid[IDCount] = ((u16)NewRemote.Rfid[IDCount] ^ 0xA569) & 0xFFFE;

			//OSSchedUnlock(&os_err);
			DriveAlarm(1, 150);
			InfoPrintf("\r\n");
			InfoPrintf("ѧϰ��RFID =%08X\r\n",NewRemote.Rfid[IDCount]);
			InfoPrintf("ѧϰ��Rfloq=%08X\r\n",NewRemote.Rfloq[IDCount]);			
			//InfoPrintf("ѧϰ��LFID =%08X\r\n",NewRemote.Lfid[IDCount]);
			IDCount++;
			InfoPrintf("ѧϰ��RFID����=%d\r\n",IDCount);
			key_sn++;
			InitTime=OSTimeGet(&os_err);
			DelayXms(200);
			//OSSchedLock(&os_err);
		}
		else
		{
			//δ���յ�RF60�·�����Ч�ĸ�Ƶ����
		}

		//��ʱ�ж�
		LastTime = OSTimeGet(&os_err);
		if(key_max_4_flag==1)
		{
			temp=4;	
		}
		else
		{
			temp=2;
		}
		//if((LastTime - InitTime > STUDY_MODE_TIME+(key_sn*60))/*||(READ_STUDYKEY == Bit_RESET)*/) 	//��������ʱ��10s
		if((( (LastTime-InitTime) > 6000) /*||(READ_STUDYKEY == Bit_RESET)*/) 	//��������ʱ��10s
		    ||(key_sn>=temp)
		  )
		{
			InfoPrintf("ѧϰIDʱ���Ѻ���\r\n");
			//OSSchedUnlock(&os_err);
			DriveAlarm(4, 50);	//zgc add
            DelayXms(300);
			//OSSchedLock(&os_err);
			break;
		}

		//��ͣ�ط��͵�Ƶ�źţ�ȷ��PCF7952�ܽ��յ����ذ������7952��Ƶ����ID S
		IWDG_ReloadCounter();	//ι��
		OSSchedLock(&os_err);
		if(SendStudyLF()==1)
		{
			OSSchedUnlock(&os_err);
			DriveAlarm(4, 50);
			break;
		}
		OSSchedUnlock(&os_err);
		DelayXms(5);
		//OSSchedLock(&os_err);
	}
	
	InfoPrintf("ѧϰ��RFID����xxxxxxxxxxxx=%d\r\n",IDCount);
	
	if(IDCount != 0) 	//��Чң����������0
	{ 
		NewRemote.Totle = IDCount;
		NewRemote.Index = 0;
		NewRemote.State = Remote.State; //���Ƶ�ǰ����״̬
		InfoPrintf("ѧϰ��RFID����dccccccccccccccc=%d\r\n",IDCount);
		memcpy(&Remote, &NewRemote, sizeof(REMOTE_STATUS));

		if(FLASH_WriteRemoteID(&Remote) == FALSE) 
		{
			FLASH_WriteRemoteID(&Remote);
		}
		
		FLASH_WriteRemoteLOQ(&Remote);
		FLASH_ReadRemoteID(&NewRemote_cmp);  			//��ȡң����IDֵ��
		FLASH_ReadRemoteLOQ(&NewRemote_cmp); 			//��ȡң����LOQֵ��

		if(memcmp(&NewRemote_cmp,&Remote,sizeof(REMOTE_STATUS))==0)
		{
			InfoPrintf("����ɹ�!!!!!!!!! ң������=%d !!!!!!!!!!!!!!\r\n",IDCount);
		}
		//OSSchedUnlock(&os_err);
		DriveAlarm(IDCount, 500);  //����ң��������������ʾ
		//OSSchedLock(&os_err);
	}

	
    //OSSchedUnlock(&os_err);
	
  	return;
}


static void rf_open_module(void)
{
	//user_uart2_open();
	//gps_power_ctl(1);
	//gps_reset_ctl(0);
	//user_delay_ms(TIME_10ms);
	//gps_config_module();
	//gps_init_ant_ck_pin();
}

static void rf_close_module(void)
{
	
}


#define ___RF____MIDDLE__________________________________________________________


#define ___RF_APP__________________________________________________________________________




static void RfTaskMsg_handle(TASK_MSG_T *p_msg)
{

	switch (p_msg->msg_src_id)
	{
		case DEV_MANAGE_TASK_ID:
			switch (p_msg->msg_father_type)
			{
				case FATHER_MSG_T_TASK_POWER_CTRL:
					if(p_msg->msg_son_type==POWER_CTRL_PWR_ON)
					{
						rf_open_module();
					}
					else if(p_msg->msg_son_type==POWER_CTRL_WAKEUP)
					{

					}
					else if(p_msg->msg_son_type==POWER_CTRL_SLEEP)
					{

					}
					else if(p_msg->msg_son_type==POWER_CTRL_PWR_OFF)
					{

					}
					break;
				default:
					break;
			}
			break;
		case MODEM_MANAGE_TASK_ID:
			break;
		default:
			break;
	}		
}

void init_adc(uint8_t chan)
{
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_Cmd(ADC1, DISABLE);
	
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channels configuration */ 
	ADC_RegularChannelConfig(ADC1, chan, 1, ADC_SampleTime_28Cycles5);

	/* Enable ADC1 EOC interrupt */
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	/*ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);*/

	//ADC_Cmd(ADC1, ENABLE);
}

void start_adc(uint8_t chan)
{
	ADC_RegularChannelConfig(ADC1, chan, 1, ADC_SampleTime_28Cycles5);
	ADC_Cmd(ADC1, ENABLE);
}



void ADC1_2_IRQHandler(void)
{
	uint16_t temp;
	uint32_t all=0;
	uint8_t i;	

	if(1 == ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC))
	{
		bat_current = ADC_GetConversionValue(ADC1);
		if(check_bat_vol_ok_flag==2)
		{
			if((bat_current>bat_off_average+bat_dif_val)&&(bat_cali_ok_flag==1))
			{
				#if 1
				{
					static uint8_t cnt=0;
					if(cnt++%10==0)
					InfoPrintf("��������...\r\n");
				}
				#endif
			}
		}

		if(check_bat_vol_ok_flag==0)
		{
			check_bat_vol_buf[check_bat_vol_buf_offset++]=bat_current;
			if(check_bat_vol_buf_offset>=CHECK_BUF_LEN)
			{
				all=0;
				for(i=0;i<CHECK_BUF_LEN;i++)
				{
					all+=check_bat_vol_buf[i];
				}

				check_bat_vol_buf_offset=0;
				check_bat_vol_ok_flag=1;
				bat_off_average=all/CHECK_BUF_LEN;
				InfoPrintf("У׼����� Ϩ��� ��ѹOK\r\n");
			}
			
			check_bat_vol_buf_offset%=CHECK_BUF_LEN;
		}
		else if(check_bat_vol_ok_flag==1)
		{
			all=0;
			check_bat_vol_buf[check_bat_vol_buf_offset++]=bat_current;
			for(i=0;i<CHECK_BUF_LEN;i++)
			{
				all+=check_bat_vol_buf[i];
			}
			bat_engined_average=all/CHECK_BUF_LEN;
			check_bat_vol_buf_offset%=CHECK_BUF_LEN;
			//InfoPrintf("У׼����� ���� ��ѹOK\r\n");
		}
				
		ADC_Cmd(ADC1, DISABLE);
	}
}

static void recalibrate_bat_vol(void)
{
	check_bat_vol_ok_flag=0;
	check_bat_vol_buf_offset=0;
	bat_cali_ok_flag=0;
}

static void check_study_key(void)
{
	OS_ERR os_err;
	uint32_t temp,j;
	uint32_t i;
	static uint32_t cali_alarm_timestamp=0;
	
	if(READ_STUDYKEY == Bit_RESET)
	{
		temp=0;
		for(i=1;i<150;i++)
		{
			user_delay_ms(10);
			if(READ_STUDYKEY == Bit_SET)
			{
				i=0;
				break;
			}
			IWDG_ReloadCounter();
		}
		
		if(i!=0)
		{
			InfoPrintf("����ѧϰң����ģʽ...\r\n");
			OnStudyRemote();
			user_delay_ms(200);
		}
		#ifdef EN_ADC1_CHN1
		else
		{
			//InfoPrintf("���У׼...\r\n");
			temp=1;
			for(i=0;i<300;i++)
			{
				user_delay_ms(10);
				
				if((READ_STUDYKEY == Bit_RESET) && (temp%2==1))
				{
					user_delay_ms(30);
					temp++;
				}
				else if((READ_STUDYKEY == Bit_SET) && (temp%2==0))
				{
					user_delay_ms(30);
					temp++; 
				}

				if((temp==1)&&(i>120))
				{
					break;
				}

				if(temp>=5)
				{
					InfoPrintf("����У׼ģʽ...\r\n");
					recalibrate_bat_vol();
					start_calibrate_bat_timestamp=OSTimeGet(&os_err);
					break;
				}	
			}
		}
		#endif
	}

	#ifdef EN_ADC1_CHN1
	if(OSTimeGet(&os_err)-start_calibrate_bat_timestamp>10000)
	{
		if(check_bat_vol_ok_flag==1)
		{
			if(bat_engined_average>bat_off_average+100)
			{
				bat_dif_val=bat_engined_average-bat_off_average;
				bat_cali_ok_flag=1;
			}
			else
			{
				bat_cali_ok_flag=0;
				InfoPrintf("У׼����\r\n");
				DriveAlarm(12, 70);
			}
			check_bat_vol_ok_flag=2;
			InfoPrintf("У׼����\r\n");
			cali_alarm_timestamp=0;
		}
	}
	else
	{
		if(check_bat_vol_ok_flag<=1)
		{
			if((OSTimeGet(&os_err)-cali_alarm_timestamp)>=1000)
			{
				cali_alarm_timestamp=OSTimeGet(&os_err);
				DriveAlarm(1, 200);
			}
		}
	}
	#endif
}

void IWDG_Configuration(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	// ��ʹ�ܣ�iwdg�ļĴ�����д����,������д��0x5555�����ܲ����Ĵ���
	IWDG_SetPrescaler(IWDG_Prescaler_256);			// �������Ź�ʹ���ڲ���������LSI����LSI����256��Ƶ
	//IWDG_SetReload(2000);							// �趨�������Ź��������ļ���ֵ(0x000~0xFFF;0~4095)����λʱ��Ϊ12.8s
	//IWDG_SetReload(200);							// �趨�������Ź��������ļ���ֵ(0x000~0xFFF;0~4095)����λʱ��Ϊ1.28s
	IWDG_SetReload(800);							// �趨�������Ź��������ļ���ֵ(0x000~0xFFF;0~4095)����λʱ��Ϊ2.56s
	IWDG_ReloadCounter();							// ���ض������Ź�����������Ĵ���д��0xAAAA�����߸��¼���ֵ
	IWDG_Enable();									// �������Ź�����Ĵ���д��0xCCCC����
}


void RfTask(void *p_arg)
{
	TASK_MSG_T *task_msg;
	OS_MSG_SIZE msg_size;
	uint32_t last_time=0;
	uint32_t last_adc_time=0;
	uint32_t i;
	uint8_t temp;
	OS_ERR os_err;
	p_arg=p_arg;
	
	KEY_PWR_OFF;
	MCURegID = *(vu32 *)(0x1FFFF7E8 + 8);			//��ȡMCU�̼�ID
	factory_trace_id = *(vu32 *)(0x1FFFF7E8 + 0);	//��ȡMCU�̼�ID zgc
	FLASH_ReadRemoteID(&Remote);  					//��ȡң����IDֵ��
	FLASH_ReadRemoteLOQ(&Remote); 					//��ȡң����LOQֵ��

	#ifdef EN_ADC1_CHN1
	init_adc(ADC_Channel_1);
	#else
	DelayXms(5);  
	on_status=READ_ON_LINE;
	#endif
	
	while(1)
	{
		task_msg = OSTaskQPend(0,OS_OPT_PEND_NON_BLOCKING,&msg_size,NULL,&os_err);
		if(NULL != task_msg)
		{
			RfTaskMsg_handle(task_msg);
			user_free(task_msg,__FUNCTION__);
		}
		
		OnRfRev();

		check_study_key();
		
		#ifdef EN_ADC1_CHN1
		if(user_get_os_time()-last_adc_time>T_10MS)
		{
			last_adc_time=user_get_os_time();
			start_adc(ADC_Channel_1);
			
		}
		#else
		if(user_get_os_time()-last_adc_time>T_50MS)
		{
			last_adc_time=user_get_os_time();
			if(READ_ON_LINE!=on_status)
			{
				DelayXms(5);  
				for(i=0;i<10;i++)
				{
					temp=READ_ON_LINE;
					if(temp==on_status)
					{
						break;
					}
					
					if(i==9)
					{
						on_status=temp;
						InfoPrintf("!!!!!!!!!!on_status=%d,\r\n",on_status);
						on_change_flag=1;
						remote_started_flag=0;
						if(on_status==1)
						{
							if(ActiveImmo() == TRUE) 
							{		
								if(smart_card_pwr_status!=1)
								{
									power_on_smart_card(); 
								}
								smart_card_need_dly_pwr_off_flag=0;
								//DriveAlarm(3, 100);
								InfoPrintf("AAAң�����ڳ��ϣ�תΪ �������� ģʽ !!!!\r\n");
								InfoPrintf("\r\n\r\n\r\n");
								remote_started_flag=0;
							}
							else 
							{
								//control_car_power(CAR_OFF);
								DriveAlarm(1, 100);
								DelayXms(1000); 
								InfoPrintf("ң�������ڳ��ϣ�Ϩ��OK\r\n");
							}
						}
					}
					DelayXms(2);
				}
			}
		}
		#endif

		if(READ_BREAK_PIN==1)
		{
			DelayXms(5);
			if((remote_started_flag==1)&&(READ_BREAK_PIN==1))
			{
				if(ActiveImmo() == TRUE) 
				{		
					if(smart_card_pwr_status!=1)
					{
						power_on_smart_card(); 
						smart_card_need_dly_pwr_off_flag=0;
					}
					//DriveAlarm(5, 100);
					InfoPrintf("ң�����ڳ��ϣ�תΪ �������� ģʽ !!!!\r\n");
					InfoPrintf("\r\n\r\n\r\n");
					remote_started_flag=0;
				}
				else 
				{
					control_car_power(CAR_OFF);
					DriveAlarm(1, 100);
					DelayXms(1000); 
					InfoPrintf("ң�������ڳ��ϣ�Ϩ��OK\r\n");
				}
			}
		}
		
		if(smart_card_need_dly_pwr_off_flag==1)
		{
			if(OSTimeGet(&os_err)-remote_pwr_on_card_timestamp>T_3S)
			{
				//DriveAlarm(2, 200);
				power_off_smart_card(); 
			}
		}

		key_release();

		if(user_get_os_time()-last_time>T_600MS)
		{
			if((remote_started_flag==1)||(on_status==0))
			{
				last_time=user_get_os_time();
				//InfoPrintf("bat_OFF_average=%d,bat_engined_average=%d,bat_current=%d\r\n",bat_off_average,bat_engined_average,bat_current);
				SendAwakeLF();
			}
		}


		IWDG_ReloadCounter();
		//DelayXms(15);
		OSSchedRoundRobinYield(&os_err);
	}

}





