#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "usart.h"
#include <string.h>
#include <stdlib.h>
#define BUFFER_SIZE 64 // ����Ϊ�ʵ��Ļ�������С

uint8_t rxBuffer[BUFFER_SIZE];
uint16_t rxIndex = 0;
uint16_t rxLength = 0;
uint8_t Serial_RxData;
uint8_t Serial_RxFlag;
uint8_t receivedData[4]; // ���ڴ洢���յ��ֽ�����
volatile uint8_t rxBufferIndex = 0; // ���ջ���������
uint8_t Serial_TxPacket[4];				//FF 01 02 03 04 FE
uint8_t Serial_RxPacket[4];
void Serial_Init(void)
{	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}


void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}
uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;
}

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		Serial_RxData = USART_ReceiveData(USART1);
		Serial_RxFlag = 1;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

//void Serial_GetArray(uint8_t *Array, uint16_t Length)
//{
//	uint16_t i;
//	uint16_t GetArray[Length];
//	for (i = 0; i < Length; i ++)
//	{
//		GetArray[i]=Serial_GetRxData();
//	}
//}
//void USART12_IRQHandler(void)
//{
//	static uint8_t RxState = 0;
//	static uint8_t pRxPacket = 0;
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
//	{
//		uint8_t RxData = USART_ReceiveData(USART1);
//		
//		if (RxState == 0)
//		{
//			if (RxData == 0xFF)
//			{
//				RxState = 1;
//				pRxPacket = 0;
//			}
//		}
//		else if (RxState == 1)
//		{
//			Serial_RxPacket[pRxPacket] = RxData;
//			pRxPacket ++;
//			if (pRxPacket >= 4)
//			{
//				RxState = 2;
//			}
//		}
//		else if (RxState == 2)
//		{
//			if (RxData == 0xFE)
//			{
//				RxState = 0;
//				Serial_RxFlag = 1;
//			}
//		}
//		
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//	}
//}

//uint8_t Serial_GetRxData1(void)
//{
//	return *receivedData;
//}

//void USART12_IRQHandler(void)
//{
//    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
//    {   
//        // ��USART1�������ݲ��洢��receivedData����
//        receivedData[rxBufferIndex] = USART_ReceiveData(USART1);
//        Serial_RxFlag = 1;
//        // ���ӻ���������
//        rxBufferIndex++;

//        // ����Ѿ����յ�4���ֽڣ����Խ��д���
//        if (rxBufferIndex == 4)
//        {
//            // �����ﴦ����յ������ݣ����罫�ֽ����黹ԭΪ����

//            // ������󣬿�������������׼��������һ������
//            rxBufferIndex = 0;
//        }
//    }
//}


//void USART1_IRQHandler(void)
//{
//    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
//    {
//        uint8_t receivedByte = USART_ReceiveData(USART1);
//        
//        if (rxIndex == 0)
//        {
//            if (receivedByte == 0x55) // ��ͷ��ʶ
//            {
//                rxBuffer[rxIndex] = receivedByte;
//                rxIndex++;
//            }
//        }
//        else if (rxIndex == 1)
//        {
//            rxLength = receivedByte; // ���ݰ�����
//            rxBuffer[rxIndex] = receivedByte;
//            rxIndex++;
//        }
//        else
//        {
//            rxBuffer[rxIndex] = receivedByte;
//            rxIndex++;
//            
//            if (rxIndex == (rxLength + 2)) // ������ͷ������
//            {
//                // �������ݰ�������ϣ����Խ��д���
//                // �����rxBuffer �а��������������ݰ�
//                
//                // ���������ݺ����ý���״̬
//                rxIndex = 0;
//                rxLength = 0;
//            }
//        }

//        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//    }
//}
//void Read_Usart(char rx,float *arrays)
//{
//	static char shuzu[3][8]={0};		//����3�����ݣ��ɸ��ģ���ÿ�����ݰ���С����͸��ſ�����8λ��ʮ���ƣ�  ���͵��ַ�(������.����)�������<8
//	static int begin=0,x=0,y=0;		//��Ч���ݱ�־����ά����Ԫ�ر���
//	int num;
//	if(rx=='[')		//��ʼ��־
//	{
//		begin=1;
//		x=0;
//		y=0;
//	}
//	else if(rx==']')	//������־ �ڽ���ʱ�ַ�ת��Ϊʵ��
//	{
//		begin=0;
//		for(num=0;num<=x;num++)
//		{
//			arrays[num]=(float)atof(shuzu[num]);//��Ҫ���ͷ�ļ�<stdlib.h>
//		}
//		/* ��ӡ���� */
//		for(num=0;num<=x;num++)
//		{
//			printf("%d : %f\r\n",num+1,arrays[num]);
//		}
//		printf("\r\n");
//		memset(shuzu,0,sizeof(shuzu));	//������飬��ʼ��Ϊ0
//	}
//	else if(rx==',')
//	{
//		x++;
//		y=0;
//	}
//	else if(begin==1)
//	{
//		shuzu[x][y]=rx;
//		y++;
//	}	
//}
