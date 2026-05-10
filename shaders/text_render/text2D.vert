#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aColor;

out vec2 TexCoords;

uniform mat4 proj;

void main()
{
	gl_Position = proj * vec4(aPos, 0, 1);

	TexCoords = aColor;
}