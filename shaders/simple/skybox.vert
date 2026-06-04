#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 proj;
uniform mat4 view;

out vec3 texDir;

void main()
{
	texDir = aPos;

	mat4 v = mat4(mat3(view));

	gl_Position = proj * v * model * vec4(aPos, 1.0);
}
