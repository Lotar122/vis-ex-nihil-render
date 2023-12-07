#version 450

#extension GL_KHR_vulkan_glsl : enable

vec2 positions[3] = vec2[](
	vec2(0.00, -0.05),
	vec2(0.05, 0.05),
	vec2(-0.05, 0.05)
);
vec3 color[3] = vec3[](
	vec3(1, 0, 0),
	vec3(0, 1, 0),
	vec3(0, 0, 1)
);
layout(location=1)out vec4 colorInt;
layout(push_constant) uniform constants {
	mat4 model;
	float time;
} ObjectData;
float cosv = cos(ObjectData.time);
float sinv = sin(ObjectData.time);
float sinmv = sin(ObjectData.time * 1.3456);
float cosmv = cos(ObjectData.time * 1.3456);
void main()
{
	
	color[0] = vec3(sinv, cosv, sinmv);
	color[1] = vec3(cosv, sinv, sinmv);
	color[2] = vec3(cosv, cosmv, cosmv);
	gl_Position = ObjectData.model * vec4(positions[gl_VertexIndex], 0.0, 1.0);
	colorInt = vec4(color[gl_VertexIndex], 1.0);
}