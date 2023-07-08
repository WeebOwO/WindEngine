#version 450 core

layout(location=0) in Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vin;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform LightBuffer {
    vec3 lightDirection;
	float light;
} lightData;

layout(set = 1, binding = 0) uniform sampler2D albedoTexture;
layout(set = 1, binding = 1) uniform sampler2D normalTexture;
layout(set = 1, binding = 2) uniform sampler2D metalnessTexture;
layout(set = 1, binding = 3) uniform sampler2D roughnessTexture;

void main() {
    vec3 albedo = texture(albedoTexture, vin.texcoord).rgb;
	vec3 normal = normalize(2.0 * texture(normalTexture, vin.texcoord).rgb - 1.0);
	normal = normalize(vin.tangentBasis * normal);
	float metalness = texture(metalnessTexture, vin.texcoord).r;
	float roughness = texture(roughnessTexture, vin.texcoord).r;

	outColor = vec4(albedo, 1.0);
}