#ifdef VIA_OPENCV

#ifndef USRGAMECONTROLLER_H
#define USRGAMECONTROLLER_H

//using namespace cv;
using namespace std;

#include "qtcyberdip.h"
#include "stdafx.h"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <iostream>

//�����趨
//#define x1_origin  96
//#define x2_origin  623
#define x1_origin 23 //26
#define x2_origin 261 //295
#define y1_origin 140 //139
#define y2_origin 379 //408
#define total_x 285 //318
#define total_y 536//544

#define WIN_NAME "Frame"

//��Ϸ������
class usrGameController
{
private://Ϊ��ʵ����ʾЧ�������ӵ�����
	//���ص��ṹ��
	deviceCyberDip* device;
//������
	struct MouseArgs{
		cv::Rect box;
		bool Drawing, Hit;
		// init
		MouseArgs() :Drawing(false), Hit(false)
		{
			box = cv::Rect(0, 0, -1, -1);
		}
	};
	//���ص�����
	friend void  mouseCallback(int event, int x, int y, int flags, void*param);
	MouseArgs argM;
//������Ϊ��ʵ�ֿ�����ʾЧ�������ӵ�����
public:
	//���캯�������б����ĳ�ʼ����Ӧ�ڴ����
	usrGameController(void* qtCD);
	//�������������ձ���������Դ
	~usrGameController();
	//����ͼ������ÿ���յ�ͼ��ʱ�������
	int usrProcessImage(cv::Mat& img);
	//���ʹ�������
	void move_and_work(double x1, double y1, double x2, double y2);
	//��Եɨ��
	void usrGameController::my_marginscan(cv::Mat img,cv:: Mat T, int n);
};

//������Ϊ��ʵ����ʾЧ�������ӵ�����
//���ص�����
void  mouseCallback(int event, int x, int y, int flags, void*param);
//������Ϊ��ʵ�ֿ�����ʾЧ�������ӵ�����

#endif
#endif
