#version 450 core

layout(location = 0) in Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vin;

layout(location = 0) out vec4 gbufferA;
layout(location = 1) out vec4 gbufferB;
layout(location = 2) out vec4 gbufferC;
layout(location = 3) out vec4 gbufferD;

layout(push_constant) uniform PushConstant {
    uint materialIndex;
} pushConstant;

struct Material
{   
    uint albedoTextureIndex;
    uint normalTextureIndex;
    uint metallicRoughnessTextureIndex;
    float roughnessScale;
    float metallicScale;
};

layout(set = 0, binding = 1) uniform sampler textureSampler;
layout(set = 0, binding = 2) uniform MaterialBuffer {
    Material materials[256];
}; 

layout(set = 1, binding = 0) uniform texture2D textureArray[75];

void main() {
    Material material = materials[pushConstant.materialIndex];
    vec4 albedoColor = texture(sampler2D(textureArray[material.albedoTextureIndex], textureSampler), vin.texcoord);  
    vec4 normal = texture(sampler2D(textureArray[material.normalTextureIndex], textureSampler), vin.texcoord);  
    vec4 metallicRoughness = texture(sampler2D(textureArray[material.metallicRoughnessTextureIndex], textureSampler), vin.texcoord);     

    gbufferA = vec4(vin.position, 1);
    gbufferB = normal;
    gbufferC = albedoColor;
    gbufferD = metallicRoughness;
}