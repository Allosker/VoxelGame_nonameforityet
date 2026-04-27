#version 460 core

in vec2 Uvs;

uniform sampler2D basicTexture;

out vec4 FragColor;

void main()
{
	vec4 temp = texture(basicTexture, Uvs / textureSize(basicTexture, 0));

	if (temp.a < 0.1)
		discard;
	
	FragColor = temp;
}