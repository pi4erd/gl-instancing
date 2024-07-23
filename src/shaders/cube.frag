#version 460 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;

layout(location = 0) out vec4 fragColor;

void main() {
    // vec3 color = vertexColor * clamp(dot(normalize(vec3(1)), normalize(vertexNormal)), 0.02, 1.0);
    fragColor = vec4(vertexColor, 1.0);
}
