#version 460 core
layout (location = 0) in vec4 aVertex;

out vec2 TexCoords;

uniform mat4 proj;

void main()
{
	gl_Position = proj * vec4(aVertex.xy, 0, 1);

	TexCoords = aVertex.zw;
}