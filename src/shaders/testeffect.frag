#version 460 core

layout(location = 0) in vec2 texturePos;

layout(location = 0) uniform sampler2D renderTexture;

layout(location = 0) out vec4 fragColor;

void main() {
    // fragColor = vec4(0.4, 0.4, 0.4, 1.0);
    vec3 color = 1.0 - texture(renderTexture, texturePos).xyz;
    fragColor = vec4(color, 1.0);
}
