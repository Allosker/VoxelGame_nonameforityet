#version 460 core
in vec3 texDir;
uniform samplerCube cubemap;

out vec4 FragColor;

void main()
{
	FragColor = texture(cubemap, texDir);
}