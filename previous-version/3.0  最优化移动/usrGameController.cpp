#include "usrGameController.h"
#include "stdlib.h"
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include <iostream>  
#include <stdio.h> 
#include <math.h>

using namespace cv;
using namespace std;

//匹配元素结构体
struct match
{
	double min;
	Point position;
};

match MatchingMethod(int, void*, Mat temp, Mat origin);
int digitRecongnition(int number_y, cv::Mat diImg);

#ifdef VIA_OPENCV
//构造与初始化
usrGameController::usrGameController(void* qtCD)
{
	qDebug() << "usrGameController online.";
	device = new deviceCyberDip(qtCD);//设备代理类
	cv::namedWindow(WIN_NAME);
	cv::setMouseCallback(WIN_NAME, mouseCallback, (void*)&(argM));
}

//析构
usrGameController::~usrGameController()
{
	cv::destroyAllWindows();
	if (device != nullptr)
	{
		delete device;
	}
	qDebug() << "usrGameController offline.fucking brotherwei";
}

//处理图像 

static int judge[64] = { 0 };
static int flag = 0;
static int step = 0,realtime = 0,judge_flag =0,number;
static Mat TP;
int usrGameController::usrProcessImage(cv::Mat& img)
{
	cv::Size imgSize(img.cols, int(img.rows*(1- 1.6/16.2)));
	//cv::Size imgSize(img.cols, int(img.rows));
	//cout << img.rows;
	if (imgSize.height <= 0 || imgSize.width <= 0)
	{
		qDebug() << "Invalid image. Size:" << imgSize.width <<"x"<<imgSize.height;
		return -1;
	}
	//截取图像边缘、获取实时图像并转为灰度图
	cv::Mat pt1 = img(cv::Rect(10, int(img.rows * 16 / 162), int(imgSize.width - 20), int(imgSize.height - 40)));
	//cv::Mat pt1 = img(cv::Rect(0,50, int(imgSize.width), int(imgSize.height - 50)));
	
	//vector<cv::Mat> bgrImg;
	//split(pt1, bgrImg);
	//cv::Mat blueImg = bgrImg.at(0);
	//cv::Mat greenImg = bgrImg.at(1);
	//cv::Mat redImg = bgrImg.at(2);


	Mat pt;
	cvtColor(pt1, pt, CV_BGR2GRAY);
	//pt1.convertTo(pt, CV_32FC4, 1 / 255.0);
	cv::imshow(WIN_NAME, pt);
	

	//保存拼图
	//cv::imwrite("origin_pad_img.jpg", pt1);
	//exit(0);

	//move_and_work(26, 139, 295, 408);
	//exit(0);



	//识别块数num
	if (step == 0)
	{
		int number_y;
		number_y = 357;//375		
		number = digitRecongnition(number_y, pt);
		cout <<"number=" <<number<<endl;
		step = 1;
	}

	//点击开始
	if (step == 1)
	{
		//cv::waitKey(0);
		device->comMoveTo(165 * RANGE_X / total_x,460 * RANGE_Y / total_y);
		//device->comMoveTo(278 * RANGE_X / total_x, 505 * RANGE_Y / total_y);//for pad
		device->comHitDown();
		waitKey(1500);
		device->comHitUp();
		waitKey(1000);		
		
	}
	//截图

	if (step == 2)
	{
		if (realtime == 0)
		{
			TP = pt.clone();
			//imshow("截图", TP);
			imwrite("realtime.jpg", TP);
			realtime = 1;
			waitKey(1000);
		}

		if (judge_flag)
		{
			//中心点计算
			//int n = 4;
			int n = sqrt(number);
			cout << n << endl;
			int nxny[2][8] = { 0 };
			for (int i = 0;i < n;i++)
			{
				nxny[0][i] = x1_origin + (2 * i + 1)*(x2_origin - x1_origin) / (2 * n);
				nxny[1][i] = y1_origin + (2 * i + 1)*(y2_origin - y1_origin) / (2 * n);
			}


			//读取原图截图
			//Mat rgbd = imread("realtime.jpg", CV_8UC1);
			Mat rgbd = imread("full1.jpg", CV_8UC1);

			//Mat rgbd = imread("origin_liu.jpg",1);
			//vector<cv::Mat> bgrrgbd;
			//split(rgbd, bgrrgbd);
			//cv::Mat blue_rgbd = bgrrgbd.at(0);
			//cv::Mat green_rgbd = bgrrgbd.at(1);
			//cv::Mat red_rgbd = bgrrgbd.at(2);

		/*********************************************************************************************************/
			//cout << int(flag / 10) << endl;
			if (int(flag / 10) > 0)
			{
				flag = 0;
				my_marginscan(pt, rgbd, n);
			}
			flag++;

			//计算中间区域长&宽
			int x_length, y_length;
			x_length = x2_origin - x1_origin;
			y_length = y2_origin - y1_origin;

			//模板匹配
			Mat temp_origin;
			Point position,good_position;
			match n_match[64];
			int BMatchNum[3] = {-1};

			for (int i = 0;i < n;i++)
			{
				for (int j = 0;j < n;j++)
				{
					int num = i*n + j;
					//不同大小截取小框边长不同
					if (n > 5)
					{
						temp_origin = rgbd(Rect(int(nxny[0][j] - x_length / (2 * n)), int(nxny[1][i] - y_length / (2 * n)), int(x_length / n), int(y_length / n)));
					}
					else
					{
						temp_origin = rgbd(Rect(int(nxny[0][j] - x_length / (4 * n)), int(nxny[1][i] - y_length / (4 * n)), int(x_length / 2 / n), int(y_length / 2 / n)));
					}		
					cv::imshow("原图", temp_origin);
					//position = MatchingMethod(0, 0, redImg, temp_origin);

					//填充数组对应值为0，即还未拼好时才会进行下面操作
					if (!judge[num]) n_match[num] = MatchingMethod(0, 0, pt, temp_origin);
					else continue;

					position = n_match[num].position;
					if (abs(position.x + temp_origin.cols / 2 - nxny[0][j]) + abs(position.y + temp_origin.rows / 2 - nxny[1][i]) <= 3)
					{
						judge[num] = 1;
					}
					//cout << position.x + temp_origin.cols / 2 << endl << position.y + temp_origin.rows / 2 << endl<< nxny[0][j]<<endl<< nxny[1][i]<<endl<<endl;
					
				}
			}
			//找到方差最小的一次匹配，认为该匹配正确率最大
			double tmptmp = 0;
			bool first_round = true;
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
				{
					int num = i*n + j;
					if (judge[num] == 1) continue;
					if (first_round || n_match[num].min < tmptmp)
					{
						tmptmp = n_match[num].min;
						BMatchNum[0] = num;
						BMatchNum[1] = i;
						BMatchNum[2] = j;
						first_round = false;
					}
					else continue;
				}
			}
			cout << "i="<<BMatchNum[1] << "j=" << BMatchNum[2] << "num=" << BMatchNum[0] << endl;
			//对于上面最好的一次匹配，做出相应移动
			good_position = n_match[BMatchNum[0]].position;
			if (n > 5)
			{
				temp_origin = rgbd(Rect(int(nxny[0][BMatchNum[2]] - x_length / (2 * n)), int(nxny[1][BMatchNum[1]] - y_length / (2 * n)),
					int(x_length / n), int(y_length / n)));
			}
			else
			{
				temp_origin = rgbd(Rect(int(nxny[0][BMatchNum[2]] - x_length / (4 * n)), int(nxny[1][BMatchNum[1]] - y_length / (4 * n)),
					int(x_length / 2 / n), int(y_length / 2 / n)));
			}
			move_and_work(good_position.x + temp_origin.cols / 2, good_position.y + temp_origin.rows / 2, nxny[0][BMatchNum[2]], nxny[1][BMatchNum[1]]);
			waitKey(2000);

			//判断是否结束
			int to_the_end = 0;
			for (int i = 0;i < n*n;i++)
			{
				to_the_end += judge[i];
				cout << judge[i]<<" ";
			}
			if (to_the_end == n*n)
			{
				device->comMoveTo(0, 0);
				exit(0);
				//cout << "end" << to_the_end<<endl;
			}

		}
		judge_flag = 1;
	}
	step = 2;

	return 0;
}

//模板匹配函数
match MatchingMethod(int, void*, Mat img, Mat T)
{

Mat img_display;

img.copyTo(img_display);
//cout << "img" << img.cols << "\r" << img.rows << endl;
//cout << "T" << T.cols << "\r" << T.rows << endl;

int result_cols = img.cols - T.cols + 1;
int result_rows = img.rows - T.rows + 1;
Mat result=Mat(result_cols, result_rows, CV_32FC4);
//cout << img.channels() << " " << T.channels() << " " << result.channels() << endl;

matchTemplate(img, T, result, CV_TM_SQDIFF);

//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

double minVal;
double maxVal;
Point minLoc;
Point maxLoc;
Point matchLoc;

cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

matchLoc = minLoc;

cv::rectangle(img_display, matchLoc, Point(matchLoc.x + T.cols, matchLoc.y + T.rows), Scalar::all(0), 2, 8, 0);
//rectangle(result, matchLoc, Point(matchLoc.x + T.cols, matchLoc.y + T.rows), Scalar::all(0), 2, 8, 0);
cv::imshow("Origin Image", img_display);
//imshow("Temp window", result);
//cout << result;
match MT = { minVal,matchLoc };
return (MT);
}

//触笔工作函数
void usrGameController::move_and_work(double x1, double y1, double x2, double y2)
{
		double real_x1, real_y1, real_x2, real_y2;
		//硬件保护
		if (x1 < 0) x1 = 0; if (x1 > total_x) x1 = total_x;
		if (x2 < 0) x2 = 0; if (x2 > total_x) x2 = total_x;
		if (y1 < 0) y1 = 0; if (y1 > total_y) y1 = total_y;
		if (y2 < 0) y2 = 0; if (y2 > total_y) y2 = total_y;
		//像素坐标转化为机器坐标
		real_x1 = x1 * RANGE_X / total_x;
		real_x2 = x2 * RANGE_X / total_x;
		real_y1 = y1 * RANGE_Y / total_y;
		real_y2 = y2 * RANGE_Y / total_y;
		//机器移动
		device->comMoveTo(real_x1, real_y1);
		waitKey(100);
		device->comHitDown();
		waitKey(100);
		device->comMoveTo(real_x2, real_y2);
		waitKey(1000);
		device->comHitUp();
	
}

//边缘扫描
void usrGameController::my_marginscan(Mat img, Mat T, int n)
{
	int buchang = total_x / n - 1;
	int variance = 0;
	double maxV = 0;
	int max_store = 0;
	int lowbound = (x2_origin - x1_origin) / (2 * n) - 1;
	//上面
	for (int i = 0;i < n;i++)
	{
		Mat xiaokuang_img = img(Rect((0 + buchang*i), 0, buchang, lowbound));
		Mat xiaokuang_T = T(Rect((0 + buchang*i), 0, buchang, lowbound));	
		for (int ii = 0;ii < lowbound;ii++)
			for (int jj = 0;jj < buchang;jj++)
			{
				uchar *data_img = xiaokuang_img.ptr<uchar>(ii);
				uchar *data_T = xiaokuang_T.ptr<uchar>(ii);
				variance += (data_img[jj] - data_T[jj])*(data_img[jj] - data_T[jj]);
			}

		if (i == 0) { maxV = variance; max_store = 0; }
		else
		{
			if (variance > maxV) { maxV = variance;max_store = i; }
		}
	}
	if (variance > 10000) move_and_work((max_store + 0.5)*buchang, lowbound / 2, total_x / 2, 1.3*lowbound);
	//下面
	for (int i = 0;i < n;i++)
	{
		Mat xiaokuang_img = img(Rect((0 + buchang*i), (total_y - lowbound), buchang, lowbound));
		Mat xiaokuang_T = T(Rect((0 + buchang*i), (total_y - lowbound), buchang, lowbound));
		for (int ii = 0;ii < lowbound;ii++)
			for (int jj = 0;jj < buchang;jj++)
			{
				uchar *data_img = xiaokuang_img.ptr<uchar>(ii);
				uchar *data_T = xiaokuang_T.ptr<uchar>(ii);
				variance += (data_img[jj] - data_T[jj])*(data_img[jj] - data_T[jj]);
			}

		if (i == 0) { maxV = variance; max_store = 0; }
		else
		{
			if (variance > maxV) { maxV = variance;max_store = i; }
		}
	}
	if (variance > 10000) move_and_work((max_store + 0.5)*buchang, (total_y - lowbound / 2), total_x / 2, (total_y - 1.3*lowbound));
}


//数字识别
int digitRecongnition(int number_y, cv::Mat diImg) {
	Mat T = imread("滑块.jpg", CV_8UC1);
	Point p;
	match MT;
	int result = 0;
	int x = 11;//fsq13
	int x_n = 25;//fsq27
	MT = MatchingMethod(0, 0, diImg, T);
	p = MT.position;
	cout << p;

	if (p == Point(x, number_y)) result = 4;
	if (p == Point(x + x_n * 1, number_y)) result = 9;
	if (p == Point(60, number_y)) result = 16;
	if (p == Point(85, number_y)) result = 25;
	if (p == Point(110, number_y)) result = 36;
	if (p == Point(x + x_n * 5, number_y)) result = 49;
	if (p == Point(x + x_n * 6, number_y)) result = 64;

	return result;
}

//鼠标回调函数
void mouseCallback(int event, int x, int y, int flags, void*param)
{
	usrGameController::MouseArgs* m_arg = (usrGameController::MouseArgs*)param;
	switch (event)
	{
	case CV_EVENT_MOUSEMOVE: // 鼠标移动时
	{
		if (m_arg->Drawing)
		{
			m_arg->box.width = x - m_arg->box.x;
			m_arg->box.height = y - m_arg->box.y;
		}
	}
	break;
	case CV_EVENT_LBUTTONDOWN:case CV_EVENT_RBUTTONDOWN: // 左/右键按下
	{
		m_arg->Hit = event == CV_EVENT_RBUTTONDOWN;
		m_arg->Drawing = true;
		m_arg->box = cvRect(x, y, 0, 0);
		//尝试
		//m_arg->box = cvRect(0, 0, 0, 0);
	}
	break;
	case CV_EVENT_LBUTTONUP:case CV_EVENT_RBUTTONUP: // 左/右键弹起
	{
		m_arg->Hit = false;
		m_arg->Drawing = false;
		if (m_arg->box.width < 0)
		{
			m_arg->box.x += m_arg->box.width;
			m_arg->box.width *= -1;
		}
		if (m_arg->box.height < 0)
		{
			m_arg->box.y += m_arg->box.height;
			m_arg->box.height *= -1;
		}
	}
	break;
	}
}
#endif
