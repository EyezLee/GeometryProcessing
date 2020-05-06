#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <iostream>
#include <string>
#include <ostream>
#include <fstream>
#include <sstream>

using namespace std;

class Shader
{
public:
	GLuint m_shaderID;

	// build shader and program object
	Shader(string vertexPath, string fragmentPath)
	{
		// read stream
		ifstream vertexFile, fragmentFile;
		string vertexCode, fragmentCode;
		vertexFile.exceptions(ifstream::failbit | ifstream::badbit);
		fragmentFile.exceptions(ifstream::failbit | ifstream::badbit);

		try
		{
			stringstream vertexStream, fragmentStream;
			vertexFile.open(vertexPath, ifstream::in);
			vertexStream << vertexFile.rdbuf();
			vertexFile.close();
			vertexCode = vertexStream.str();
			fragmentFile.open(fragmentPath, ifstream::in);
			fragmentStream << fragmentFile.rdbuf();
			fragmentFile.close();
			fragmentCode = fragmentStream.str();
		}
		catch (ifstream::failure& e)
		{
			cout << "Failed to read shader file" << endl;
		}

		const char* vertexShaderSource = vertexCode.c_str();
		const char* fragmentShaderSource = fragmentCode.c_str();
		// create shader object
		GLuint vertexShader, fragmentShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);
		CheckShaderProgram(vertexShader, "VERTEX");
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		CheckShaderProgram(fragmentShader, "FRAGMENT");

		// create and link shader program
		m_shaderID = glCreateProgram();
		glAttachShader(m_shaderID, vertexShader);
		glAttachShader(m_shaderID, fragmentShader);
		glLinkProgram(m_shaderID);
		CheckShaderProgram(m_shaderID, "PROGRAM");

		// delete shaders
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	void SetVec3(string name, glm::vec3& value)
	{
		glUniform3fv(glGetUniformLocation(m_shaderID, name.c_str()), 3, &value[0]);
	}

	// set data
	void SetMat4(string name, glm::mat4 &value)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_shaderID, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}

	// use program
	void UseShader()
	{
		glUseProgram(m_shaderID);
	}

private:
	void CheckShaderProgram(GLuint shader, string type)
	{
		if (type == "PROGRAM")
		{
			GLint success;
			char infoLog[1024];
			glGetProgramiv(m_shaderID, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				cout << type << " compile error: " << infoLog << endl;
			}
		}
		else
		{
			GLint success;
			char infoLog[1024];
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				cout << type << " shader compile error: " << infoLog << endl;
			}
		}
	}
};

#endif // !SHADER_H

