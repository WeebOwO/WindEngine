#version 450 core

const float PI = 3.141592;
const float Epsilon = 0.00001;
const vec3 Fdielectric = vec3(0.04);

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
	vec3 lightRadiance;
	vec3 lightColor;
} lightData;

layout(set = 0, binding = 0) uniform CameraBuffer {   
    mat4 view;
    mat4 proj;
	mat4 viewproj;
	vec3 viewPos;
} cameraData;

layout(set = 1, binding = 0) uniform sampler2D albedoTexture;
layout(set = 1, binding = 1) uniform sampler2D normalTexture;
layout(set = 1, binding = 2) uniform sampler2D metallicTexture;
layout(set = 1, binding = 3) uniform sampler2D roughnessTexture;
layout(set = 1, binding = 4) uniform samplerCube iblIrradianceTexture;
layout(set = 1, binding = 5) uniform sampler2D iblSpecBrdfLut;

// GGX/Towbridge-Reitz normal distribution function
// Term N
float NdfGGX(float cosLh, float roughness) {
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
// Term G
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
	Material material;
	// Setup Material
	vec3 normal = normalize(2.0 * texture(normalTexture, vin.texcoord).rgb - 1.0);

	material.albedo = texture(albedoTexture, vin.texcoord).rgb;
	material.normal = normalize(vin.tangentBasis * normal);
	material.metallic = texture(metallicTexture, vin.texcoord).r;
	material.roughness = texture(roughnessTexture, vin.texcoord).r;
	
	vec3 eyePosition = cameraData.viewPos;

	// outgoing light and reflection light
	vec3 lo = normalize(eyePosition - vin.position);
	float cosLo = max(0.0, dot(material.normal, lo));
	vec3 lr = 2.0 * cosLo * material.normal - lo;


	// // direct light part 
	vec3 directLighting = vec3(0);
	vec3 f0 = mix(Fdielectric, material.albedo, material.metallic);
	
	vec3 li = -lightData.lightDirection;
	// vec3 lightradiance = lightData.lightRadiance;

	// vec3 lh = normalize(li + lo);

	// float cosLi = max(0.0, dot(material.normal, li));
	// float cosLh = max(0.0, dot(material.normal, lh));

	// vec3 f = fresnelSchlick(f0, max(0.0, dot(lh, lo)));
	// float d = NdfGGX(cosLh, material.roughness);
	// float g = gaSchlickGGX(cosLi, cosLo, material.roughness);

	outColor = vec4(directLighting, 1.0);
}