#version 450

layout (location = 0) in vec3 position;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform CameraBuffer {   
    mat4 view;
    mat4 proj;
	mat4 viewproj;
} cameraData;

layout(set = 0, binding = 1) uniform ObjectBuffer {
    mat4 model;
} objectData;

void main() {
    mat4 v = cameraData.view;
    gl_Position = vec4(position, 1.0);
    fragColor = vec3(1.0, 1.0, 1.0);
}