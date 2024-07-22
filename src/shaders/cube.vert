#version 460 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 iOffset;

layout(location = 0) uniform mat4 projection_view;

layout(location = 0) out vec3 vertexPosition;
layout(location = 1) out vec3 vertexColor;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 rand3(vec3 co) {
    return vec3(
        rand(co.yz + co.zx),
        rand(co.xz + co.zx),
        rand(co.xy + co.zy)
    );
}

void main() {
    gl_Position = projection_view * vec4(vPos + iOffset, 1.0);
    vertexColor = normalize(vec3(0.6, 0.6, 1.0) * 2.0 + rand3(iOffset));
    vertexPosition = vPos + iOffset;
}