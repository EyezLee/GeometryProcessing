#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <../../parse.h>
#include <../../GeometryProcessing/shader.h>
#include <../../camera.h>
#include <../../he/halfedge.h>
// namespace 
using namespace std;

// ultilities variables
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
bool firstMouse = true;
double lastCursorX = SCR_WIDTH / 2, lastCursorY = SCR_HEIGHT / 2;

// ultilities func
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);

// scene data
Scene scene;
Camera camera(glm::vec3(0, 0, 3));

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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // tell glfw to capture mouse
	glfwSetCursorPosCallback(window, mouse_callback);

	// config glad function pointers of OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to init GLAD" << endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	string scenePath = "../src/sceneData/scene_bunny1.txt";
	ParseScene(&scene, scenePath);

	// half edge mesh data
	he::Mesh_Data* meshData = new he::Mesh_Data();
	int indexNum = scene.models[0].meshSource->indices.size();
	for (int i = 0; i < indexNum; i++)
		meshData->faces->push_back(&(scene.models[0].meshSource->indices[i]));
	int vertexNum = scene.models[0].meshSource->vertices.size();
	for (int i = 0; i < vertexNum; i++)
		meshData->vertices->push_back(&(scene.models[0].meshSource->vertices[i]));
	vector<HEF*>* hef = new vector<HEF*>;
	vector<HEV*>* hev = new vector<HEV*>;
	bool success = build_HE(meshData, hev, hef);
	for (int i = 0; i < hev->size(); i++)
	{
		HE* he = hev->at(i)->out;
		glm::vec3 normal;
		do 
		{
			HEF* f = he->face;
			HEV* v0 = f->edge->vertex;
			HEV* v1 = f->edge->next->vertex;
			HEV* v2 = f->edge->next->next->vertex;

			glm::vec3 faceNormal = glm::cross(glm::vec3(v1->x - v0->x, v1->y - v0->y, v1->z - v0->z),
												glm::vec3(v2->x - v0->x, v2->y - v0->y, v2->z - v0->z));
			float faceArea = glm::length(faceNormal) / 2; // triangle area = cross product * 1/2
			normal += faceNormal * faceArea; // area wight normal

			// next edge
			he = he->flip->next;
		} 
		while (hev->at(i)->out);
		
		// normalize normal
		normal = glm::normalize(normal);
	}

	// prepare shader program
	string vertexPath = "../src/shaders/phongShading.vs";
	string fragmentPath = "../src/shaders/phongShading.fs";
	Shader shaderProgram(vertexPath, fragmentPath);

	// VAO container: VBO + EBO + vertex attributes operation
	GLuint VAO; // vertex array object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	GLuint VBO; // vertex buffer object
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	vbo_t vboBuffer = SorttoVBO(&scene.models[0]);
	glBufferData(GL_ARRAY_BUFFER, vboBuffer.size() * sizeof(Vertex), &vboBuffer[0], GL_STATIC_DRAW);

	//GLuint EBO; // element buffer object
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene.meshMap.begin()->second.indices.size() * sizeof(GLuint), &scene.meshMap.begin()->second.indices[0], GL_STATIC_DRAW);

	// bind vertex to shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);// vertices position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));// vertices normal attribute
	glEnableVertexAttribArray(1);

	// set uniform values to shader program
	shaderProgram.UseShader(); // need to install shader program object first
	shaderProgram.SetMat4("projection", scene.projection);
	shaderProgram.SetVec3("light.lightPos", scene.lights[0].pos);
	shaderProgram.SetVec3("light.lightCol", scene.lights[0].col);
	shaderProgram.SetVec3("material.diffuseCol", scene.models[0].material.diffuse);
	shaderProgram.SetVec3("material.ambientCol", scene.models[0].material.ambient);
	shaderProgram.SetVec3("material.specularCol", scene.models[0].material.specular);
	shaderProgram.SetFloat("material.shininess", scene.models[0].material.shininess);
	shaderProgram.SetVec3("cameraPos", camera.m_position);
	glm::mat3 modelNorm = glm::mat3(glm::transpose(glm::inverse(scene.models[0].modelMatrix)));
	shaderProgram.SetMat3("modelNorm", modelNorm);


	// render loop
	while (!glfwWindowShouldClose(window))
	{ 
		processInput(window);

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// active shader program
		shaderProgram.UseShader();
		scene.camera = camera.GetCameraMatrix();
		shaderProgram.SetMat4("camera", scene.camera);

		for (int i = 0; i < scene.models.size(); i++)
		{
			shaderProgram.SetMat4("model", scene.models[i].modelMatrix);
		}

		// bind VAO
		glBindVertexArray(VAO);
		// draw
		glDrawArrays(GL_TRIANGLES, 0, vboBuffer.size());
		//glDrawElements(GL_TRIANGLES, scene.meshMap.begin()->second.indices.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// terminte program
	glfwTerminate();
	//delete meshData;
	//meshData = nullptr;
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

	// camera movements
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastCursorX = xPos;
		lastCursorY = yPos;
		firstMouse = false;
	}
	double xOffset = xPos - lastCursorX;
	double yOffset = lastCursorY - yPos;
	lastCursorX = xPos;
	lastCursorY = yPos;

	// update camera matrix
	camera.processMouseMovement(xOffset, yOffset);
}