#version 460 core
layout(location = 0) in vec3 aOffset;
layout(location = 1) in vec3 aColor;
//layout(location = 2) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragColor;

void main()
{
	gl_Position = proj * view * model * vec4(/*aPos +*/ aOffset , 1.0);

	fragColor = aColor;
}