#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout(set = 0, binding = 0) uniform LightProjection {   
    mat4 viewproj;
} lightProjection;

void main() {
    gl_Position = lightProjection.viewproj * vec4(position, 1.0f);
}