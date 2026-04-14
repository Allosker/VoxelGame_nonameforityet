#version 460 core

in vec2 UVs;

uniform sampler2D basic_texture;

out vec4 FragColor;

void main()
{
	FragColor = texture(basic_texture, UVs / textureSize(basic_texture, 0));
}