#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <algorithm>

#include <../../scene.h>

using namespace std;

void ParseCameraandProjection(stringstream* scnFile, string line, glm::mat4* cam, glm::mat4* proj);
void ParseLight(stringstream* scnFile, light_t* lits, string line);
void ParseObj(stringstream* scnFile, string line, Mesh* mesh);
void ParseMatirialandWorldTrans();

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
		string firstWord = line.substr(0);
		if (firstWord == "camera:")
			ParseCameraandProjection(&sceneStream, line, &scn->camera, &scn->projection);
		else if (firstWord == "light:")
			ParseLight(&sceneStream, &scn->lights, line);
		else if (firstWord == "objects:")
			ParseObj(&sceneStream, line, &scn->mesh);
		//else 
		//	ParseMatirialandWorldTrans();
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
	*cam = glm::translate(*cam, glm::vec3(0.0f, 0.0f, -3.0f));
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

void ParseObj(stringstream* sceneFile, string line, Mesh* mesh)
{
	while (line.size() > 0)
	{
		getline(*sceneFile, line);
		if (line.size() == 0)
			break;
		string tag, objPath;
		istringstream iss(line);
		iss >> tag >> objPath;
		iss.clear();
		string fullPath = "../src/hw5/" + objPath;
		ifstream objFile;
		stringstream objStream;
		string objLine;

		// enter into obj file
		objFile.exceptions(ifstream::badbit | ifstream::failbit);
		try
		{
			objFile.open(fullPath, ifstream::in);
			objStream << objFile.rdbuf();
			objFile.close();
			cout << objStream.str() << endl;
		}
		catch (ifstream::failure e)
		{
			cout << "Fail to open .obj file" << endl;
		}
		// parse obj file to scene.mesh
		while (!objStream.eof())
		{
			getline(objStream, objLine);
			istringstream iss(objLine);
			glm::vec3 xyz;
			iss >> tag >> xyz.x >> xyz.y >> xyz.z;
			iss.clear();
			if (tag == "f")
			{
				mesh->indices.push_back(xyz.x);
				mesh->indices.push_back(xyz.y);
				mesh->indices.push_back(xyz.z);
			}
			else if(tag == "v")
			{
				mesh->vertices.push_back(xyz);
			}
		}
	}
}

void ParseMatirialandWorldTrans()
{

}

#endif // !PARSER_H