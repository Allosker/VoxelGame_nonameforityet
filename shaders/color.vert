#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 model;
uniform mat4 vp;

out vec3 Color;

void main()
{
	gl_Position = vp * model * vec4(aPos, 1.0);
	Color = aColor;
}