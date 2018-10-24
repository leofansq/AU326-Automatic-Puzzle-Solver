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
static int flag = 0,flag1 = 0;
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
	cv::imwrite("36origin.jpg", pt1);
	exit(0);

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
			//cout << n << endl;
			int nxny[2][8] = { 0 };
			for (int i = 0;i < n;i++)
			{
				nxny[0][i] = x1_origin + (2 * i + 1)*(x2_origin - x1_origin) / (2 * n);
				nxny[1][i] = y1_origin + (2 * i + 1)*(y2_origin - y1_origin) / (2 * n);
			}


			//读取原图截图
			Mat rgbd = imread("realtime.jpg", CV_8UC1);
			//Mat rgbd = imread("full1.jpg", CV_8UC1);

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
				my_marginscan_td(pt, rgbd, n);
				my_marginscan_lr(pt, rgbd, n);
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
			Mat sijiao1, sijiao2, sijiao3, sijiao4;//四角
			match sijiao_match[4];
			int BMatchNum[3] = {-1};

			//对于n=6及以上情况，拼图块不会自动吸附，改变策略，先拼四周再来中间
			if (n >= 6)
			{
				//先处理四角
				if ((judge[0] == 0) || (judge[n - 1] == 0) || (judge[n*(n - 1)] == 0) || (judge[n*n - 1] == 0))
				{
					sijiao1 = rgbd(Rect(int(nxny[0][0] - x_length / (2 * n)), int(nxny[1][0] - y_length / (2 * n)), int(x_length / n), int(y_length / n)));
					sijiao2 = rgbd(Rect(int(nxny[0][n - 1] - x_length / (2 * n)), int(nxny[1][0] - y_length / (2 * n)), int(x_length / n), int(y_length / n)));
					sijiao3 = rgbd(Rect(int(nxny[0][0] - x_length / (2 * n)), int(nxny[1][n - 1] - y_length / (2 * n)), int(x_length / n), int(y_length / n)));
					sijiao4 = rgbd(Rect(int(nxny[0][n - 1] - x_length / (2 * n)), int(nxny[1][n - 1] - y_length / (2 * n)), int(x_length / n), int(y_length / n)));

					if (!judge[0]) sijiao_match[0] = MatchingMethod(0, 0, pt, sijiao1);
					if (abs(sijiao_match[0].position.x + temp_origin.cols / 2 - nxny[0][0]) + abs(sijiao_match[0].position.y + temp_origin.rows / 2 - nxny[1][0]) <= 3)
						judge[0] = 1;

					if (!judge[n - 1]) sijiao_match[1] = MatchingMethod(0, 0, pt, sijiao2);
					if (abs(sijiao_match[1].position.x + temp_origin.cols / 2 - nxny[0][0]) + abs(sijiao_match[1].position.y + temp_origin.rows / 2 - nxny[1][0]) <= 3)
						judge[n - 1] = 1;

					if (!judge[n*(n - 1)])sijiao_match[2] = MatchingMethod(0, 0, pt, sijiao3);
					if (abs(sijiao_match[2].position.x + temp_origin.cols / 2 - nxny[0][0]) + abs(sijiao_match[2].position.y + temp_origin.rows / 2 - nxny[1][0]) <= 3)
						judge[n*(n - 1)] = 1;

					if (!judge[n*n - 1]) sijiao_match[3] = MatchingMethod(0, 0, pt, sijiao4);
					if (abs(sijiao_match[3].position.x + temp_origin.cols / 2 - nxny[0][0]) + abs(sijiao_match[3].position.y + temp_origin.rows / 2 - nxny[1][0]) <= 3)
						judge[n*n - 1] = 1;

					double tmptmp = 0;
					bool sijiao_round = true;
					int move;
					for (int i = 0; i < 4; i++)
					{
						int num;
						if (i == 0) num = 0;
						if (i == 1) num = n - 1;
						if (i == 2) num = n*(n - 1);
						if (i == 3) num = n*n - 1;

						if (judge[num]) continue;
						if (sijiao_round || sijiao_match[i].min < tmptmp)
						{
							tmptmp = sijiao_match[i].min;
							move = i;
							sijiao_round = false;
						}
						else continue;
					}

				
					if (move == 0) move_and_work(sijiao_match[0].position.x + sijiao1.cols / 2, sijiao_match[0].position.y + sijiao1.rows / 2, nxny[0][0], nxny[1][0]);
					if (move == 1) move_and_work(sijiao_match[1].position.x + sijiao1.cols / 2, sijiao_match[1].position.y + sijiao1.rows / 2, nxny[0][n - 1], nxny[1][0]);
					if (move == 2) move_and_work(sijiao_match[2].position.x + sijiao1.cols / 2, sijiao_match[2].position.y + sijiao1.rows / 2, nxny[0][0], nxny[1][n - 1]);
					if (move == 3) move_and_work(sijiao_match[3].position.x + sijiao1.cols / 2, sijiao_match[3].position.y + sijiao1.rows / 2, nxny[0][n - 1], nxny[1][n - 1]);
				}

				//接下来一圈一圈处理,最外圈移4n-4，次外圈移4（n-2）-4，……
				//8*8，依次为28*2=56、20*1.5=30、12*1=12、4*0.5=2，7*7，依次为24*2=48、16*1.5=24、8*1=8、1,6*6，依次为20*2=40、12*1.5=18-3=15、4*0.5=2,
				/*if (n == 8) 
				{
					for (int i = 0; i < 100; i++)
					{
						if (i < 56) roundmove(n, 0, pt, rgbd);
						if((i>=56)&&(i<86)) roundmove(n, 1, pt, rgbd);
						if ((i >= 86) && (i<98)) roundmove(n, 2, pt, rgbd);
						if ((i >= 98) && (i<100)) roundmove(n, 3, pt, rgbd);
					}
				}
				if (n == 7)
				{
					for (int i = 0; i < 81; i++)
					{
						if (i < 48) roundmove(n, 0, pt, rgbd);
						if ((i >= 48) && (i<72)) roundmove(n, 1, pt, rgbd);
						if ((i >= 72) && (i<80)) roundmove(n, 2, pt, rgbd);
						if ((i >= 80) && (i<81)) roundmove(n, 3, pt, rgbd);
					}
				}*/
				if (n == 6)
				{
					if(int(flag1 / 57) == 0)
					{
						if (flag1 < 40) roundmove(n, 0, pt, rgbd);
						if ((flag1 >= 40) && (flag1<55)) roundmove(n, 1, pt, rgbd);
						if ((flag1 >= 55) && (flag1<57)) roundmove(n, 2, pt, rgbd);
					}
					else flag1 = 0;
				}
				flag1++;
			}

			//n小于6
			else
			{
				for (int i = 0; i < n; i++)
				{
					for (int j = 0; j < n; j++)
					{
						int num = i*n + j;
					
						temp_origin = rgbd(Rect(int(nxny[0][j] - x_length / (4 * n)), int(nxny[1][i] - y_length / (4 * n)), int(x_length / 2 / n), int(y_length / 2 / n)));

						//填充数组对应值为0，即还未拼好时才会进行下面操作
						if (!judge[num]) n_match[num] = MatchingMethod(0, 0, pt, temp_origin);
						else continue;

						position = n_match[num].position;
						if (abs(position.x + temp_origin.cols / 2 - nxny[0][j]) + abs(position.y + temp_origin.rows / 2 - nxny[1][i]) <= 3)
						{
							judge[num] = 1;
						}
						
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
				cout << "i=" << BMatchNum[1] << "j=" << BMatchNum[2] << "num=" << BMatchNum[0] << endl;
				//对于上面最好的一次匹配，做出相应移动
				good_position = n_match[BMatchNum[0]].position;
				
					temp_origin = rgbd(Rect(int(nxny[0][BMatchNum[2]] - x_length / (4 * n)), int(nxny[1][BMatchNum[1]] - y_length / (4 * n)),
						int(x_length / 2 / n), int(y_length / 2 / n)));

				move_and_work(good_position.x + temp_origin.cols / 2, good_position.y + temp_origin.rows / 2, nxny[0][BMatchNum[2]], nxny[1][BMatchNum[1]]);
				waitKey(2000);
			}

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
		//补偿硬件误差
		double dist_x, dist_y;
		dist_x = x2 - x1;
		dist_y = y2 - y1;
		double gamma = 1.1;
		x2 = x1 + dist_x*gamma;
		y2 = y1 + dist_y*gamma;
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

//上下边缘扫描
void usrGameController::my_marginscan_td(Mat img, Mat T, int n)
{
	int buchang = total_x / n - 1;
	int variance = 0;
	double maxV = 0;
	int max_store = 0;
	int lowbound = (y2_origin - y1_origin) / (2 * n) - 1;
	//上面
	for (int i = 0; i < n; i++)
	{
		variance = 0;
		Mat xiaokuang_img = img(Rect((0 + buchang*i), 0, buchang, lowbound));
		Mat xiaokuang_T = T(Rect((0 + buchang*i), 0, buchang, lowbound));
		for (int ii = 0; ii < lowbound; ii++)
			for (int jj = 0; jj < buchang; jj++)
			{
				uchar *data_img = xiaokuang_img.ptr<uchar>(ii);
				uchar *data_T = xiaokuang_T.ptr<uchar>(ii);
				variance += (data_img[jj] - data_T[jj])*(data_img[jj] - data_T[jj]);
			}

		if (i == 0) { maxV = variance; max_store = 0; }
		else
		{
			if (variance > maxV) { maxV = variance; max_store = i; }
		}
	}
	if (variance > 10000) move_and_work((max_store + 0.5)*buchang, lowbound / 2, total_x / 2, 3 * lowbound);
	//下面
	for (int i = 0; i < n; i++)
	{
		variance = 0;
		Mat xiaokuang_img = img(Rect((0 + buchang*i), (total_y - lowbound - 1), buchang, lowbound - 1));
		Mat xiaokuang_T = T(Rect((0 + buchang*i), (total_y - lowbound - 1), buchang, lowbound - 1));
		for (int ii = 0; ii < lowbound - 1; ii++)
			for (int jj = 0; jj < buchang; jj++)
			{
				uchar *data_img = xiaokuang_img.ptr<uchar>(ii);
				uchar *data_T = xiaokuang_T.ptr<uchar>(ii);
				variance += (data_img[jj] - data_T[jj])*(data_img[jj] - data_T[jj]);
			}

		if (i == 0) { maxV = variance; max_store = 0; }
		else
		{
			if (variance > maxV) { maxV = variance; max_store = i; }
		}
	}
	if (variance > 10000) move_and_work((max_store + 0.5)*buchang, (total_y - lowbound / 2), total_x / 2, (total_y - 3 * lowbound));
}

//左右边缘扫描
void usrGameController::my_marginscan_lr(Mat img, Mat T, int n)
{
	int buchang1 = y1_origin / 3;
	int buchang2 = (total_y - y2_origin) / 3;
	int variance = 0;
	double maxV = 0;
	int max_store = 0;
	int lowbound = (x2_origin - x1_origin) / (2 * n) - 1;
	//左上面
	for (int i = 0; i < 3; i++)
	{
		variance = 0;
		Mat xiaokuang_img = img(Rect(0, (0 + buchang1*i), lowbound, buchang1));
		Mat xiaokuang_T = T(Rect(0, (0 + buchang1*i), lowbound, buchang1));
		for (int ii = 0; ii < buchang1; ii++)
			for (int jj = 0; jj < lowbound; jj++)
			{
				uchar *data_img = xiaokuang_img.ptr<uchar>(ii);
				uchar *data_T = xiaokuang_T.ptr<uchar>(ii);
				variance += (data_img[jj] - data_T[jj])*(data_img[jj] - data_T[jj]);
			}

		if (i == 0) { maxV = variance; max_store = 0; }
		else
		{
			if (variance > maxV) { maxV = variance; max_store = i; }
		}
	}
	if (variance > 10000) move_and_work(lowbound / 2, (max_store + 0.5)*buchang1, total_x / 2, 3 * lowbound);
	//左下面
	for (int i = 0; i < 3; i++)
	{
		variance = 0;
		Mat xiaokuang_img = img(Rect(0, (y2_origin + buchang2*i), lowbound, buchang2 - 1));
		Mat xiaokuang_T = T(Rect(0, (y2_origin + buchang2*i), lowbound, buchang2 - 1));
		for (int ii = 0; ii < lowbound - 1; ii++)
			for (int jj = 0; jj < buchang2; jj++)
			{
				uchar *data_img = xiaokuang_img.ptr<uchar>(ii);
				uchar *data_T = xiaokuang_T.ptr<uchar>(ii);
				variance += (data_img[jj] - data_T[jj])*(data_img[jj] - data_T[jj]);
			}

		if (i == 0) { maxV = variance; max_store = 0; }
		else
		{
			if (variance > maxV) { maxV = variance; max_store = i; }
		}
	}
	if (variance > 10000) move_and_work(lowbound / 2, y2_origin + (max_store + 0.5)*buchang2, total_x / 2, (total_y - 3 * lowbound));

	//右上面
	for (int i = 0; i < 3; i++)
	{
		variance = 0;
		Mat xiaokuang_img = img(Rect(total_x - lowbound - 1, (0 + buchang1*i), lowbound, buchang1));
		Mat xiaokuang_T = T(Rect(total_x - lowbound - 1, (0 + buchang1*i), lowbound, buchang1));
		for (int ii = 0; ii < buchang1; ii++)
			for (int jj = 0; jj < lowbound; jj++)
			{
				uchar *data_img = xiaokuang_img.ptr<uchar>(ii);
				uchar *data_T = xiaokuang_T.ptr<uchar>(ii);
				variance += (data_img[jj] - data_T[jj])*(data_img[jj] - data_T[jj]);
			}

		if (i == 0) { maxV = variance; max_store = 0; }
		else
		{
			if (variance > maxV) { maxV = variance; max_store = i; }
		}
	}
	if (variance > 10000) move_and_work(total_x - (lowbound / 2), (max_store + 0.5)*buchang1, total_x / 2, 3 * lowbound);
	//右下面
	for (int i = 0; i < 3; i++)
	{
		variance = 0;
		Mat xiaokuang_img = img(Rect(total_x - lowbound - 1, (y2_origin + buchang2*i), lowbound, buchang2 - 1));
		Mat xiaokuang_T = T(Rect(total_x - lowbound - 1, (y2_origin + buchang2*i), lowbound, buchang2 - 1));
		for (int ii = 0; ii < lowbound - 1; ii++)
			for (int jj = 0; jj < buchang2; jj++)
			{
				uchar *data_img = xiaokuang_img.ptr<uchar>(ii);
				uchar *data_T = xiaokuang_T.ptr<uchar>(ii);
				variance += (data_img[jj] - data_T[jj])*(data_img[jj] - data_T[jj]);
			}

		if (i == 0) { maxV = variance; max_store = 0; }
		else
		{
			if (variance > maxV) { maxV = variance; max_store = i; }
		}
	}
	if (variance > 10000) move_and_work(total_x - (lowbound / 2), y2_origin + (max_store + 0.5)*buchang2, total_x / 2, (total_y - 3 * lowbound));
}

//数字识别
int digitRecongnition(int number_y, cv::Mat diImg) 
{
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
	if (p == Point(x + x_n * 2, number_y)) result = 16;
	if (p == Point(x + x_n * 3, number_y)) result = 25;
	if (p == Point(110, number_y)) result = 36;
	if (p == Point(x + x_n * 5, number_y)) result = 49;
	if (p == Point(x + x_n * 6, number_y)) result = 64;

	return result;
}

//一圈的移动
void usrGameController::roundmove(int n,int x,Mat pt,Mat rgbd)//n表示n*n块数，（x+1）表示第几圈
{
	int nxny[2][8] = { 0 };
	for (int i = 0; i < n; i++)
	{
		nxny[0][i] = x1_origin + (2 * i + 1)*(x2_origin - x1_origin) / (2 * n);
		nxny[1][i] = y1_origin + (2 * i + 1)*(y2_origin - y1_origin) / (2 * n);
	}
	//计算中间区域长&宽
	int x_length, y_length;
	x_length = x2_origin - x1_origin;
	y_length = y2_origin - y1_origin;

	Mat temp_origin;
	Point position, good_position;
	match n_match[64];
	
	for (int i = x; i < n-x; i++)//行数
		{
			for (int j = x; j < n-x; j++)//列数
			{
				int num1 = i*n + j;
				
				//if ((num == 0) || (num == n - 1) || (num == n*(n - 1)) || (num = n*n - 1)) continue;//排除四角
				if ((i != x) && (i != n-x-1) && (j != x) && (j != n-x-1)) continue;//排除该圈内部

				temp_origin = rgbd(Rect(int(nxny[0][j] - x_length / (2 * n)), int(nxny[1][i] - y_length / (2 * n)), int(x_length / n), int(y_length / n)));
				
				//填充数组对应值为0，即还未拼好时才会进行下面操作
				if (!judge[num1]) n_match[num1] = MatchingMethod(0, 0, pt, temp_origin);
				else continue;

				position = n_match[num1].position;
				if (abs(position.x + temp_origin.cols / 2 - nxny[0][j]) + abs(position.y + temp_origin.rows / 2 - nxny[1][i]) <= 3)
					judge[num1] = 1;
			}
		}

		//找到方差最小的一次匹配，认为该匹配正确率最大
		double tmptmp = 0;
		bool first_round1 = true;
		int BMatchNum[3] = { -1 };
		for (int i = x; i < n-x; i++)
		{
			for (int j = x; j < n-x; j++)
			{
				int num1 = i*n + j;
				if (judge[num1] == 1) continue;
				//if ((num == 0) || (num == n - 1) || (num == n*(n - 1)) || (num = n*n - 1)) continue;//排除四角
				if ((i != x) && (i != n - x - 1) && (j != x) && (j != n - x - 1)) continue;//排除该圈内部
				//修饰最小方差
				double myminV = n_match[num1].min;
				if (x == 1)
				{
					//四个角
					if (num1 == 0)
					{
						if (judge[num1 + 1] == 1) myminV = 0.5*myminV;
						if (judge[num1 + n] == 1) myminV = 0.5*myminV;
					}
					if (num1 == n-1)
					{
						if (judge[num1 - 1] == 1) myminV = 0.5*myminV;
						if (judge[num1 + n] == 1) myminV = 0.5*myminV;
					}
					if (num1 == n*(n-1))
					{
						if (judge[num1 + 1] == 1) myminV = 0.5*myminV;
						if (judge[num1 - n] == 1) myminV = 0.5*myminV;
					}
					if (num1 == n*n-1)
					{
						if (judge[num1 - 1] == 1) myminV = 0.5*myminV;
						if (judge[num1 - n] == 1) myminV = 0.5*myminV;
					}
					//上下
					if(((num1>0)&&(num1<n-1))|| ((num1>n*(n-1)) && (num1<n*n-1)))
					{
						if (judge[num1 - 1] == 1) myminV = 0.5*myminV;
						if (judge[num1 + 1] == 1) myminV = 0.5*myminV;
					}
					//左右
					if ((num1>n-1) && (num1<n*n-1))
					{
						if (judge[num1 - 1] == 1) myminV = 0.5*myminV;
						if (judge[num1 + 1] == 1) myminV = 0.5*myminV;
					}
				}
				else
				{
					if (judge[num1 - n] == 1) myminV = 0.5*myminV;
					if (judge[num1 - 1] == 1) myminV = 0.5*myminV;
					if (judge[num1 + 1] == 1) myminV = 0.5*myminV;
					if (judge[num1 + n] == 1) myminV = 0.5*myminV;
				}
				if (first_round1 || (myminV < tmptmp))
				{
					tmptmp = myminV;
					BMatchNum[0] = num1;
					BMatchNum[1] = i;
					BMatchNum[2] = j;
					first_round1 = false;
				}
				else continue;
			}
		}
		cout << "i=" << BMatchNum[1] << "j=" << BMatchNum[2] << "num1=" << BMatchNum[0] << endl;
		//对于上面最好的一次匹配，做出相应移动
		good_position = n_match[BMatchNum[0]].position;
		
		temp_origin = rgbd(Rect(int(nxny[0][BMatchNum[2]] - x_length / (2 * n)), int(nxny[1][BMatchNum[1]] - y_length / (2 * n)),
				int(x_length / n), int(y_length / n)));
		if (!BMatchNum[0])
		{
			move_and_work(good_position.x + temp_origin.cols / 2, good_position.y + temp_origin.rows / 2, nxny[0][BMatchNum[2]], nxny[1][BMatchNum[1]]);
			waitKey(2000);
		}
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
