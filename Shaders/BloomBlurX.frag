#version 450 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D bloomSetup;

void main() {
    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    vec2 offset = 1.0 / textureSize(bloomSetup, 0);
    vec3 result = texture(bloomSetup, uv).rgb * weight[0]; 

    for(int i = 1; i < 5; ++i) {
        result += texture(bloomSetup, uv + vec2(offset.x * i, 0.0)).rgb * weight[i];
        result += texture(bloomSetup, uv - vec2(offset.x * i, 0.0)).rgb * weight[i];
    }

    outColor = vec4(result, 1.0);
}