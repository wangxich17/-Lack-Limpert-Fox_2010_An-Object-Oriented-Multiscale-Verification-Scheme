#include<Eigen/Dense>
#include<iostream>
#include<opencv2/opencv.hpp>
#include<Eigen/Core>
#include<complex>

using namespace std;
using namespace Eigen;
const int cells =100000;
int f_equal_close(float Cx, float Cy, MatrixXf Zjk_intensity, int visit[], int b);
MatrixXf f_res(MatrixXcf Zjk, MatrixXcf Zj, float Cavg_Zjk, float Cavg_Zj, float Cmin_Zjk, float Cmin_Zj, float Cmax_Zjk, float Cmax_Zj);

MatrixXf f_bigtoequal(MatrixXcf Zjk, MatrixXcf Zj, MatrixXf Zjk_intensity, MatrixXf Zj_intensity)
{
	float SEmin, SEmax, SEavg, SEd, SEr, SEt, RSE, D;
	int b = Zj.cols();
	int bk = Zjk.cols();// Zjk的每一列存储了一个cells的向量信息，因此矩阵Zj的列数b既是cells的数目
	MatrixXf T_res(1, 8);      // Tres()用来累计truth的第i个cell和forecast的第i个cell的误差
	T_res.setZero();
	int visit[cells];
	memset(visit, 0, sizeof(0));
	memset(visit, 0, sizeof(visit));//用来标记forecast的cells匹配情况
	for (int i = 0; i < b; i++) {
		//找出位置为loc的未访问的距离truth的第i个cell的中心(Cx, Cy)最近的forecst的cell 进行match
		float Cx = Zj_intensity(0, i);
		float Cy = Zj_intensity(1, i);
		int loc = f_equal_close(Cx, Cy, Zjk_intensity, visit, bk);
		visit[loc] = 1;

		//循环赋值矩阵t_Zj和t_Zjk（复数形式）,用于后面计算
		MatrixXcf t_Zj(Zj.rows(), 1);
		for (int j = 0; j < Zj.rows(); j++) {
			t_Zj(j, 0)._Val[0] = Zj(j, i)._Val[0];
			t_Zj(j, 0)._Val[1] = Zj(j, i)._Val[1];
		}
		MatrixXcf t_Zjk(Zjk.rows(), 1);
		for (int j = 0; j < Zjk.rows(); j++) {
			t_Zjk(j, 0)._Val[0] = Zjk(j, i)._Val[0];
			t_Zjk(j, 0)._Val[1] = Zjk(j, i)._Val[1];
		}

		//取出雨区的降雨最小值Cmin,最大值Cmax,平均值Cavg，用于计算
		float Cmin_Zj = Zj_intensity(2, i);
		float Cmax_Zj = Zj_intensity(3, i);
		float Cavg_Zj = Zj_intensity(4, i);
		float Cmin_Zjk = Zjk_intensity(2, loc);
		float Cmax_Zjk = Zjk_intensity(3, loc);
		float Cavg_Zjk = Zjk_intensity(4, loc);

		//f_res()计算出每个两个cell的各项误差
		MatrixXf Res = f_res(t_Zjk, t_Zj, Cavg_Zjk, Cavg_Zj, Cmin_Zjk, Cmin_Zj, Cmax_Zjk, Cmax_Zj);
		for (int m = 0; m < 8; m++) {
			T_res(0, m) = T_res(0, m) + Res(0, m);
		}
	}
	SEmin = T_res(0, 0);
	SEmax = T_res(0, 1);
	SEavg = T_res(0, 2);
	//按照公式SEd, SEt, SEr取均值
	SEd = T_res(0, 3) / b;
	SEr = T_res(0, 4) / b;
	SEt = T_res(0, 5) / b;
	RSE = T_res(0, 6);
	D = T_res(0, 7);

	//将结果全部返回Res_alue()
	MatrixXf res_value(1, 8);
	res_value << SEmin,SEmax,SEavg,SEd,SEr,SEt,RSE,D;
	return res_value;
}
