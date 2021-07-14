#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>

//#include <../../scene.h>
#include <../../GeometryProcessing/scene.h>

using namespace std;

//using mesh_map = std::map<string, Mesh>;
void ParseCameraandProjection(stringstream* scnFile, string line, glm::mat4* cam, glm::mat4* proj);
void ParseLight(stringstream* scnFile, light_t* lits, string line);
void ParseObj(stringstream* scnFile, string line, mesh_map* meshMap);
void ParseMatirialandWorldTrans(stringstream* scnFile, string line, model_t* models, mesh_map* meshMap);

void ParseScene(Scene* scn, string scenePath)
{
	// parse scene file to string
	ifstream sceneFile;
	stringstream sceneStream;
	sceneFile.exceptions(ifstream::badbit | ifstream::failbit);
	try
	{
		sceneFile.open(scenePath, ifstream::in);
		sceneStream << sceneFile.rdbuf();
		sceneFile.close();
	}
	catch (ifstream::failure e)
	{
		cout << "Failed to load scene file: " << scenePath << endl;
	}

	scn->scenePath = scenePath;
	string line;
	while (!sceneStream.eof())
	{
		getline(sceneStream, line);
		string firstWord = line.substr(0, line.find('\n'));
		if (firstWord == "camera:")
			ParseCameraandProjection(&sceneStream, line, &scn->camera, &scn->projection);
		else if (firstWord == "light:")
			ParseLight(&sceneStream, &scn->lights, line);
		else if (firstWord == "objects:")
			ParseObj(&sceneStream, line, &scn->meshMap);
		else 
			ParseMatirialandWorldTrans(&sceneStream, line, &scn->models, &scn->meshMap);
	}
}

void ParseCameraandProjection(stringstream* sceneFile, string line, glm::mat4* cam, glm::mat4* proj)
{
	while (line.size() > 0)
	{
		getline(*sceneFile, line);
		cout << line << endl;
	}
	// initialize matrix first
	*cam = glm::mat4(1.0f); 
	*proj = glm::mat4(1.0f);
	*cam = glm::translate(*cam, glm::vec3(0.0f, 0.0f, -5.0f));
	*proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

void ParseLight(stringstream* sceneFile, light_t* lits, string line)
{
	while (line.size() > 0)
	{
		getline(*sceneFile, line);
		line.erase(remove(line.begin(), line.end(), ','), line.end());
		string tag;
		Light currLit;
		istringstream iss(line);
		iss >> tag >> currLit.pos.x >> currLit.pos.y >> currLit.pos.z >> currLit.col.x >> currLit.col.y >> currLit.col.z >> currLit.atten_k;
		iss.clear();
		lits->push_back(currLit);
		cout << line << endl;
	}
}

void ParseObj(stringstream* sceneFile, string line, mesh_map* meshMap)
{
	while (line.size() > 0)
	{
		getline(*sceneFile, line);
		if (line.size() == 0)
			break;

		// read and parse .obj file
		string tag, objPath;
		istringstream iss(line);
		iss >> tag >> objPath;
		iss.clear();

		Mesh currMesh;
		currMesh.vertices.push_back(glm::vec3(0, 0, 0)); // shift to match indices which start at 1
		currMesh.normals.push_back(glm::vec3(0, 0, 0)); // also shift normal 

		// enter into obj file
		string fullPath = "../src/sceneData/" + objPath;
		ifstream objFile;
		stringstream objStream;
		objFile.exceptions(ifstream::badbit | ifstream::failbit);
		try
		{
			objFile.open(fullPath, ifstream::in);
			objStream << objFile.rdbuf();
			objFile.close();
			//cout << objStream.str() << endl;
		}
		catch (ifstream::failure e)
		{
			cout << "Fail to open .obj file" << endl;
		}
		// parse obj file to scene.mesh
		while (!objStream.eof())
		{
			string objLine;
			getline(objStream, objLine);
			istringstream iss(objLine);
			string firstWord = objLine.substr(0, objLine.find(" "));
			string initial;
			if (firstWord == "v")
			{
				glm::vec3 pos;
				iss >> initial >> pos.x >> pos.y >> pos.z;
				currMesh.vertices.push_back(pos);
			}
			else if (firstWord == "vt")
			{
				glm::vec2 coord;
				iss >> initial >> coord.x >> coord.y;
				currMesh.texCoords.push_back(coord);
			}
			else if (firstWord == "vn")
			{
				glm::vec3 norm;
				iss >> initial >> norm.x >> norm.y >> norm.z;
				currMesh.normals.push_back(norm);
			}
			else if (firstWord == "f")
			{
				std::replace(objLine.begin(), objLine.end(), '/', ' ');
				iss.str(objLine);
				glm::vec3 vertexIndices, normalIndices;
				iss >> initial >> vertexIndices.x >> normalIndices.x
					>> vertexIndices.y >> normalIndices.y
					>> vertexIndices.z >> normalIndices.z;
				currMesh.indices.push_back(vertexIndices);
				currMesh.normIndices.push_back(normalIndices);
				// parse normal indices when neccessary...
			}
			iss.clear();
		}
		meshMap->insert(std::pair<string, Mesh>(tag, currMesh));
	}
}

void ParseMatirialandWorldTrans(stringstream* scnFile, string line, model_t* models, mesh_map* meshMap)
{
	int numModel = 0;
	while (line.size() > 0)
	{
		string name;
		if (meshMap->find(line) != meshMap->cend()) // if find name of the mesh in meshmap
		{
			name = line;
			numModel++;
			Model currModel;
			currModel.meshSource = &(meshMap->find(name)->second);
			currModel.modelMatrix = glm::mat4(1.0); // initialize matrix
			models->push_back(currModel);
		}
		getline(*scnFile, line);
		istringstream iss(line);
		string tag = line.substr(0, line.find(" "));
		if (models->size() > 0)
		{
			int modelCount = numModel - 1;
			if (tag == "ambient")
			{
				glm::vec3 amb;
				iss >> tag >> amb.x >> amb.y >> amb.z;
				models->at(modelCount).material.ambient = amb;
			}
			if (tag == "diffuse")
			{
				glm::vec3 diff;
				iss >> tag >> diff.x >> diff.y >> diff.z;
				models->at(modelCount).material.diffuse = diff;
			}
			if (tag == "specular")
			{
				glm::vec3 spec;
				iss >> tag >> spec.x >> spec.y >> spec.z;
				models->at(modelCount).material.specular = spec;
			}			
			if (tag == "shininess")
			{
				float shin;
				iss >> tag >> shin;
				models->at(modelCount).material.shininess = shin;
			}
			if (tag == "s")
			{
				glm::vec3 scale;
				iss >> tag >> scale.x >> scale.y >> scale.z;
				models->at(modelCount).modelMatrix = glm::scale(models->at(modelCount).modelMatrix, scale);
			}
			if (tag == "r")
			{
				glm::vec3 rot;
				float rad;
				iss >> tag >> rot.x >> rot.y >> rot.z >> rad;
				models->at(modelCount).modelMatrix = glm::rotate(models->at(modelCount).modelMatrix, rad, rot);
			}
			if (tag == "t")
			{
				glm::vec3 trans;
				iss >> tag >> trans.x >> trans.y >> trans.z;
				models->at(modelCount).modelMatrix = glm::translate(models->at(modelCount).modelMatrix, trans);
			}
		}
		iss.clear();
	}
}

// parse vertices data from scene.models to vbo
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};
using vbo_t = vector<Vertex>;

vbo_t MeshtoVBO(Model* model)
{
	vbo_t currVBO;
	GLuint indicesNum = model->meshSource->indices.size();
	for (int i = 0; i < indicesNum; i++)
	{
		Vertex currVertex;
		for (int idx = 0; idx < 3; idx++)
		{
			// push back vertex position data first
			GLuint vertexIndex = model->meshSource->indices[i][idx];
			currVertex.position = model->meshSource->vertices[vertexIndex];
			// push back normal data for per vertex
			GLuint normalIndex = model->meshSource->normIndices[i][idx];
			currVertex.normal = model->meshSource->normals[normalIndex];

			currVBO.push_back(currVertex);

			// set temp texCoord
			currVertex.texCoord = glm::vec2(1, 1);
		}
	}

	return currVBO;
}


#endif // !PARSER_H
