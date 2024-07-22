#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = vec4(vertexColor, 1.0);
}
