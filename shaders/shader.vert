#version 460 core
#extension GL_NV_gpu_shader5 : require

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTextureCoords;
layout(location = 2) in float aShadowAO;
layout(location = 3) in uint16_t aLight;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 textureCoords;
out float shadowAO;
out float sunlight;
out vec4 blocklight;


void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);

	textureCoords = aTextureCoords;
	shadowAO = aShadowAO;


	float r = float(int(aLight >> 8) & 0xF) / 15.;
	float g = float(int(aLight >> 4) & 0xF) / 15.;
	float b = float(int(aLight     ) & 0xF) / 15.;

	float asunlight = float(int(aLight >> 12) & 0xF) / 15.;

	vec4 ablocklight = vec4(r,g,b, 1.);

	sunlight = asunlight;
	blocklight = ablocklight;
}