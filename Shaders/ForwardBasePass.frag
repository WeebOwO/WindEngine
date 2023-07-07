#version 450 core

layout(location=0) in Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vin;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(vin.position, 1.0);
}