#include "usrGameController.h"
#include "stdlib.h"
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include <iostream>  
#include <stdio.h> 
#include <math.h>

using namespace cv;
using namespace std;

Point MatchingMethod(int, void*, Mat temp, Mat origin);
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
	qDebug() << "usrGameController offline.";
}

//处理图像

static int judge[64] = { 0 };
static int flag = 0;
static int step = 0,realtime = 0,judge_flag =0,num;
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
		number_y = 358;//375		
		num = digitRecongnition(number_y, pt);
		cout <<"num=" <<num<<endl;
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
			int n = sqrt(num);
			cout << n << endl;
			int nxny[2][8] = { 0 };
			for (int i = 0;i < n;i++)
			{
				nxny[0][i] = x1_origin + (2 * i + 1)*(x2_origin - x1_origin) / (2 * n);
				nxny[1][i] = y1_origin + (2 * i + 1)*(y2_origin - y1_origin) / (2 * n);
			}


			//读取原图截图
			Mat rgbd = imread("realtime.jpg", CV_8UC1);


			//Mat rgbd = imread("origin_liu.jpg",1);
			//vector<cv::Mat> bgrrgbd;
			//split(rgbd, bgrrgbd);
			//cv::Mat blue_rgbd = bgrrgbd.at(0);
			//cv::Mat green_rgbd = bgrrgbd.at(1);
			//cv::Mat red_rgbd = bgrrgbd.at(2);

		/*********************************************************************************************************/
			//cout << int(flag / 5) << endl;
			if (int(flag / 5) > 0)
			{
				flag = 0;
				//my_marginscan(pt, rgbd, n);
			}
			flag++;

			//计算中间区域长&宽
			int x_length, y_length;
			x_length = x2_origin - x1_origin;
			y_length = y2_origin - y1_origin;

			//模板匹配
			Mat temp_origin;
			Point position;

			for (int i = 0;i < n;i++)
			{
				for (int j = 0;j < n;j++)
				{
					int num = i*n + j;
					if (n > 5)
					{
						temp_origin = rgbd(Rect(int(nxny[0][j] - x_length / (2 * n)), int(nxny[1][i] - y_length / (2 * n)), int(x_length / n), int(y_length / n)));
					}
					else
					{
						temp_origin = rgbd(Rect(int(nxny[0][j] - x_length / (4 * n)), int(nxny[1][i] - y_length / (4 * n)), int(x_length / 2 / n), int(y_length / 2 / n)));
					}		
					imshow("原图", temp_origin);
					//position = MatchingMethod(0, 0, redImg, temp_origin);
					if (!judge[num]) position = MatchingMethod(0, 0, pt, temp_origin);
					else continue;
					if (abs(position.x + temp_origin.cols / 2 - nxny[0][j]) + abs(position.y + temp_origin.rows / 2 - nxny[1][i]) <= 1)
					{
						judge[num] = 1;
						continue;
					}
					//cout << position.x + temp_origin.cols / 2 << endl << position.y + temp_origin.rows / 2 << endl<< nxny[0][j]<<endl<< nxny[1][i]<<endl<<endl;
					//将匹配的拼图块结果移至相应位置
					move_and_work(position.x + temp_origin.cols / 2, position.y + temp_origin.rows / 2, nxny[0][j], nxny[1][i]);
					waitKey(2000);
				}

			}



			//ORB
		/*
			Ptr<ORB> orb = ORB::create(2000);//创建ORB
			vector<KeyPoint> Keypoints_pt, Keypoints_T;//定义特征点向量
			Mat descriptors_pt, descriptors_T;
			orb->detectAndCompute(pt, Mat(), Keypoints_pt, descriptors_pt);//检测特征点
			orb->detectAndCompute(rgbd, Mat(), Keypoints_T, descriptors_T);

			//Matching
			vector<DMatch> matches;//定义匹配器
			BFMatcher matcher;
			matcher.match(descriptors_pt, descriptors_T, matches);


			//保存之前BFMatcher匹配的点的坐标
			std::vector<Point2f> match_points_pt;
			std::vector<Point2f> match_points_T;

			for (size_t i = 0;i<matches.size();i++)
			{
				match_points_pt.push_back(Keypoints_pt[matches[i].queryIdx].pt);
				match_points_T.push_back(Keypoints_T[matches[i].trainIdx].pt);
			}
			vector<uchar>inliersMask(match_points_pt.size());
			Mat H = findHomography(match_points_T, match_points_pt, CV_FM_RANSAC, 3.0, inliersMask, 2000);
			//3.0 容错阈值，当某一个匹配与估计的假设小于阈值时，则被认为是一个内点
			//2000 迭代次数  不影响响应时间，每次迭代都会改变迭代总次数，最终58
			//cout << "原匹配点数为：" << matches.size() << endl;

			vector<DMatch>inliers;
			for (size_t i = 0;i<inliersMask.size();i++) {
				if (inliersMask[i])
					inliers.push_back(matches[i]);
			}
			matches.swap(inliers);
			cout << "good_matches数为：" << matches.size() << endl;

			bool flag = false;
			int matches_pt_x, matches_pt_y, matches_T_x, matches_T_y;
			int matches_dist_min = 10000, disttmp=0;
			for (int i = 0; i < matches.size(); i++)
			{
				matches_pt_x = Keypoints_pt[matches[i].queryIdx].pt.x;
				matches_pt_y = Keypoints_pt[matches[i].queryIdx].pt.y;
				matches_T_x = Keypoints_T[matches[i].trainIdx].pt.x;
				matches_T_y = Keypoints_T[matches[i].trainIdx].pt.y;

				if ((matches_pt_y < y1_origin) || (matches_pt_y > y2_origin))
				{
					for (int j = 0; j < n; j++)
					//{
						for (int k = 0;k < n;k++)
						{
							disttmp = (matches_T_x - nxny[0][j]) * (matches_T_x - nxny[0][j]) + (matches_T_y - nxny[1][k]) * (matches_T_y - nxny[1][k]);
							if (disttmp < matches_dist_min) matches_dist_min = disttmp;
						}
					//}
					if (matches_dist_min < (x2_origin - x1_origin)*(x2_origin - x1_origin) / (10 * n*n) + (y2_origin - y1_origin)*(y2_origin - y1_origin) / (10 * n*n))
					{
						move_and_work(matches_pt_x, matches_pt_y, matches_T_x, matches_T_y);
						flag = true;
						//cout << "(" << matches_pt_x << "," << matches_pt_y << ")" << endl << "(" << matches_T_x << "," << matches_T_y << ")" << endl << endl;
					}
				}
				if(flag) break;
			}
		*/

			int to_the_end = 0;
			for (int i = 0;i < n*n;i++)
			{
				to_the_end += judge[i];
				cout << judge[i]<<" ";
			}
			//cout << endl;
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
	//判断鼠标点击尺寸
	if (argM.box.x >= 0 && argM.box.x < imgSize.width&&
		argM.box.y >= 0 && argM.box.y < imgSize.height
		)
	{
		qDebug() << "X:" << argM.box.x << " Y:" << argM.box.y;
		if (argM.Hit)
		{
			device->comHitDown();
		}
		device->comMoveToScale(((double)argM.box.x + argM.box.width) / pt.cols, ((double)argM.box.y + argM.box.height) / pt.rows);
		argM.box.x = -1; argM.box.y = -1;
		if (argM.Hit)
		{
			device->comHitUp();
		}
		else
		{
			device->comHitOnce();
		}
	}
	return 0;
}

int minof3(int x1, int x2, int x3)
{
	int maxout = x1;
	//if (x1 < maxout) maxout = x1;
	if (x2 < maxout) maxout = x2;
	if (x3 < maxout) maxout = x3;
	return maxout;
}

//模板匹配函数
Point MatchingMethod(int, void*, Mat img, Mat T)
{
//T.convertTo(T, CV_32FC4, 1 / 255.0);
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

minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

matchLoc = minLoc;

rectangle(img_display, matchLoc, Point(matchLoc.x + T.cols, matchLoc.y + T.rows), Scalar::all(0), 2, 8, 0);
//rectangle(result, matchLoc, Point(matchLoc.x + T.cols, matchLoc.y + T.rows), Scalar::all(0), 2, 8, 0);
imshow("Origin Image", img_display);
//imshow("Temp window", result);
//cout << result;
return matchLoc;
}

//触笔工作函数
void usrGameController::move_and_work(double x1, double y1, double x2, double y2)
{
	//int p;
	//cin >> p;
	//if (p == 0) {
		double real_x1, real_y1, real_x2, real_y2;
		real_x1 = x1 * RANGE_X / total_x;
		real_x2 = x2 * RANGE_X / total_x;
		real_y1 = y1 * RANGE_Y / total_y;
		real_y2 = y2 * RANGE_Y / total_y;

		device->comMoveTo(real_x1, real_y1);
		waitKey(100);
		device->comHitDown();
		waitKey(100);
		device->comMoveTo(real_x2, real_y2);
		waitKey(1000);
		device->comHitUp();
	//}
	//cout << "(" << x1 << "," << y1 << ")" << endl << "(" << x2 << "," << y2 << ")" << endl << endl;
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
	int result = 0;
	int x = 11;//fsq13
	int x_n = 25;//fsq27
	p = MatchingMethod(0, 0, diImg, T);
	cout << p;

	if (p == Point(x, number_y)) result = 4;
	if (p == Point(x + x_n * 1, number_y)) result = 9;
	if (p == Point(x + x_n * 2, number_y)) result = 16;
	if (p == Point(x + x_n * 3, number_y)) result = 25;
	if (p == Point(x + x_n * 4, number_y)) result = 36;
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
