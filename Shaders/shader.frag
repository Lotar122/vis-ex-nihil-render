#version 450

layout(location = 0) out vec4 color;
layout(location=2) in vec4 colorInt;

void main()
{
	color = colorInt;
}