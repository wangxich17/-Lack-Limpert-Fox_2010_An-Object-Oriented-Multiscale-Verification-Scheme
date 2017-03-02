#include<Eigen/Dense>   
#include<complex>          //复数的头文件
#include<iostream>
#include<math.h>
#include <opencv2/opencv.hpp>     //opencv 图像处理头文件   

using namespace std;           //命名空间
using namespace Eigen;
using namespace cv;

const int len_row = 128;     //根据二进制文件设置图像的宽和高
const int len_col = 61;
const int file_size = 7808;
const int threshold_value_rain = 25;   //设置降雨量过滤的阈值
const int threshold_value_contours = 6;  //设置轮廓过滤的阈值

MatrixXf f_intensities(char *filename)
{
	FILE *fp = fopen(filename, "rb");
	MatrixXf A(1, 1);
	A << -1;///以二进制形式打开文件
	if (fp == NULL) {
		cout << "No such file" << endl;
		return A;
	}
	//以四个字节为单位，依次读取fp所指向的数值，并且通过阈值threshold_value过滤
	float temp;

	Mat I0(len_row, len_col, CV_32F), I1(len_row, len_col, CV_32F), I2;
	for (int i = 0; i < 7808; i++) {
		fread(&temp, 4, 1, fp);
		if (temp > threshold_value_rain) {
			I0.at<float>(i / len_col, i%len_col) = temp;
			I1.at<float>(i / len_col, i%len_col) = 255;
		}
		else {
			I0.at<float>(i / len_col, i%len_col) = 0;
			I1.at<float>(i / len_col, i%len_col) = 0;
		}
		fseek(fp, 4L, 1);
	}
	fclose(fp);//关闭文件


			   //提取轮廓
	I1.convertTo(I2, CV_8U, 1, 0);
	vector<vector<Point>> contours;
	vector<vector<Point>> contours2;
	vector<Vec4i> hierarchy;
	Point temp_point;
	findContours(I2, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	for (int i = 0; i<contours.size(); i++) {
		if (contours[i].size()>threshold_value_contours) {
			vector<Point> temp_vector;
			for (int j = 0; j < contours[i].size(); j++) {
				temp_point.x = contours[i][j].x;
				temp_point.y = contours[i][j].y;
				temp_vector.push_back(temp_point);
			}
			contours2.push_back(temp_vector);   //过滤后的轮廓
		}
	}

	//计算Cx,Cy,Cmax，Cmin,Cavg;
	float Cx, Cy, Cmin, Cmax, Cavg, sum_x, sum_y, sum_avg, Tavg;
	int Tx, Ty;
	MatrixXf intensities(contours2.size(), 5);
	for (int i = 0; i < contours2.size(); i++) {
		sum_x = 0;
		sum_y = 0;
		sum_avg = 0;
		for (int j = 0; j < contours2[i].size(); j++) {
			Tx = contours2[i][j].x;
			Ty = contours2[i][j].y;
			Tavg = I0.at<float>(Ty, Tx);
			sum_x += Tx;
			sum_y += Ty;
			sum_avg += Tavg;
			if (j == 0) {
				Cmin = Cmax = Tavg;
			}
			else {
				if (Cmin > Tavg) {
					Cmin = Tavg;
				}
				if (Cmax < Tavg) {
					Cmax = Tavg;
				}
			}
		}
		intensities(i, 0) = sum_x / contours2[i].size();
		intensities(i, 1) = sum_y / contours2[i].size();
		intensities(i, 2) = Cmin;
		intensities(i, 3) = Cmax;
		intensities(i, 4) = sum_avg / contours2[i].size();
	}
	if (contours2.size()>0)
		return  intensities;
	else
		return A;
}