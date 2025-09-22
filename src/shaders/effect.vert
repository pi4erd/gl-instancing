#version 410 core

layout (location = 0) in vec2 vertLoc;
layout (location = 1) in vec2 texPos;

layout (location = 0) out vec2 texturePos;

void main() {
    gl_Position = vec4(vertLoc, 0.0, 1.0);
    texturePos = texPos;
}
