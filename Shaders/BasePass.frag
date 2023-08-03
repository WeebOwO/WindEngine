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

layout(set = 0, binding = 2) uniform sampler textureSampler;
layout(set = 0, binding = 3) uniform MaterialBuffer {
    Material materials[256];
};

layout(set = 0, binding = 4) uniform PlaneDistance {
    float zNear;
    float zFar;
} planeDistance;

layout(set = 1, binding = 0) uniform texture2D textureArray[75];

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * planeDistance.zNear * planeDistance.zFar) / (planeDistance.zFar + planeDistance.zNear - z * (planeDistance.zFar - planeDistance.zNear));
}

void main() {
    Material material = materials[pushConstant.materialIndex];
    vec4 albedoColor = texture(sampler2D(textureArray[material.albedoTextureIndex], textureSampler), vin.texcoord);  
    vec3 normal = texture(sampler2D(textureArray[material.normalTextureIndex], textureSampler), vin.texcoord).rgb;  
    normal = vin.tangentBasis * (2 * normal - 1.0);
    vec4 metallicRoughness = texture(sampler2D(textureArray[material.metallicRoughnessTextureIndex], textureSampler), vin.texcoord);     

    gbufferA = vec4(vin.position, LinearizeDepth(gl_FragCoord.z));
    gbufferB = vec4(normal, 1.0);
    gbufferC = albedoColor;
    gbufferD = metallicRoughness;
}