﻿#ifndef SMOOTH_H
#define SMOOTH_H

#include <Eigen/Dense>
#include <Eigen/Sparse>
// build sparse matrix for laplacian

Eigen::SparseMatrix<double>  build_laplacian(vector<he::HEV*>* hev)
{
	// initialize matrix
	int verticesNum = hev->size() - 1;
	Eigen::SparseMatrix<double> laplacian(verticesNum, verticesNum); // discrete laplacian 

	// reserve space for non-zero elements in sparse matrix
	laplacian.reserve(7);

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
			laplacian.insert(i - 1, j - 1) = -(cotA + cotB) / 2 * area; // when i != j

			// move to the next vertex
			halfedge = halfedge->flip->next;
		} 
		while (halfedge != v->out); 
		
		//	∑(cot(alpha) + cot(beta))ij	/ 2A		i=j
		laplacian.insert(i - 1, i - 1) = cotSum / 2 * area;
	}

	// heat equation after implicit/ backward Euler
	// F = (I - h * ∆)
	double timeStep = 0.002;
	Eigen::SparseMatrix<double> identity(verticesNum, verticesNum);
	identity.reserve(1);
	identity.setIdentity();
	laplacian = identity - timeStep * laplacian;

	// for efficient storage
	laplacian.makeCompressed();
	return laplacian;
}

// solve position x y z 
void Smooth(vector<he::HEV*>* hev)
{
	// build discrete laplacian matrix operator
	Eigen::SparseMatrix<double> F = build_laplacian(hev);

	// init solver
	Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;

	// tailor solver to matrix
	solver.analyzePattern(F);
	solver.factorize(F);

	// roll in rho vectors
	/* 
	Poisson equation Δϕ = ρ
	F * xh = x0
	x0 is rho 
	*/
	int verticesNum = hev->size() - 1;
	Eigen::VectorXd rhoX(verticesNum);
	Eigen::VectorXd rhoY(verticesNum);
	Eigen::VectorXd rhoZ(verticesNum);
	for (int i = 1; i < hev->size(); i++)
	{
		rhoX(i - 1) = hev->at(i)->position.x;
		rhoY(i - 1) = hev->at(i)->position.y;
		rhoZ(i - 1) = hev->at(i)->position.z;
	}

	// init newly smoothed vectors
	Eigen::VectorXd phiX(verticesNum);
	Eigen::VectorXd phiY(verticesNum);
	Eigen::VectorXd phiZ(verticesNum);

	// solve
	phiX = solver.solve(rhoX);
	phiY = solver.solve(rhoY);
	phiZ = solver.solve(rhoZ);

	// update vertices position and normals
	for (int i = 1; i < hev->size(); i++)
	{
		hev->at(i)->position.x = phiX(i - 1);
		hev->at(i)->position.y = phiY(i - 1);
		hev->at(i)->position.z = phiZ(i - 1);
	}
	updata_HE_normal(hev);
}


#endif // !SMOOTH_H
