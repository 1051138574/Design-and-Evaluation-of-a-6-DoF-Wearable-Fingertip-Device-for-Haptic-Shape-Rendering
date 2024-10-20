#include "pca9685.h"
#include "myiic.h"
#include "delay.h"
#include "math.h"



void pca_write(u8 adrr,u8 data)//��PCAд����,adrrd��ַ,data����
{ 
	IIC_Start();
	
	IIC_Send_Byte(pca_adrr);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(adrr);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(data);
	IIC_Wait_Ack();
	
	IIC_Stop();
}

u8 pca_read(u8 adrr)//��PCA������
{
	u8 data;
	IIC_Start();
	
	IIC_Send_Byte(pca_adrr);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(adrr);
	IIC_Wait_Ack();
	
	IIC_Start();
	
	IIC_Send_Byte(pca_adrr|0x01);
	IIC_Wait_Ack();
	
	data=IIC_Read_Byte(0);
	IIC_Stop();
	
	return data;
}


void pca_setfreq(float freq)//����PWMƵ��
{
		u8 prescale,oldmode,newmode;
		double prescaleval;
		freq *= 0.92; 
		prescaleval = 25000000;
		prescaleval /= 4096;
		prescaleval /= freq;
		prescaleval -= 1;
		prescale =floor(prescaleval + 0.5f);//freq��������Ҫ��Ƶ�ʣ�prescale��������Ҫ���õ�ֵ

		oldmode = pca_read(pca_mode1);
	
		newmode = (oldmode&0x7F) | 0x10; // sleep
	
		pca_write(pca_mode1, newmode); // go to sleep
	
		pca_write(pca_pre, prescale); // set the prescaler
	
		pca_write(pca_mode1, oldmode);
		delay_ms(2);
	
		pca_write(pca_mode1, oldmode | 0xa1); 
}


/*num:���PWM�������0~15��on:PWM��������ֵ0~4096,off:PWM�½�����ֵ0~4096
һ��PWM���ڷֳ�4096�� 
��0��ʼ+1�������Ƶ�onʱ����Ϊ�ߵ�ƽ������������offʱ������Ϊ�͵�ƽ��ֱ������4096���¿�ʼ��

���Ե� on ������0ʱ������ʱ,��on����0ʱ��off/4096��ֵ����PWM��ռ�ձȡ�*/

//on = 0, off/4096Ϊռ�ձ�
//
//����pwmռ�ձȣ���onΪ0ʱ��0ff / 4096 ��ֵ��Ϊռ�ձ�
void pca_setpwm(u8 num, u32 on, u32 off)
{
		pca_write(LED0_ON_L+4*num,on);
		pca_write(LED0_ON_H+4*num,on>>8);
		pca_write(LED0_OFF_L+4*num,off);
		pca_write(LED0_OFF_H+4*num,off>>8);
}


/*
	�������ã���ʼ�����������
	������1.PWMƵ��
		    2.��ʼ������Ƕ�
  ���ܣ�0-15ͨ��ȫ������Ϊ0�����ö��Ƶ���Լ���ʼ�Ƕ�
*/
void Servo_Init(float hz,u8 angle)
{
	u32 off=0;                    //ռ�ձ�Ϊ0
	IIC_Init();
	pca_write(pca_mode1,0x0);
	pca_setfreq(hz);              //����PWMƵ��
	off=(u32)(103+angle*2.2);
	pca_setpwm(0,0,off);
	pca_setpwm(1,0,off);
	pca_setpwm(2,0,off);
	pca_setpwm(3,0,off);
	pca_setpwm(4,0,off);
	pca_setpwm(5,0,off);
	pca_setpwm(6,0,off);
	pca_setpwm(7,0,off);
	pca_setpwm(8,0,off);
	pca_setpwm(9,0,off);
	pca_setpwm(10,0,off);
	pca_setpwm(11,0,off);
	pca_setpwm(12,0,off);
	pca_setpwm(13,0,off);
	pca_setpwm(14,0,off);
	pca_setpwm(15,0,off);
	delay_ms(500);
}




/*
	�������ã� ���ƶ��ת��
	������     num��pca9685 ����˿�0-15 �� angle���Ƕȣ���������60�������ת��60�ȡ�
  ���ܣ�     ���ö������Ԥ��Ƕ�ת��
 
*/
//void Servo_angle(u8 num,u8 target_angle)
//{u8 current_angle = 0;
//	u32 off = 0;
//	 while (current_angle != target_angle) {
//		 if (current_angle < target_angle) {
//            current_angle++; // �����ӽǶ�
//        } else {
//            current_angle--; // �𲽼��ٽǶ�
//        }
//  off = (u32)(103+current_angle*2.2);
//	pca_setpwm(num,0,off);
//			delay_ms(20);
//}
//	 }

void Servo_angle(u8 num,u8 angle)
{
	u32 off = 0;
  off = (u32)(103+angle*2.2);
	
		pca_setpwm(num,0,off);
	
	
	
//	 u8 target_angle = 0; // Ŀ��Ƕ�
//    static u8 current_angle[16] = {60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60}; // ��ǰ�Ƕ�
//     u8 total_steps = 0; // �ܲ���
//     u8 step_delay = 0; // ������ʱ
//     u32 off_time = 0; // PWM �½�����ֵ
//    // ����Ŀ��Ƕ�
//    target_angle = angle;
//    // �岹����
//    total_steps = abs(target_angle*2.2 - current_angle[num]*2.2); // �ܲ���ΪĿ��ǶȺ͵�ǰ�Ƕ�֮��ľ���ֵ
//        int n=100;// ����岹����
//    u8 dp = 0;
//    if (target_angle > current_angle[num])// ����岹����
//        dp = total_steps/n;
//    else if (target_angle < current_angle[num])
//        dp = -total_steps/n;
//off_time = (u32)(103 + current_angle[num] * 2.2);
//    // ��ʼ�岹�˶�
//    for (int step_count = 0; step_count < n; step_count++) { 
////       if(abs(off_time-(u16)(103 + target_angle * 2.2))<2)
////				 {
////				 off_time = (u16)(103 + target_angle * 2.2);
////				 pca_setpwm(num, 0, off_time);
////			    }
////			 else
//				   // �����½�ʱ��
//			   off_time +=dp;
//         pca_setpwm(num, 0, off_time); // ���¶�� PWM     
//			delay_ms(2);
//    }
//		current_angle[num]=target_angle;
		
		

}
