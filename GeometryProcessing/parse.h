#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>

#include <../../scene.h>

using namespace std;

void ParseCameraandProjection(stringstream* scnFile, glm::mat4* cam, glm::mat4* proj, string line);
void ParseLight(stringstream* scn, string line);
void ParseObj();
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

	string line;
	while (!sceneStream.eof())
	{
		getline(sceneStream, line);
		string firstWord = line.substr(0);
		if (firstWord == "camera:")
			ParseCameraandProjection(&sceneStream, &scn->camera, &scn->projection, line);
		else if (firstWord == "light:")
			ParseLight(&sceneStream, line);
		//else if (firstWord == "object:")
		//	ParseObj();
		//else 
		//	ParseMatirialandWorldTrans();
	}
}

void ParseCameraandProjection(stringstream* sceneFile, glm::mat4* cam, glm::mat4* proj, string line)
{
	while (line.size() != 0)
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

void ParseLight(stringstream* sceneFile, string line)
{
	while (line.size() != 0)
	{
		getline(*sceneFile, line);
		cout << line << endl;
	}
}

void ParseObj()
{

}

void ParseMatirialandWorldTrans()
{

}

#endif // !PARSER_H
