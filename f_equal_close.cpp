/*主要用于寻找未被访问forecast的cells中距离truth的cell最近的cell，按照就近原则匹配, b表示cell的数目，
visit记录是否被访问，0代表未尚访问，(Cx, Cy)代表truth中cell的中心点，
Zjk_intensity存放着forecast的cells的中心(Cx, Cy)和雨水强度最小值最大值以及均值（Cmin, Cmax, Cavg）*/
#include<Eigen/Dense>
#include<iostream>
#include<opencv2/opencv.hpp>
#include<Eigen/Core>
#include<complex>

using namespace std;
using namespace Eigen;

int f_equal_close(float Cx, float Cy, MatrixXf Zjk_intensity, int visit[], int b)
{
	int loc = -1;        //初始化位置loc = -1
	float min_dis = 0;   //初始化距离min_dis = 0;
	for (int i = 0; i < b; i++) {
		cout << visit[i] << endl;
		float t_Cx = Zjk_intensity(0, i);    //(tx_CX, ty_Cy)对应着forecast第i个cell的中心坐标
		float t_Cy = Zjk_intensity(1, i);
		if (visit[i] == 0) {               // 判断该位置是否已经被匹配
			if (loc == -1) {               //如果是第一次找到没访问过的cell更新loc和min_dis
				loc = i;                   //标记此位置
				min_dis = sqrt((Cx - t_Cx)*(Cx - t_Cx) + (Cy - t_Cy)*(Cy - t_Cy));  //更新距离
			}
			else {
				if (min_dis > sqrt((Cx - t_Cx)*(Cx - t_Cx) + (Cy - t_Cy)*(Cy - t_Cy))) //找到距离更近的且未被访问，则更新loc和min_dis
				{
					loc = i;                               ////标记此位置
					min_dis = sqrt((Cx - t_Cx)*(Cx - t_Cx) + (Cy - t_Cy)*(Cy - t_Cy));  //更新距离
				}
			}
		}
	}
	visit[loc] = 1;     //标记本次查找最近forecast中cell为1代表已经访问
	return loc;
}


