#version 450

#extension GL_KHR_vulkan_glsl : enable

vec2 positions[3] = vec2[](
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
);
layout(location=1)out vec4 colorInt;
layout(push_constant) uniform constants {
	mat4 model;
} ObjectData;

void main()
{
	gl_Position = ObjectData.model * vec4(positions[gl_VertexIndex], 0.0, 1.0);
	colorInt = vec4(positions[gl_VertexIndex], 0.25, 1.0);
}