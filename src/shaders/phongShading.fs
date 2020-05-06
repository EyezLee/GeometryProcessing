#version 330 core

in vec3 normal;
in vec3 FragPos;

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

out vec4 FragColor;

void main()
{
	// diffuse
	vec3 lightDir = normalize(light.lightPos - FragPos);
	float diff = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = diff * material.diffuseCol;

	// specular
	vec3 lightRef = reflect(-lightDir, normal); // light relect
	vec3 eyeDir = normalize(cameraPos - FragPos);
	float spec = pow(max(dot(eyeDir, lightRef), 0.0f), 1/material.shininess);
	vec3 specular = spec * material.specularCol;

	// phong shading formula
	vec3 color = (material.ambientCol + diffuse + specular) * light.lightCol; 
	FragColor = vec4(color, 1.0f);
}