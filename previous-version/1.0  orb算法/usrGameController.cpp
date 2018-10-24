#include "usrGameController.h"
#include "stdlib.h"
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include <iostream>  
#include <stdio.h> 

using namespace cv;
using namespace std;
/*
//Mat result;
char* image_window = "Origin Image";
char* result_window = "Temp window";
int match_method = CV_TM_SQDIFF;
int max_Trackbar = 5;
void MatchingMethod(int, void*, Mat temp, Mat origin);*/

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
int usrGameController::usrProcessImage(cv::Mat& img)
{
	//cv::Size imgSize(img.cols, img.rows - UP_CUT);
	cv::Size imgSize(img.cols, img.rows*(1- 1.6/16.2));
	if (imgSize.height <= 0 || imgSize.width <= 0)
	{
		qDebug() << "Invalid image. Size:" << imgSize.width <<"x"<<imgSize.height;
		return -1;
	}
	//截取图像边缘
	cv::Mat pt1 = img(cv::Rect(10, img.rows*1.6 / 16.2, imgSize.width - 20, imgSize.height - 40));
	cv::imshow(WIN_NAME, pt1);
	Mat pt;
	cvtColor(pt1, pt, CV_BGR2GRAY);
	

	//保存拼图
	//cv::imwrite("origin_4.jpg", pt);
	//exit(0);

	//顶点获取
	/*uchar *p;
	int f=0;
	for (int i = 137;i <=137;i++)
	{
		p = pt.ptr<uchar>(i);
		for (int j =96;j < 1000;j++)
		{
			if (p[j] == 255)
			{
				f = 1;
				cout << j;
			    break;
			}
		}

		if(f==1)
		{

			break;
		}
	}*/

	//中心点计算
	
	int n = 2;
	int nxny[2][8] = { 0 };
	for (int i = 0;i < n;i++)
	{
		nxny[0][i] = x1_origin + (2*i + 1)*(x2_origin - x1_origin) / (2*n);
		nxny[1][i] = y1_origin + (2*i + 1)*(y2_origin - y1_origin) / (2*n);
	}

	//surf特征匹配
	/*Mat srcImage = imread("origin.jpg");
	imshow("原图", srcImage);

	//首先得到特征点的集合  
	//先配置参数  
	vector<KeyPoint> keyPoint;
	//在库中：typedef SURF SurfFeatureDetector;   typedef SURF SurfDescriptorExtractor;所以三者是等价的（别名）  
	SURF surf(1000);            //1000为检测算子的阀值  
	surf.detect(srcImage, keyPoint, Mat());

	//开始绘制特征点  
	Mat dstImage;
	dstImage.create(srcImage.size(), srcImage.type());
	drawKeypoints(srcImage, keyPoint, dstImage, Scalar(-1), 2);

	imshow("检测到特征点后的图像", dstImage);

	waitKey(0);*/

	//模板匹配
	/*
	Mat origin = imread("origin_4.jpg");
	//Mat test = imread("test.jpg");
	//Mat top = test(Rect(0, 0, test.cols / 2, test.rows / 2));
	//imwrite("test_top", top);
	//Mat test_top = imread("test_top.jpg");
	//imshow(image_window,origin);
	//imshow(result_window,test);
	MatchingMethod(0,0,pt,origin);
	//Sleep(4000);*/

	
	//ORB
	//读取图片
	Mat rgbd = imread("origin_4.jpg", CV_8UC1);
	//Mat rgbd1 = imread("test_36.jpg", CV_8U);
	//imshow("rgbd1", depth2);
	//waitKey(0);
	Ptr<ORB> orb = ORB::create();//创建ORB
	vector<KeyPoint> Keypoints_pt, Keypoints_T;//定义特征点向量
	Mat descriptors_pt, descriptors_T;
	orb->detectAndCompute(pt, Mat(), Keypoints_pt, descriptors_pt);//检测特征点
	orb->detectAndCompute(rgbd, Mat(), Keypoints_T, descriptors_T);

	int i = 0;
	//Matching
	vector<DMatch> matches;//定义匹配器
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
	matcher->match(descriptors_pt, descriptors_T, matches);//匹配二图存入matches
	//cout << "find out total " << matches.size() << " matches" << endl;
	//cout << Keypoints_pt[matches[1].queryIdx].pt.x <<endl<< Keypoints_pt[matches[1].queryIdx].pt.y;

	//选取好的特征点--没什么用
	double max_dist = 0; double min_dist = 100;
	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_pt.rows; i++)
	{
	double dist = matches[i].distance;
	if (dist < min_dist) min_dist = dist;
	if (dist > max_dist) max_dist = dist;
	}
	//printf("-- Max dist : %f \n", max_dist);
	//printf("-- Min dist : %f \n", min_dist);
	//-- Draw only "good" matches (i.e. whose distance is less than 0.6*max_dist )
	//-- PS.- radiusMatch can also be used here.
	std::vector< DMatch > good_matches;
	for (int i = 0; i < descriptors_pt.rows; i++)
	{
	if (matches[i].distance < 0.9*max_dist)
	{
	good_matches.push_back(matches[i]);
	}
	}
	//cout << "find out total " << good_matches.size() << " good_matches" << endl;

	int matches_pt_x, matches_pt_y, matches_T_x, matches_T_y;
	int matches_dist_min = 10000, disttmp=0;
	for (int i = 0; i < good_matches.size(); i++)
	{
		matches_pt_x = Keypoints_pt[good_matches[i].queryIdx].pt.x;
		matches_pt_y = Keypoints_pt[good_matches[i].queryIdx].pt.y;
		matches_T_x = Keypoints_T[good_matches[i].trainIdx].pt.x;
		matches_T_y = Keypoints_T[good_matches[i].trainIdx].pt.y;
//(matches_pt_x < x1_origin) || (matches_pt_x > x2_origin) || 
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
				//输出，判断位置
				cout << "(" << matches_pt_x << "," << matches_pt_y << ")" << endl;
				if (matches_T_x <= 120 && matches_T_y <= 270.5) cout << "1" << endl;
				if (matches_T_x <= 120 && matches_T_y >= 270.5) cout << "3" << endl;
				if (matches_T_x >= 120 && matches_T_y <= 270.5) cout << "2" << endl;
				if (matches_T_x >= 120 && matches_T_y >= 270.5) cout << "4" << endl;
				//cout << "(" << matches_pt_x << "," << matches_pt_y << ")" << endl << "(" << matches_T_x << "," << matches_T_y << ")" << endl << endl;
			}
		}
		break;
	}



	//尝试
	/*if (argM.Hit)
	{
		device->comMoveTo(5, 5);
		device->comHitDown();
		device->comMoveTo(10, 10);
		device->comHitUp();
		argM.Hit = false;


	}*/



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
/*
void MatchingMethod(int, void*,Mat temp,Mat origin)
{
	Mat img_display;
	Mat result;
	temp.copyTo(img_display);

	int result_cols = temp.cols - origin.cols + 1;
	int result_rows = temp.rows - origin.rows + 1;
	result.create(result_cols, result_rows, CV_32FC4);

	matchTemplate(temp, origin, result, match_method);

	//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal; 
	double maxVal;
	Point minLoc; 
	Point maxLoc;
	Point matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

   	matchLoc = minLoc;
	
	rectangle(img_display, matchLoc, Point(matchLoc.x + origin.cols, matchLoc.y + origin.rows), Scalar::all(0), 2, 8, 0);
	rectangle(result, matchLoc, Point(matchLoc.x + origin.cols, matchLoc.y + origin.rows), Scalar::all(0), 2, 8, 0);
	//imshow(image_window, img_display);
	//imshow(result_window, result);
	cout << result;
	return;
}*/

//触笔工作函数
void usrGameController::move_and_work(double x1, double y1, double x2, double y2)
{
	//int p;
	//cin >> p;
	//if (p == 0) {
		double real_x1, real_y1, real_x2, real_y2;
		real_x1 = x1 * 25 / 318;
		real_x2 = x2 * 25 / 318;
		real_y1 = y1 * 43.5 / 544;
		real_y2 = y2 * 43.5 / 544;

		device->comMoveTo(real_x1, real_y1);
		device->comHitDown();
		device->comMoveTo(real_x2, real_y2);
		device->comHitUp();
	//}
	//cout << "(" << x1 << "," << y1 << ")" << endl << "(" << x2 << "," << y2 << ")" << endl << endl;
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
