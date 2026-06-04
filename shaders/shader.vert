#version 460 core
#extension GL_NV_gpu_shader5 : require

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTextureCoords;
layout(location = 2) in float aShadowAO;
layout(location = 3) in uint8_t aBlocklight;
layout(location = 4) in uint8_t aSunlight;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 textureCoords;
out float shadowAO;

flat out uint8_t blocklight;
flat out uint8_t sunlight;


void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);

	textureCoords = aTextureCoords;
	shadowAO = aShadowAO;

	blocklight = aBlocklight;
	sunlight = aSunlight;
}