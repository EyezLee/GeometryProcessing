#ifndef SCENE_H
#define SCENE_H

#include <vector>
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

using mesh_t = vector<glm::vec3>;

struct Model
{
	mesh_t* meshSource;
	Material material;
	glm::mat4 modelMatrix;
};

// type alias
using light_t = vector<Light>;
using model_t = vector<Model>;

struct Scene
{
	string scenePath;
	glm::mat4 camera;
	glm::mat4 projection;
	light_t lights;
	model_t models;
};

#endif // !SCENE_H
