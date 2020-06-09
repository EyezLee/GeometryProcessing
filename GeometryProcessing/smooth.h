#ifndef SMOOTH_H
#define SMOOTH_H

#include <Eigen/Dense>
#include <Eigen/Sparse>
// build sparse matrix for laplacian

Eigen::SparseMatrix<double>  build_laplacian(vector<he::HEV*>* hev)
{
	// initialize matrix
	int verticesNum = hev->size() - 1;
	Eigen::SparseMatrix<double> DL(verticesNum, verticesNum); // discrete laplacian 

	// reserve space for non-zero elements
	DL.reserve(7);

	// fill out matrix for per vertex
	for (int i = 1; i < hev->size(); i++) // index 0 is null 
	{
		he::HEV* v = hev->at(i);
		he::HE* halfedge = v->out;
		do
		{
			// calculate curvature normal sum/ discrete laplacian value at nextV
			// ∆X = 1 / 2A ∑(cot(alpha) + cot(beta0)) * (Xi - Xj);
			he::HEV* nextV = halfedge->next->vertex;
			int idx = nextV->index;
			he::HEV* alphaV = halfedge->next->next->vertex;
			he::HEV* betaV = halfedge->flip->next->next->vertex;

			// go through all the incident vertices for vertex[i]
			
		} 
		while (halfedge != hev->at(i)->out); 
	}
	// I - nablas; implicit or backward euler

	return DL;
}

// solve position x y z 
void solve()
{

}
// solve possion equation by heat equation


#endif // !SMOOTH_H
