/*2007论文公式(8)中第4个公式*/
#include<iostream>
#include<Eigen/Core>
#include<Eigen/Dense>
#include<complex>

using namespace std;
using namespace Eigen;

complex<float> f_b0jk(MatrixXcf Zjk, MatrixXcf Zj, float Rjk, float FFjk)
{
	complex<float> T_FFjk(0, FFjk);      //将FFjk转化为复数，以用于公式计算
    complex<float> T_b0jk = Zjk.sum() - Rjk*exp(T_FFjk)*Zj.sum();     //先计算中间结果
	complex<float> b0jk(T_b0jk.real() / 7.0, T_b0jk.imag() / 7.0);     //实部和虚部分别除以A-1(此处A已默认为8)
	return b0jk;
}
