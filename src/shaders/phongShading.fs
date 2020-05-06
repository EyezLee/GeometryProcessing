#version 330 core

in vec3 normal;
in vec3 FragPos;

uniform vec3 diffuseCol;
uniform vec3 ambientCol;
uniform vec3 specularCol;
uniform float shininess;

uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 cameraPos;

out vec4 FragColor;

void main()
{
	// diffuse
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = diff * diffuseCol;

	// specular
	vec3 lightRef = reflect(-lightDir, normal); // light relect
	vec3 eyeDir = normalize(cameraPos - FragPos);
	float spec = pow(max(dot(eyeDir, lightRef), 0.0f), 1/shininess);
	vec3 specular = spec * specularCol;

	// phong shading formula
	vec3 color = (ambientCol + diffuse + specular) * lightCol; 
	FragColor = vec4(color, 1.0f);
}