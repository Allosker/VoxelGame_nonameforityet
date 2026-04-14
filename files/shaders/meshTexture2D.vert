#version 460 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUVs;

uniform mat4 model;
uniform mat4 proj;

out vec2 UVs;

void main()
{
	gl_Position = proj * model * vec4(aPos.xy, 0.0, 1.0);
	UVs = aUVs;
}