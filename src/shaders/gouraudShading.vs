#version 330 core 
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

uniform mat4 projection;
uniform mat4 camera;
uniform mat4 model;
uniform mat3 modelNorm;

struct Material
{
	vec3 diffuseCol;
	vec3 ambientCol;
	vec3 specularCol;
	float shininess;
};

struct Light
{
	vec3 lightCol;
	vec3 lightPos;
};

uniform Material material;
uniform Light light;

uniform vec3 cameraPos;

out vec3 normal;

void main()
{
	gl_Position = projection *  camera * model * vec4(pos, 1.0f);
	normal = norm;
}