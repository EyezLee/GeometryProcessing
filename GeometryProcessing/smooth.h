#ifndef SMOOTH_H
#define SMOOTH_H

#include <Eigen/Dense>
#include <Eigen/Sparse>
// build sparse matrix for laplacian

Eigen::SparseMatrix<double>  build_laplacian(vector<he::HEV*> *hev)
{
	// initial matrix
	int verticesNum = hev->size() - 1;
	Eigen::SparseMatrix<double> DL(verticesNum, verticesNum); // discrete laplacian 
	// reserve 0

	// calculate cot a b sum per vertex

	// I - nablas; implicit or backward eulur 

	return DL;
}

// solve position x y z 
void solve()
{

}
// solve possion equation by heat equation


#endif // !SMOOTH_H
