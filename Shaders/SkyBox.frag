#version 450 core

layout(location = 0) in vec3 viewPos;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform samplerCube SkyboxCubemap;

void main() {
    outColor = vec4(texture(SkyboxCubemap, normalize(viewPos)).rgb, 1.0);
}