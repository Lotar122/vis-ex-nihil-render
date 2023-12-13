#version 450

#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 colorInput;
layout(location = 2)out vec4 colorInt;
layout(push_constant) uniform constants {
	mat4 model;
	float time;
} ObjectData;
void main()
{
	gl_Position = ObjectData.model * vec4(vertexPosition, 1.0);
	colorInt = vec4(colorInput, 1.0);
}