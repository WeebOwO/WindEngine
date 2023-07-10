#version 450 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec2 texcoord;

layout(location=0) out Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vout;

layout(set = 0, binding = 0) uniform CameraBuffer {   
    mat4 view;
    mat4 proj;
	mat4 viewproj;
	vec3 viewPos;
} cameraData;

layout(set = 0, binding = 1) uniform ObjectBuffer {
    mat4 model;
} objectData;

void main() {
    vout.position = (objectData.model * vec4(position, 1.0)).xyz;
	vout.texcoord = vec2(texcoord.x, 1.0-texcoord.y);

	vout.tangentBasis = mat3(tangent, bitangent, normal);

	gl_Position = cameraData.viewproj * vec4(position, 1.0);
}