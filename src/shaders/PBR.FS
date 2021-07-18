#version 330

in vec3 worldPos;
in vec3 normal;

struct Light
{
	vec3 lightCol;
	vec3 lightPos;
};
uniform Light light;

out vec4 FragColor;

void main()
{
	vec4 col = vec4(light.lightCol, 1);
	FragColor = col;
}