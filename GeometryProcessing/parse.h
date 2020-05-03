#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <map>

#include <../../scene.h>

using namespace std;

using mesh_map = std::map<string, Mesh>;
void ParseCameraandProjection(stringstream* scnFile, string line, glm::mat4* cam, glm::mat4* proj);
void ParseLight(stringstream* scnFile, light_t* lits, string line);
void ParseObj(stringstream* scnFile, string line, mesh_map* meshMap);
void ParseMatirialandWorldTrans(stringstream* scnFile, string line, model_t* models);

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
			ParseMatirialandWorldTrans(&sceneStream, line, &scn->models);
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

		// enter into obj file
		string fullPath = "../src/hw5/" + objPath;
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
				GLuint vertexIndices[3], normalIndices[3];
				iss >> initial >> vertexIndices[0] >> normalIndices[0]
					>> vertexIndices[1] >> normalIndices[1]
					>> vertexIndices[2] >> normalIndices[2];
				currMesh.indices.push_back(vertexIndices[0]);
				currMesh.indices.push_back(vertexIndices[1]);
				currMesh.indices.push_back(vertexIndices[2]);
				// parse normal indices when neccessary...
			}
			iss.clear();
		}
		meshMap->insert(std::pair<string, Mesh>(tag, currMesh));
	}
}

void ParseMatirialandWorldTrans(stringstream* scnFile, string line, model_t* models)
{
	int numModel = 0;
	while (line.size() > 0)
	{
		string name;
		if (line.size() == 1)
		{
			name = line;
			numModel++;
			Model currModel;
			currModel.name = name;
			models->push_back(currModel);
		}
		getline(*scnFile, line);
		istringstream iss(line);
		string tag = line.substr(0, line.find(" "));
		if (models->size() > 0)
		{
			if (tag == "ambient")
			{
				glm::vec3 amb;
				iss >> tag >> amb.x >> amb.y >> amb.z;
			}
		}
		iss.clear();
	}
}

#endif // !PARSER_H
