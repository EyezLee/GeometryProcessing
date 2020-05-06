#version 330 core

in vec3 normal;
in vec3 FragPos;

uniform vec3 diffuseCol;
uniform vec3 lightPos;

out vec4 FragColor;

void main()
{
	vec3 lightDir = normalize(FragPos - lightPos);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * diffuseCol;
	FragColor = vec4(normal, 1.0);
}