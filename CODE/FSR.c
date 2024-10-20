#include "stm32f10x.h"
#include "FSR.h"
#include "sys.h" 
#include "delay.h"
#include "usart.h"
#include "serial.h"				
#include "control.h"
//������ʼ������
void FSR_IO_Init(void) //IO��ʼ��
{ 	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 //Ĭ������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC
	
	GPIO_SetBits(GPIOC,GPIO_Pin_15);					//��ʼ������Ϊ0
	
  EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
u8 state=0;
extern double* x;
extern  double* xx;
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line14) == SET)
	{
		/*��������������������󣬿��ٴ��ж����ŵ�ƽ���Ա��ⶶ��*/
	if(FSR_Scan(1) == 1 && state == 0)
		{  xx[11] -= 1;
				x=inv(xx[3],xx[7],xx[11],xx[15],xx[19],xx[23]);
					  servocontrol11(x);
			free(x);
				state = 1;
		}
		
		if(FSR_Scan(1) == 0 && state == 1)
		{
				//printf("���ڷ�ֵ\r\n");
			Serial_SendString("contact\r\n");
				state = 0;
		}
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
}
u8 FSR_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&FSR_GPIO==1)
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(FSR_GPIO==1)return KEY_PRESS;
	}else if(FSR_GPIO==0)key_up=1; 	    
 	return 0;// �ް�������
}
