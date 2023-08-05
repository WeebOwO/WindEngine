#version 450 core

layout (location = 0) in vec2 uv;

const float PI = 3.141592;
const float Epsilon = 0.00001;
const vec3 Fdielectric = vec3(0.04);

const float pointlightConst = 1.0;
const float pointlightLinear = 0.09;
const float pointlightQuat = 0.032;

const int MAX_LIGHT = 64;

struct Material {
    vec3 position;
	vec3 albedo;
	vec3 normal;
	float metallic;
	float roughness;
};

struct PointLight {
	vec3 position;
	vec3 intensity;
	vec3 lightColor;
};

layout(push_constant) uniform PushConstant {
    uint pointLightCnts;
} pushConstant;

layout(set = 0, binding = 0) uniform CameraBuffer {   
    mat4 view;
    mat4 proj;
	mat4 viewproj;
	vec3 viewPos;
} cameraData;

layout (set = 0, binding = 1) uniform Sun {
	vec3 lightPos;
    vec3 lightDirection;
	vec3 lightRadiance;
	vec3 lightColor;
} sun;

layout (set = 0, binding = 2) uniform PointLights {
	PointLight lights[MAX_LIGHT];
} pointLightArray;

layout(set = 0, binding = 3) uniform LightProjection {   
    mat4 viewproj;
} lightProjection;

layout (set = 1, binding = 0) uniform sampler2D gbufferA;
layout (set = 1, binding = 1) uniform sampler2D gbufferB;
layout (set = 1, binding = 2) uniform sampler2D gbufferC;
layout (set = 1, binding = 3) uniform sampler2D gbufferD;
layout(set = 1, binding = 4) uniform samplerCube iblIrradianceTexture;
layout(set = 1, binding = 5) uniform sampler2D iblSpecBrdfLut;
layout(set = 1, binding = 6) uniform samplerCube iblSepcTexture;
layout(set = 1, binding = 7)uniform sampler2D shadowMap;

layout (location = 0) out vec4 sceneColor;

float ShadowCalculation(vec4 fragPosLightSpace, Material material) {
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords.xy = projCoords.xy * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r; 
	float currentDepth = projCoords.z;

	float bias = 0.05;

	// pcf
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }

    shadow /= 9.0;	
	return shadow;
}

void main() {
    // init material property
    Material material;

    material.albedo = texture(gbufferC, uv).rgb;
    material.position = texture(gbufferA, uv).rgb;
    material.normal = texture(gbufferB, uv).rgb;
    material.metallic = 1 - texture(gbufferD, uv).b;
    material.roughness = texture(gbufferD, uv).g;

	vec4 fragPosLightSpace = lightProjection.viewproj * vec4(material.position, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords.xy = projCoords.xy * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r; 
	float currentDepth = projCoords.z;

	vec3 eyePosition = cameraData.viewPos;	

	vec3 f0 = mix(Fdielectric, material.albedo, material.metallic);

	vec3 lo = normalize(eyePosition - material.position);
	vec3 li = normalize(sun.lightDirection);
	vec3 lh = normalize(li + lo);
	
	float cosLN = dot(li, material.normal);

	vec3 kd = f0 / PI;
	// direct light part
	vec3 diffuse = kd * max(cosLN, 0) * sun.lightColor;
	vec3 ambilent = material.albedo * 0.1 * sun.lightColor;
	float ks = pow(max(dot(lh, material.normal), 0), 32);
	vec3 spec = ks * sun.lightColor * material.albedo;

	float shadowMask = ShadowCalculation(fragPosLightSpace, material);
	
	vec3 color = ambilent + (1 - shadowMask) * (spec + diffuse);

	// point light part
	vec3 pointLightResult = vec3(0.0);

	for(int i = 0; i < pushConstant.pointLightCnts; ++i) {
		PointLight light = pointLightArray.lights[i];
		float dis = length(light.position - material.position);
	
		float attenuation = 1.0 / (pointlightConst + pointlightLinear * dis + 
                pointlightQuat * (dis * dis));
		vec3 lightDirection = normalize(light.position - material.position);
		float pointCosLN = max(dot(lightDirection, material.normal), 0);

		vec3 pointDiffuse = pointCosLN * light.lightColor * kd * attenuation * light.intensity;
		pointLightResult += pointDiffuse;
	}

	sceneColor = vec4(pointLightResult, 1.0);
}