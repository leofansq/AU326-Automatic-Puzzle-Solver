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

//参数设定
//#define x1_origin  96
//#define x2_origin  623
#define x1_origin 23 //26
#define x2_origin 261 //295
#define y1_origin 140 //139
#define y2_origin 379 //408
#define total_x 285 //318
#define total_y 536//544

#define WIN_NAME "Frame"

//游戏控制类
class usrGameController
{
private://为了实现演示效果，增加的内容
	//鼠标回调结构体
	deviceCyberDip* device;
//以下是
	struct MouseArgs{
		cv::Rect box;
		bool Drawing, Hit;
		// init
		MouseArgs() :Drawing(false), Hit(false)
		{
			box = cv::Rect(0, 0, -1, -1);
		}
	};
	//鼠标回调函数
	friend void  mouseCallback(int event, int x, int y, int flags, void*param);
	MouseArgs argM;
//以上是为了实现课堂演示效果，增加的内容
public:
	//构造函数，所有变量的初始化都应在此完成
	usrGameController(void* qtCD);
	//析构函数，回收本类所有资源
	~usrGameController();
	//处理图像函数，每次收到图像时都会调用
	int usrProcessImage(cv::Mat& img);
	//触笔工作函数
	void move_and_work(double x1, double y1, double x2, double y2);
	//边缘扫描
	void usrGameController::my_marginscan(cv::Mat img,cv:: Mat T, int n);
};

//以下是为了实现演示效果，增加的内容
//鼠标回调函数
void  mouseCallback(int event, int x, int y, int flags, void*param);
//以上是为了实现课堂演示效果，增加的内容

#endif
#endif
