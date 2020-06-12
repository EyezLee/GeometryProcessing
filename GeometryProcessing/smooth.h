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

	// reserve space for non-zero elements in sparse matrix
	DL.reserve(7);

	// fill out matrix for per vertex
	for (int i = 1; i < hev->size(); i++) // index 0 is null 
	{
		he::HEV* v = hev->at(i);
		he::HE* halfedge = v->out;

		// calculate the incident area A
		float area = 0;
		do 
		{
			he::HEV* nextV = halfedge->next->vertex;
			// incident area of vertex i 
			float triangle = glm::length(glm::cross(nextV->position - v->position, halfedge->next->next->vertex->position - v->position)) / 2; // triangle area
			area += triangle;

			// traverse to next vertex
			halfedge = halfedge->next->next->flip;
		} 
		while (halfedge != v->out);

		// store matrix
		float cotSum = 0; // for when i = j
		halfedge = v->out; // reset halfedge
		do
		{
			// calculate curvature normal sum/ discrete laplacian value at nextV
			he::HEV* nextV = halfedge->next->vertex;
			int j = nextV->index;
			he::HEV* alphaV = halfedge->next->next->vertex;
			he::HEV* betaV = halfedge->flip->next->next->vertex;

			// cotagent value alpha and beta; cot = cos / sin = dot product / magnitude of cross product
			glm::vec3 alphatoV = v->position - alphaV->position;
			glm::vec3 alphatoN = nextV->position - alphaV->position;
			float cotA = glm::dot(alphatoV, alphatoV) / glm::length(glm::cross(alphatoV, alphatoN)); 
			glm::vec3 betatoV = v->position - betaV->position;
			glm::vec3 betatoN = nextV->position - betaV->position;
			float cotB = glm::dot(betatoV, betatoN) - glm::length(glm::cross(betatoV, betatoN));
			cotSum += cotA + cotB;
			/*
			∆ = 1 / 2A ∑(cot(alpha) + cot(beta)) * (vi - vj);
			   = ∑(cot(alpha) + cot(beta)) / 2A * vi - ∑(cot(alpha) + cot(beta)) / 2A * vj;
			   = vi * ∑(cot(alpha) + cot(beta)) / 2A - ∑(cot(alpha) + cot(beta)) / 2A * vj;
			 ∆ij={
						−(cot(alpha) + cot(beta)) / 2A			i≠j
						∑(cot(alpha) + cot(beta))ij	/ 2A		i=j
					}	
			*/
			DL.insert(i - 1, j - 1) = -(cotA + cotB) / 2 * area; // when i != j

			// move to the next vertex
			halfedge = halfedge->flip->next;
		} 
		while (halfedge != v->out); 
		
		//	∑(cot(alpha) + cot(beta))ij	/ 2A		i=j
		DL.insert(i - 1, i - 1) = cotSum / 2 * area;
	}

	// for efficient storage
	DL.makeCompressed();
	return DL;
}

// solve position x y z 
void solve()
{

}
// solve possion equation by heat equation


#endif // !SMOOTH_H
