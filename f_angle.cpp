/*遍历像素，并求出每个像素所对应Res = （x坐标，y坐标, 对应的偏角angle，与中心点的距，所在象限）
返回的结果是一个矩阵Res，每一行分别是（x坐标，y坐标, 对应的偏角angle，与中心点的距，所在象限）*/
#include<iostream>
#include<math.h>
#include<Eigen/Core>
#include<Eigen/Dense>
#include<complex>
using namespace std;
using namespace Eigen;

const float PI = 3.14159265358979323846;//圆周率PI

MatrixXf f_angle(MatrixXf boundary, float Cx, float Cy)
{
	float tx, ty, temp, value, dx, dy;    //临时变量
    int	k = boundary.rows();              //轮廓长度
	
	//求出每个轮廓点距离中心点的距离，形成的偏角，以及所在的象限(在假想的坐标中)
	MatrixXf Res(k, 5);                  
	for (int i = 0; i < k; i++) {
		tx = boundary(i, 0);				//tx取出i位置像素的x坐标
		ty = boundary(i, 1);				//ty取出i位置像素的y坐标
		temp = (ty - Cy) / (tx - Cx);	    //计算正切tan(y / x)
		value = atan(temp);                 //求出反正切值value既是偏角
		Res(i, 0) = tx;                     //按照i循环放入Res()矩阵中暂存
		Res(i, 1) = ty;                     //按照i循环放入Res()矩阵中暂存
		dx = tx - Cx;                       //像素点坐标和cell中心点坐标差值（dx, dy）
		dy = ty - Cy;
		if (dx > 0) {                       //假设中心点为原点，建立坐标系，判断象限，坐标系的建立和opencv图像坐标系一致
			if (dy >= 0) {
				Res(i, 2) = value;                    //对应的偏角数值
				Res(i, 3) = sqrt(dx*dx + dy*dy);      //与中心点的距离
				Res(i, 4) = 1;                        //所在象限
			}
		}
		if (dx <= 0) {
			if (dy > 0) {
				Res(i, 2) = PI + value;					//对应的偏角数值
				Res(i, 3) = sqrt(dx*dx + dy*dy);        //与中心点的距离
				Res(i, 4) = 2;                          //所在象限
			}
		}
		if (dx < 0) {
			if (dy <= 0) {
				Res(i, 2) = value + PI;               //对应的偏角数值
				Res(i, 3) = sqrt(dx*dx + dy*dy);      //与中心点的距离
				Res(i, 4) = 3;                        //所在象限
			}
		}
		if (dx >= 0) {
			if (dy < 0) {
				Res(i, 2) = 2 * PI + value;             //对应的偏角数值
				Res(i, 3) = sqrt(dx*dx + dy*dy);        //与中心点的距离
				Res(i, 4) = 4;                          //所在象限
			}
		}
	}
	return Res;
}