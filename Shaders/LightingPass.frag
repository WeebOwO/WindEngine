#version 450 core

layout (location = 0) in vec2 uv;

layout (set = 0, binding = 0) uniform sampler2D gbufferA;
layout (set = 0, binding = 1) uniform sampler2D gbufferB;
layout (set = 0, binding = 2) uniform sampler2D gbufferC;
layout (set = 0, binding = 3) uniform sampler2D gbufferD;

layout (location = 0) out vec4 sceneColor;

void main() {
    sceneColor = texture(gbufferC, uv);
}