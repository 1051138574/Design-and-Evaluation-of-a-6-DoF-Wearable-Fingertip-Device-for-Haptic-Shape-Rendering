#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "pca9685.h"
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>
#include "inverse.h"
#include "math.h"
#include "serial.h"
#include "MPU6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "FSR.h"
#include "control.h"
#define NUM_SERVOS 6
#define INCREMENT 1.0  // ÿ�θ��µĽǶ�����
#define TOLERANCE 1
#define DELAY_MS 20  // �ӳ�ʱ��

//��ַ�����	

double t=0; 
int i = 0;
	
u8 state = 0;	
	
extern  double *x;
double aa[6];
uint8_t RxData;
double val1 = 180 / 3.1415926;
int i1=0;
		int i2=0;
		double xx[24];
		uint8_t  yy[4];
    int  t1;
		double y1;
		int t2;
		double current_angles[NUM_SERVOS] = {60, 60, 60, 60, 60, 60};  // ��ʼ��Ϊ��ǰ�Ƕ�
 int servo_pins[6] = {4, 7, 8, 11, 12, 15};			
	static u8 current_angle[6] = {0,0,0,0,0,0};	








	
	
		
void print11(double *x){
	for(int i=0;i<6;i++)
	{ aa[i]=x[i];
		Serial_SendString("\r\nAngel is:");
		Serial_SendNumber(aa[i],2);
		delay_ms(2000);
	}
};




int main(void)
{	
float pitch,roll,yaw;
	delay_init();//��ʱ������ʼ��
	Servo_Init(50,60);//��ʼ���������
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	delay_ms(1000);
	//uart_init(9600);
	Serial_Init();

	
	//NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	FSR_IO_Init();
	//	printf("Test start\r\n");

//	MPU_Init();		
//while(mpu_dmp_init())
//	{
//	delay_ms(20);
//	}	





	while(1)
		{

		 

			
	
     if (Serial_GetRxFlag() == 1 )
				 {

					 t1=Serial_GetRxData();
					 xx[t2]=(double)t1;
					 t2++;
					 if(t2==24)
					 {
						 t2=0;
						x=inv(xx[3],xx[7],xx[11],xx[15],xx[19],xx[23]);
					  servocontrol11(x);
						free(x);
				   }
			 }		
			
			
			
	
			
 }
				}
				
				
				
				


		