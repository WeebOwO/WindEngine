#version 450 core

layout(location = 0) in Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vin;

layout(location = 0) out vec4 fragColor;

struct Material
{
    uint AlbedoTextureIndex;
    uint NormalTextureIndex;
    uint MetallicRoughnessTextureIndex;
    float RoughnessScale;
    float MetallicScale;
};

void main() {
    fragColor = vec4(1.0);  
}