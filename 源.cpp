/* f_cells()主要计算出每个雨区cell的(x + i*sin(angle)*y...)的向量表示既是Z,
在计算出每个cell的Z_intensity(Cx, Cy, Cmin, Cmax, Cavg)，Cell的中心(Cx, Cy)和雨水强度最小值最大值以及均值（Cmin, Cmax, Cavg）*/
#include<Eigen/Dense>   
#include<complex>          //复数的头文件
#include<iostream>
#include<math.h>
#include <opencv2/opencv.hpp>     //opencv 图像处理头文件   

using namespace std;           //命名空间
using namespace Eigen;
using namespace cv;


MatrixXf f_intensities(char *filename);
MatrixXf f_angle(MatrixXf boundary, float Cx, float Cy);
MatrixXcf f_Z(MatrixXf Res);
MatrixXcf f_cells(char *filename);
MatrixXf f_small(MatrixXcf Zjk, MatrixXcf Zj, MatrixXf Zjk_intensity, MatrixXf Zj_intensity);
MatrixXf f_equal(MatrixXcf Zjk, MatrixXcf Zj, MatrixXf Zjk_intensity, MatrixXf Zj_intensity);
MatrixXf f_big(MatrixXcf Zjk, MatrixXcf Zj, MatrixXf Zjk_intensity, MatrixXf Zj_intensity);
int f_havecells(MatrixXf Zj);

int main()
 {  
	 float flag, SEmin, SEmax, SEavg, SEd, SEr, SEt, RSE, D;
	char filename_fcst[256] = "anal_17.dat";
	char filename_anal[256] = "fcst_17.dat";
	
	MatrixXf  Zj_intensity= f_intensities(filename_anal).transpose();
	MatrixXf Zjk_intensity = f_intensities(filename_fcst).transpose();
	//根据truth的cells和forecast 的cells的差值，分为三种情况讨论
	MatrixXf res;
	if (f_havecells(Zj_intensity) == 0 ||f_havecells(Zjk_intensity) == 0) {
		for (int i = 0; i < 9; i++) {
			cout << -1 << " ";
		}
		cout << endl;
		return 0;
	}
	int Zjkb = Zjk_intensity.cols();        //求列数不能使用Zj.clos(),因为它是MatrixXcf 类型
	int Zjb = Zj_intensity.cols();
	MatrixXcf Zj = f_cells(filename_anal);
	MatrixXcf Zjk = f_cells(filename_fcst);
	 if (Zjkb == Zjb) { 
		// truth 的cells等于forecast的cells时
		res=f_equal(Zjk, Zj, Zjk_intensity, Zj_intensity);
		flag = 0;             ///其中(Zjkb > Zjb) flag表示false, (Zjkb == Zjb)flag表示hit，flag(Zjkb < Zjb)flag表示miss
	}
	else if (Zjkb < Zjb) {
		//truth 的cells比forecast的cells多时
		  res= f_big(Zjk, Zj, Zjk_intensity, Zj_intensity);
		     flag = -1;
	}

	else if (Zjkb>Zjb) {      
		//truth 的cells比forecast的cells少时
		 res = f_small(Zjk, Zj, Zjk_intensity, Zj_intensity);
	     flag = 1;
	}
	cout << flag << " " << res << endl;
	return 0;
}