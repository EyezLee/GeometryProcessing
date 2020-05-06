#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

struct Light
{
	glm::vec3 pos;
	glm::vec3 col;
	float atten_k;
};

struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

struct Mesh
{
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<GLuint> indices;
	vector<GLuint> normIndices;
	vector<glm::vec2> texCoords;
};

struct Model
{
	//string name;
	Mesh* meshSource;
	Material material;
	glm::mat4 modelMatrix;
};

// type alias
using light_t = vector<Light>;
using model_t = vector<Model>;
using mesh_map = std::map<string, Mesh>;

struct Scene
{
	string scenePath;
	//Mesh mesh;
	mesh_map meshMap;
	glm::mat4 camera;
	glm::mat4 projection;
	light_t lights;
	model_t models;
};

#endif // !SCENE_H
