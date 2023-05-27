#version 450

layout(location = 0) out vec3 fragColor;

layout(location = 0) in vec3 position;


void main() {
    gl_Position = vec4(position, 1.0);
    fragColor = vec3(1.0);
}