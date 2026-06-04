#version 460 core
#extension GL_NV_gpu_shader5 : require

in vec2 textureCoords;
in float shadowAO;
flat in uint8_t blocklight;
flat in uint8_t sunlight;

uniform sampler2D basic_texture;

out vec4 FragColor;

void main()
{
	float real_blocklight = float(blocklight) / 15.;
	float real_sunlight = float(sunlight) / 15.;

	vec4 color = texture(basic_texture, textureCoords) * shadowAO;


	FragColor = max(color * real_blocklight, color * real_sunlight);

}