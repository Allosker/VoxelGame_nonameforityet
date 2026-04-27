#version 460 core

in vec2 textureCoords;

uniform sampler2D basic_texture;

out vec4 FragColor;

void main()
{
	FragColor = texture(basic_texture, textureCoords);
}