#version 450 core

// use this for tonemap
const float gamma     = 2.2;
const float exposure  = 1.0;
const float pureWhite = 1.0;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D sceneColor;

void main() {
    vec3 color = texture(sceneColor, uv).xyz;
    float luminance = dot(color.xyz, vec3(0.2126, 0.7152, 0.0722));
	float mappedLuminance = (luminance * (1.0 + luminance / (pureWhite * pureWhite))) / (1.0 + luminance);
	
	vec3 mappedColor = (mappedLuminance / luminance) * color;
    outColor = vec4(pow(mappedColor, vec3(1.0 / gamma)), 1.0);
}   