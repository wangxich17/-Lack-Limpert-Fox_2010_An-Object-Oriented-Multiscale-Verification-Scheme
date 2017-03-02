#include<Eigen/Dense>
#include<iostream>
using namespace std;
using namespace Eigen;
int f_havecells(MatrixXf Zj)
{
	if (Zj.rows() == 1) {
		return 0;
	}
	else
		return 1;
}