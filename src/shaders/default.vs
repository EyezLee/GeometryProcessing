#version core 330
layout (location = 0) in vec3 pos;

void main()
{
	gl_position = vec4(pos, 1.0);
}