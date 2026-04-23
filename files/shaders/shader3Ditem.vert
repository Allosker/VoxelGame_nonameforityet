#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUvs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 Uvs;

void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);

	Uvs = aUvs;
}