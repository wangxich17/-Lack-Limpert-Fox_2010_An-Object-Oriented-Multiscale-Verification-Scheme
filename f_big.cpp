/*当truth 的cells数目大于forest的cells数目，使用f_small（）函数计算[SEmin, SEmax, SEavg, SEd, SEr, SEt, RSE, D]
 按照论文中每个truth的cell都需要匹配的原则，将truth的cells分解为等于的（f_equal()）和小于的情况去解决（f_small()）;
这个时候允许forecast的cells被多次访问；*/
//当truth 的cells和forest的cells数目相等，使用f_equal（）函数计算[SEmin,SEmax,SEavg,SEd,SEr,SEt,RSE,D]
#include<Eigen/Dense>
#include<iostream>
#include<opencv2/opencv.hpp>
#include<Eigen/Core>
#include<complex>

using namespace std;
using namespace Eigen;

int f_equal_close(float Cx, float Cy, MatrixXf Zjk_intensity, int visit[], int b);
MatrixXf f_res(MatrixXcf Zjk, MatrixXcf Zj, float Cavg_Zjk, float Cavg_Zj, float Cmin_Zjk, float Cmin_Zj, float Cmax_Zjk, float Cmax_Zj);
MatrixXf f_bigtoequal(MatrixXcf Zjk, MatrixXcf Zj, MatrixXf Zjk_intensity, MatrixXf Zj_intensity);
MatrixXf f_small(MatrixXcf Zjk, MatrixXcf Zj, MatrixXf Zjk_intensity, MatrixXf Zj_intensity);
MatrixXf f_big(MatrixXcf Zjk, MatrixXcf Zj, MatrixXf Zjk_intensity, MatrixXf Zj_intensity)
{
	float SEmin, SEmax, SEavg, SEd, SEr, SEt, RSE, D;
	int bj = Zj.cols(); 
	int bjk = Zjk.cols();  // Zjk的每一列存储了一个cells的向量信息，因此矩阵Zj的列数b既是cells的数目
	MatrixXf T_res(1, 8);      // Tres()用来累计truth的第i个cell和forecast的第i个cell的误差
	T_res.setZero();
	int k = bj / bjk; //用来标记forecast的cells匹配情况
	int m = bj%bjk;  //k表示truth中cells的数目是forcast中cells数目的k倍，m表示余数，于是可以使用k次f_equal()和1次f_small()解决
	for (int i = 0; i <k; i++) {
		//找出位置为loc的未访问的距离truth的第i个cell的中心(Cx, Cy)最近的forecst的cell 进行match
		MatrixXcf t_Zj(Zj.rows(), bjk);
		for (int j = 0; j < Zj.rows(); j++) {
			for (int c = bjk*i; c <  (i + 1)*bjk; c++) {
				t_Zj(j,c-bjk*i )._Val[0] = Zj(j, c)._Val[0];
				t_Zj(j, c-bjk*i)._Val[1] = Zj(j, c)._Val[1];
			}
		}
		MatrixXf t_Zj_intensity(Zj_intensity.rows(), bjk);
		for (int j = 0; j < Zj_intensity.rows(); j++) {
			for (int c = bjk*i; c < (i + 1)*bjk; c++) {
				t_Zj_intensity(j, c - bjk*i) = Zj_intensity(j, c);
			}
		}
		//f_bigtoequal()用来计算这种情况下各项误差并且累计
		MatrixXf Res= f_bigtoequal(Zjk, t_Zj, Zjk_intensity, t_Zj_intensity);
		for (int m = 0; m < 8; m++) {
			T_res(0, m) = T_res(0, m) + Res(0, m);
		}
	}
	if (m > 0) { 
		//循环赋值矩阵t_Zj和t_Zjk（复数形式）,用于后面计算
		MatrixXcf t_Zj(Zj.rows(), m);
		for (int j = 0; j < Zj.rows(); j++) {
			for (int c = bjk*k; c < bj; c++) {
				t_Zj(j, c - bjk*k)._Val[0] = Zj(j, c)._Val[0];
				t_Zj(j, c - bjk*k)._Val[1] = Zj(j, c)._Val[1];
			}
		}
		MatrixXf t_Zj_intensity(Zjk_intensity.rows(), m);
		for (int j = 0; j < Zj_intensity.rows(); j++) {
			for (int c = bjk*k; c < bj; c++) {
				t_Zj_intensity(j, c - bjk*k) = Zj_intensity(j, c);
			}
		}

		// f_bigtoequal()用来计算这种情况下各项误差并且累计
		MatrixXf Res = f_bigtoequal(Zjk, t_Zj, Zjk_intensity, t_Zj_intensity);
		for (int m = 0; m < 8; m++) {
			T_res(0, m) = T_res(0, m) + Res(0, m);
		}
	}
	SEmin = T_res(0, 0);
	SEmax = T_res(0, 1);
	SEavg = T_res(0, 2);
	//按照公式SEd, SEt, SEr取均值
	SEd = T_res(0, 3) / bj;
	SEr = T_res(0, 4) / bj;
	SEt = T_res(0, 5) / bj;
	RSE = T_res(0, 6);
	D = T_res(0, 7);

	//将结果全部返回Res_alue()
	MatrixXf res_value(1, 8);
	res_value << SEmin,SEmax,SEavg,SEd,SEr,SEt,RSE,D;
	return res_value;
}
