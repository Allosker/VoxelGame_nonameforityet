#version 460 core
#extension GL_NV_gpu_shader5 : require

in vec2 textureCoords;
in float shadowAO;

in float sunlight;
in vec4 blocklight;

uniform sampler2D basic_texture;

out vec4 FragColor;

void main()
{
	vec4 color = texture(basic_texture, textureCoords);


	FragColor = clamp(color * (blocklight + sunlight), 0., 1.) * shadowAO;

}