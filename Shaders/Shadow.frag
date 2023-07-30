#version 450 core

// do nothing for shadow frag
layout(location = 0) out vec4 color;

void main() {
    color = vec4(1.0, 0.0, 0.0, 0.0);
}