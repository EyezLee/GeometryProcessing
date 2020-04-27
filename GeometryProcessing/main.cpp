#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
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
float vertices[] =
{
	-0.5, 0.5, 0.0,
	-0.5, -0.5, 0.0,
	0.5, 0.5, 0.0,
	0.5, -0.5, 0.0
};

int index[] =
{
	0, 1, 2,
	1, 3, 2
};

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

	// VAO container: VBO + EBO + vertex attributes operation
	GLuint VAO; // vertex array object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	GLuint VBO; // vertex buffer object
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	GLuint EBO; // element buffer object
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), &index, GL_STATIC_DRAW);

	// prepare shader program
	string vertexPath = "../src/shaders/default.vs";
	string fragmentPath = "../src/shaders/default.fs";
	Shader shaderProgram =  Shader(vertexPath, fragmentPath);
	// bind vertex to shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(1, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// active shader program
		shaderProgram.UseShader();
		// bind VAO
		glBindVertexArray(VAO);
		// draw
		glDrawArrays(GL_TRIANGLES, 0, 6);

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