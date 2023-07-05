#version 450

layout (location = 0) in vec3 position;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 localPos;

layout(set = 0, binding = 0) uniform CameraBuffer {   
    mat4 view;
    mat4 proj;
	mat4 viewproj;
} cameraData;

layout(set = 0, binding = 1) uniform ObjectBuffer {
    mat4 model;
} objectData;

void main() {
    localPos = position;
    gl_Position = cameraData.viewproj * vec4(position, 1.0);
    fragColor = vec3(1.0, 1.0, 1.0);
}