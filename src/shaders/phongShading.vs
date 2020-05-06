#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

uniform mat4 projection;
uniform mat4 camera;
uniform mat4 model;
uniform mat3 modelNorm;

out vec3 FragPos;
out vec3 normal;

void main()
{
	gl_Position = projection *  camera * model * vec4(pos, 1.0f);
	FragPos = vec3(model * vec4(pos, 1.0));
	normal = normalize(modelNorm * norm);
}