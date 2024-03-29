#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout(location = 0) out Vertex
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

layout(set = 0, binding = 1) uniform LightProjection {   
    mat4 viewproj;
} lightProjection;

void main() {
    vout.position = position;
    gl_Position = cameraData.viewproj * vec4(vout.position, 1.0);
    vout.texcoord = texCoord;
    vout.tangentBasis = mat3(tangent, bitangent, normal);
}