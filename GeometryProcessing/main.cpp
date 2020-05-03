#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <../../parse.h>
#include <../../GeometryProcessing/shader.h>
// namespace 
using namespace std;

// ultilities variables
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// ultilities func
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// vertices for testing
//float vertices[] =
//{
//	-0.5f, -0.5f, -0.5f, 
//	0.5f, -0.5f, -0.5f,  
//	0.5f,  0.5f, -0.5f,  
//	0.5f,  0.5f, -0.5f,  
//	-0.5f,  0.5f, -0.5f, 
//	-0.5f, -0.5f, -0.5f, 
//
//	-0.5f, -0.5f,  0.5f, 
//	0.5f, -0.5f,  0.5f,  
//	0.5f,  0.5f,  0.5f,  
//	0.5f,  0.5f,  0.5f,  
//	-0.5f,  0.5f,  0.5f, 
//	-0.5f, -0.5f,  0.5f, 
//
//	-0.5f,  0.5f,  0.5f, 
//	-0.5f,  0.5f, -0.5f, 
//	-0.5f, -0.5f, -0.5f, 
//	-0.5f, -0.5f, -0.5f, 
//	-0.5f, -0.5f,  0.5f, 
//	-0.5f,  0.5f,  0.5f, 
//
//	0.5f,  0.5f,  0.5f,  
//	0.5f,  0.5f, -0.5f,  
//	0.5f, -0.5f, -0.5f,  
//	0.5f, -0.5f, -0.5f,  
//	0.5f, -0.5f,  0.5f,  
//	0.5f,  0.5f,  0.5f,  
//
//	-0.5f, -0.5f, -0.5f, 
//	0.5f, -0.5f, -0.5f,  
//	0.5f, -0.5f,  0.5f,  
//	0.5f, -0.5f,  0.5f,  
//	-0.5f, -0.5f,  0.5f, 
//	-0.5f, -0.5f, -0.5f, 
//
//	-0.5f,  0.5f, -0.5f, 
//	0.5f,  0.5f, -0.5f,  
//	0.5f,  0.5f,  0.5f,  
//	0.5f,  0.5f,  0.5f,  
//	-0.5f,  0.5f,  0.5f, 
//	-0.5f,  0.5f, -0.5f, 
//};
//
//int index[] =
//{
//	1, 0, 2,
//	1, 3, 2
//};

// scene data
Scene scene;

int main()
{
	// config glfw window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// window object
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GeometryProcessing", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// config glad function pointers of OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to init GLAD" << endl;
		return -1;
	}

	string scenePath = "../src/hw5/scene_cube1.txt";
	ParseScene(&scene, scenePath);

	// VAO container: VBO + EBO + vertex attributes operation
	GLuint VAO; // vertex array object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	GLuint VBO; // vertex buffer object
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, scene.meshMap.begin()->second.vertices.size() * sizeof(glm::vec3), &scene.meshMap.begin()->second.vertices[0], GL_STATIC_DRAW);
	GLuint EBO; // element buffer object
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene.meshMap.begin()->second.indices.size() * sizeof(GLuint), &scene.meshMap.begin()->second.indices[0], GL_STATIC_DRAW);

	// prepare shader program
	string vertexPath = "../src/shaders/default.vs";
	string fragmentPath = "../src/shaders/default.fs";
	Shader shaderProgram(vertexPath, fragmentPath);
	// bind vertex to shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	shaderProgram.UseShader(); // need to install shader program object first
	glm::mat4 tempModel = glm::mat4(1.0f);
	tempModel = glm::rotate(tempModel, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	shaderProgram.SetMat4("model", tempModel);
	shaderProgram.SetMat4("camera", scene.camera);
	shaderProgram.SetMat4("projection", scene.projection);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(0, 0, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// active shader program
		shaderProgram.UseShader();
		// bind VAO
		glBindVertexArray(VAO);
		// draw
		//glDrawArrays(GL_TRIANGLES, 0, scene.mesh.vertices.size());
		glDrawElements(GL_TRIANGLES, scene.meshMap.begin()->second.indices.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// terminte program
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}