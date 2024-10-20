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
#define MAX_SOLUTIONS 10

double *inv(double x1, double y, double z, double roll, double  pitch, double  yaw)//��������ƽ���� ������ŷ���� ���x ����y ƫ��z
{double val1 = 180 / 3.1415926;
	double val = 3.1415926 / 180;
	double l[6];// �������ⳤ�ȳ���
	double Rb = 33;//33
	double Rp = 25;//18
	double h0;//ƽ̨��ʼ�߶�
	double beta[6] = { -60,120,60,-120,180,0 };//���������ƽ̨�ļн�
	 double* alpha = malloc(6 * sizeof(double));//�����ת���ĽǶ�
	double  p[6][3];//��ƽ̨����
	double  b[6][3];//��ƽ̨����
	double  q[6][3];//��ƽ̨��Ի�����ϵ������
	double L[6];
	double M[6];
	double N[6];//���������������
	double Rpb[3][3] = {
			cos(yaw * val) * cos(pitch * val), cos(yaw * val) * sin(pitch * val) * sin(roll * val) - sin(yaw * val) * cos(roll * val), cos(yaw * val) * sin(pitch * val) * cos(roll * val) + sin(yaw * val) * sin(roll * val),
			sin(yaw * val) * cos(pitch * val), sin(yaw * val) * sin(pitch * val) * sin(roll * val) + cos(yaw * val) * cos(roll * val), sin(yaw * val) * sin(pitch * val) * cos(roll * val) - cos(yaw * val) * sin(roll * val),
			-sin(pitch * val), cos(pitch * val) * sin(roll * val), cos(pitch * val) * cos(roll * val)
	};//��ת����

double *x;
	 double min_distance = 1e10; // ��ʼ��Ϊһ���ܴ��ֵ
    double *closest_alpha = malloc(6 * sizeof(double)); // �洢����Ľ�
    double all_alpha[MAX_SOLUTIONS][6]; // ��ά���飬ÿ�б�ʾһ����̬�Ķ����
int num_solutions; // ��ǰ��̬�Ľ������
	double weights[6] = {3.0, 3.0, 3.0, 1.0, 1.0, 1.0}; // ����ƽ�����ɶȺ���ת���ɶȵ�Ȩ��
	double gama_b[6] = { 18,42,138,161,-101,-78 };
	double gama_p[6] = { 7,53,127,173,-113,-67 };
	//��������ƽ̨����
	for (int i = 0; i < 6; i++)
	{ 
		b[i][0] = Rb * cos(gama_b[i] * val);
		b[i][1] = Rb * sin(gama_b[i] * val);
		b[i][2] = 0;
		
		p[i][0] = Rp * cos(gama_p[i] * val);
		p[i][1] = Rp * sin(gama_p[i] * val);
		p[i][2] = 0;
	}

	//h0 = sqrt(s * s + a * a - (p[1][0] - b[1][0]) * (p[1][0] - b[1][0]) - (p[1][1] - b[1][1]) * (p[1][1] - b[1][1])) - p[1][2];
	h0=40;//40
	double T[3] = { x1,y,z-h0 };
	//����p����ڻ�����ϵ������q
	for (int i = 0; i < 6; i++)
	{
		q[i][0] = T[0] + Rpb[0][0] * p[i][0] + Rpb[0][1] * p[i][1] + Rpb[0][2] * p[i][2];
		q[i][1] = T[1] + Rpb[1][0] * p[i][0] + Rpb[1][1] * p[i][1] + Rpb[1][2] * p[i][2];
		q[i][2] = T[2] + Rpb[2][0] * p[i][0] + Rpb[2][1] * p[i][1] + Rpb[2][2] * p[i][2];
	}
	//��������˳�l
	
	for (int i = 0; i < 6; i++)
	{
		l[i] = sqrt((q[i][0] - b[i][0]) * (q[i][0] - b[i][0]) + (q[i][1] - b[i][1]) * (q[i][1] - b[i][1]) + (q[i][2] - b[i][2]) * (q[i][2] - b[i][2]));
		
		if (l[i]>47.84)
		{
			l[i]=47;
		}
		
		if (l[i]<29.47 )
		{
			l[i]=30;
		}
	}
	
	//��������Ƕ�
	for (int k = 0; k < MAX_SOLUTIONS; k++) { // ��������ж����
        for (int i = 0; i < 6; i++) {
            L[i] = l[i] * l[i] - s * s + a * a;
            M[i] = 2 * a * (q[i][2] - b[i][2]);
            N[i] = 2 * a * (cos(beta[i] * val) * (q[i][0] - b[i][0]) + 
                             sin(beta[i] * val) * (q[i][1] - b[i][1]));
            // �������˶�ѧ��ʽ����Ƕ�
            all_alpha[k][i] = (int)((asin(L[i] / sqrt((M[i] * M[i]) + (N[i] * N[i]))) - 
                                atan(N[i] / M[i])) * val1);
        }
        num_solutions++; // ���ӽ������
    }

	
 // ������֮ǰ��������
    for (int k = 0; k < num_solutions; k++) { // ����ÿ�����ܵĽ�
        double total_distance = 0;

       for (int i = 0; i < 6; i++) {
        double diff = all_alpha[k][i] - x[i]; // �����ֵ
        total_distance += diff * diff; // ��ֵ��ƽ��
    }

    total_distance = sqrt(total_distance); // ��ƽ����ȡƽ�������õ�ŷ����þ���

        // �ж��Ƿ���������
        if (total_distance < min_distance) {
            min_distance = total_distance; // ������С����
            for (int j = 0; j < 6; j++) {
                closest_alpha[j] = all_alpha[k][j]; // ���������
            }
        }
    }

  
    // ������Ҫ���� closest_alpha ��������������Ľ��
    return closest_alpha; // ȷ�����������
		// �ͷ��ڴ�
    free(closest_alpha);
}
