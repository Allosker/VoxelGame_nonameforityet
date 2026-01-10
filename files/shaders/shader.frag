#version 460 core

in vec3 fragColor;

out vec4 FragColor;

void main()
{
	FragColor = vec4(fragColor, 0);
}