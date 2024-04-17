#version 450

#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 colorInput;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) out vec4 colorInt;

layout(location = 5) in vec4 instance1x4;
layout(location = 6) in vec4 instance2x4;
layout(location = 7) in vec4 instance3x4;
layout(location = 8) in vec4 instance4x4;

layout(push_constant) uniform constants {
	mat4 trans;
	mat4 proj;

	mat4 pre;
} ObjectData;

void main()
{
    mat4 instanceMatrix = mat4(
        instance1x4,
        instance2x4,
        instance3x4,
        instance4x4
    );

	gl_Position = ObjectData.pre * instanceMatrix * vec4(vertexPosition, 1.0);
	colorInt = vec4(colorInput, 1.0);
}