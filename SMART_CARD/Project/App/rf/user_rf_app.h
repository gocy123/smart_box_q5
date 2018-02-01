#ifndef _USER_RF_APP_H
#define _USER_RF_APP_H

#include <stdint.h>

#if 1
#include "stm32f10x_gpio.h"

//#define EN_ADC1_CHN1


#define GPIO_RX_RX                    GPIOA
#define GPIO_PIN_RX_RX                GPIO_Pin_0  //SI4355 RX       ��������4

#define GPIO_ON_DET                   GPIOA
#define GPIO_PIN_ON_DETECT            GPIO_Pin_1  //ON�ߵ�ƽ���    20180102 
#define GPIO_PIN_BAT_CHECK            GPIO_Pin_1  //����ص�ѹ���  20180102 

#define GPIO_RX_SCK 				  GPIOA
#define GPIO_PIN_RX_SCK 			  GPIO_Pin_4  //SI4355 SCK		�������2
#define GPIO_RX_SEL 				  GPIOA
#define GPIO_PIN_RX_SEL 			  GPIO_Pin_5  //SI4355Ƭѡ		�������2
#define GPIO_RX_MOSI				  GPIOA
#define GPIO_PIN_RX_MOSI			  GPIO_Pin_6  //SI4355 MOSI 	�������2
#define GPIO_RX_MISO				  GPIOA
#define GPIO_PIN_RX_MISO			  GPIO_Pin_7  //SI4355 MISO 	��������1

#define GPIO_CHECK_BREAK			  GPIOA 
#define GPIO_PIN_CHECK_BREAK		  GPIO_Pin_15  //ɲ�����

#define GPIO_START_KEY                GPIOB
#define GPIO_PIN_START_KEY            GPIO_Pin_0  //����Զ��һ������������          �������2

#define GPIO_BREAK					  GPIOB
#define GPIO_PIN_BREAK_OUT			  GPIO_Pin_1  //BREAK��

#define GPIO_ACC_DETECT				  GPIOB
#define GPIO_PIN_ACC_DETECT			  GPIO_Pin_4  //			�������2


#define GPIO_BEEP					  GPIOB
#define GPIO_PIN_BEEP				  GPIO_Pin_5  //������			�������2

#define GPIO_125K                     GPIOB
#define GPIO_PIN_125KL                GPIO_Pin_6  //125K            �����������3
#define GPIO_PIN_125KR                GPIO_Pin_7  //125K            �����������3	
#define LF125K_TIMx                   TIM4
#define LF125KR_Channelx              TIM_Channel_1
#define LF125KL_Channelx              TIM_Channel_2


#define GPIO_STUDYKEY				GPIOB
#define GPIO_PIN_STUDYKEY			GPIO_Pin_8  //ѧϰ����		��������1
#define GPIO_KEY_PWR				GPIOB
#define GPIO_PIN_KEY_PWR			GPIO_Pin_9  //ԭ��ң������Դ����			�������2
#define GPIO_KEY_UNLOCK  			GPIOB
#define KEY_PIN_UNLOCK 				GPIO_Pin_12
#define GPIO_KEY_LOCK  				GPIOB
#define KEY_PIN_LOCK 				GPIO_Pin_13
#define GPIO_KEY_TRUNK  			GPIOB
#define KEY_PIN_TRUNK 				GPIO_Pin_14
#define GPIO_KEY_ALARM  			GPIOB
#define KEY_PIN_ALARM 				GPIO_Pin_15



///////////////////////////////////////////////////////////////////////////////////////////////
#define CAR_START_KEY_ON          	GPIO_SetBits(  GPIO_START_KEY, GPIO_PIN_START_KEY)
#define CAR_START_KEY_OFF         	GPIO_ResetBits(GPIO_START_KEY, GPIO_PIN_START_KEY)
#define KEY_PWR_ON          		GPIO_ResetBits(GPIO_KEY_PWR, GPIO_PIN_KEY_PWR)
#define KEY_PWR_OFF         		GPIO_SetBits(  GPIO_KEY_PWR, GPIO_PIN_KEY_PWR)
#define KEY_UNLOCK_ON          		GPIO_SetBits(  GPIO_KEY_UNLOCK, KEY_PIN_UNLOCK)
#define KEY_UNLOCK_OFF         		GPIO_ResetBits(GPIO_KEY_UNLOCK, KEY_PIN_UNLOCK)
#define KEY_LOCK_ON          		GPIO_SetBits(  GPIO_KEY_LOCK, KEY_PIN_LOCK)
#define KEY_LOCK_OFF         		GPIO_ResetBits(GPIO_KEY_LOCK, KEY_PIN_LOCK)
#define KEY_TRUNK_ON          		GPIO_SetBits(  GPIO_KEY_TRUNK, KEY_PIN_TRUNK)
#define KEY_TRUNK_OFF         		GPIO_ResetBits(GPIO_KEY_TRUNK, KEY_PIN_TRUNK)
#define KEY_ALARM_ON          		GPIO_SetBits(  GPIO_KEY_ALARM, KEY_PIN_ALARM)
#define KEY_ALARM_OFF         		GPIO_ResetBits(GPIO_KEY_ALARM, KEY_PIN_ALARM)


#define READ_UHF_IN                   	GPIO_ReadInputDataBit(GPIO_RX_RX, GPIO_PIN_RX_RX)
#define READ_ON_LINE                   	GPIO_ReadInputDataBit(GPIO_ON_DET, GPIO_PIN_ON_DETECT)
#define READ_STUDYKEY                 	GPIO_ReadInputDataBit(GPIO_STUDYKEY, GPIO_PIN_STUDYKEY)
#define READ_BREAK_PIN                	GPIO_ReadInputDataBit(GPIO_CHECK_BREAK, GPIO_PIN_CHECK_BREAK)



#define BREAK_ON						GPIO_SetBits(  GPIO_BREAK, GPIO_PIN_BREAK_OUT)
#define BREAK_OFF   	 				GPIO_ResetBits(GPIO_BREAK, GPIO_PIN_BREAK_OUT)
#define BEEP_ON                       	GPIO_SetBits(  GPIO_BEEP, GPIO_PIN_BEEP)
#define BEEP_OFF                      	GPIO_ResetBits(GPIO_BEEP, GPIO_PIN_BEEP)



#define KEY_ERROR				(0x5EE0)
#define KEY_NOP               	(0x0000)
#define KEY_LOCK              	(0x002A)
#define KEY_UNLOCK            	(0x001C)
#define KEY_BOX               	(0x0046)

#define KEY_ALARM             	(0x008E)
#define REMOTE_KEY_START      	(0x007F)
#define REMOTE_KEY_STOP      	(0x0049)

#define KEY_LONG              	(0x0100)
#define KEY_HAND              	(0x0200)
#define KEY_LOCK_L            	(KEY_LOCK  | KEY_LONG)
#define KEY_UNLOCK_L          	(KEY_UNLOCK| KEY_LONG)
#define KEY_BOX_L             	(KEY_BOX   | KEY_LONG)
#define KEY_ALARM_L           	(KEY_ALARM | KEY_LONG)
#define KEY_UNLOCK_HAND       	(KEY_UNLOCK| KEY_HAND)  

#define LF_DISABLE_TIME       	(16*60*60*62L)  //ϵͳ����͹���ģʽ��ʱ�䣺62Сʱ
#define AUTO_LOCK_TIME        	(16*60*2)       //�Զ�����2����
#define WAIT_UPWIN_LOCK_TIME  	(16*8)/*(16*8)*/          //����������8sʹ�����������
#define REMOTE_COUNTER_TIME   	(16*6)          //ң������-���6s����������3��   
#define FIND_ALARM_TIME       	(16)            //ÿ1s����һ��Ѱ������
#define REMOTESTART_OFF_TIME  	(16*60*20)      //ң����������ʱ��20����
//#define STUDY_MODE_TIME       	(16*6*2)          //ѧϰ����ʱ��6s*2
#define STUDY_MODE_TIME       	(16*13)          
#define STUDY_TIME            	(48)//(16*3)          //ѧϰ��������3S
#define START_TIME            	(2)             //������������125ms
#define SHOCK_TIME            	(2)             //��������125ms


#endif

#define U8_MAX     ((u8)255)
#define S8_MAX     ((s8)127)
#define S8_MIN     ((s8)-128)
#define U16_MAX    ((u16)65535u)
#define S16_MAX    ((s16)32767)
#define S16_MIN    ((s16)-32768)
#define U32_MAX    ((u32)4294967295uL)
#define S32_MAX    ((s32)2147483647)
#define S32_MIN    ((s32)-2147483648)
typedef enum {FALSE = 0, TRUE = !FALSE} bool;

typedef struct{
  uint8_t Crcid;     //1Byte              |
  uint8_t Totle;     //1Byte              |
  uint16_t Lfid[4];  //8Byte              | ==>30�ֽ�
  uint32_t Rfid[4];  //16Byte             |
  uint32_t Gsmid;    //4 =======>30Byte   | 

  //����16���ֽڣ��ڱ��浽FLASH��ʱ�򣬻��crcloqȡָ�룬����ָ�����������4����
  uint8_t Crcloq;    //1Byte              | 
  uint8_t Index;     //1Byte              |
  uint8_t Ecar;      //1Byte              | ==>20�ֽ� 
  uint8_t State;     //1Byte              |
  uint32_t Rfloq[4]; //16 =18Byte         |
  
}REMOTE_STATUS; // 48Byte

typedef enum {
	CAR_OFF = 0, 
	CAR_ACC = 1, 
	CAR_ON = 2, 
	CAR_RUN = 3
}CAR_POWER_STATUS;  //OFF����ACC����ON����START��


extern void 	RfTask(void * p_arg);
extern void IWDG_Configuration(void);



#endif

