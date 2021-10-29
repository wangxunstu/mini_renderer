#version 330 core

layout(location = 0) in vec4 position;
layout (location = 1) in vec4 texcoord;

out vec2 Texcoord;

void main()
{
	Texcoord=texcoord.xy;
	gl_Position=position;
}