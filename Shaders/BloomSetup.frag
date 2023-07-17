#version 450 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D sceneColor;

void main() {
    vec4 color = texture(sceneColor, uv);
    float threshold = 0.75;
    float l = dot(color.xyz, vec3(0.2126, 0.7152, 0.0722));
    outColor.rgb = (l > threshold) ? color.rgb : vec3(0.0);
    outColor.a = 1.0;
}