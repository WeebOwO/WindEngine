#version 450 

const float gamma = 2.2;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D sceneColor;

void main() {
    vec3 color = texture(sceneColor, uv).xyz;
  
    outColor = vec4(pow(color, vec3(1.0 / gamma)), 1.0);
}