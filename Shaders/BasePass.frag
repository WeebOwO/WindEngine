#version 450 core

layout(location = 0) in Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vin;

layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform PushConstant {
    uint materialIndex;
} pushConstant;

// layout(set = 0, binding = 1) uniform sampler textureSampler;
// layout(set = 1, binding = 0) uniform texture2D textureArray[128];

struct Material
{
    uint AlbedoTextureIndex;
    uint NormalTextureIndex;
    uint MetallicRoughnessTextureIndex;
    float RoughnessScale;
    float MetallicScale;
};

void main() {
    fragColor = vec4(vin.position, 1.0);  
}