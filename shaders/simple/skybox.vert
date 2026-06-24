#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 vp;
uniform vec3 cameraPos;

out vec3 texDir;

void main()
{
	texDir = aPos;

	vec4 pos = vp * vec4(aPos + cameraPos, 1.0);

	gl_Position = pos.xyww;
}
