#version 450 core

struct Material {
	vec3 albedo;
	vec3 normal;
	float metallic;
	float roughness;
};

layout(location=0) in Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vin;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform LightBuffer {
    vec3 lightDirection;
	vec3 lightIntensity;
	vec3 lightColor;
} lightData;

layout(set = 1, binding = 0) uniform sampler2D albedoTexture;
layout(set = 1, binding = 1) uniform sampler2D normalTexture;
layout(set = 1, binding = 2) uniform sampler2D metallicTexture;
layout(set = 1, binding = 3) uniform sampler2D roughnessTexture;
layout(set = 1, binding = 4) uniform samplerCube iblIrradianceTexture;
layout(set = 1, binding = 5) uniform sampler2D iblSpecBrdfLut;

void main() {
	Material material;
	vec2 envBRDF = texture(iblSpecBrdfLut, vin.texcoord).rg;
	vec3 lightDir = normalize(lightData.lightDirection);
    material.albedo = texture(albedoTexture, vin.texcoord).rgb;
	vec3 normal = normalize(2.0 * texture(normalTexture, vin.texcoord).rgb - 1.0);
	material.normal = normalize(vin.tangentBasis * normal);
	material.metallic = texture(metallicTexture, vin.texcoord).r;
	material.roughness = texture(roughnessTexture, vin.texcoord).r;
	
	outColor = vec4(material.albedo * max(dot(lightDir, material.normal), 0), 1.0);
}