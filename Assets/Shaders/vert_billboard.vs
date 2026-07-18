#version 330 core

layout(location = 0) in vec3 aPos;     // (-1..1 quad)
layout(location = 1) in vec3 aColour;
layout(location = 2) in vec2 aUV;

uniform mat4 uView;
uniform mat4 uProj;

uniform vec3 uBillboardPos;
uniform vec3 uCameraRight;
uniform vec3 uCameraUp;

uniform vec2 uSize;

out vec4 vColour;
out vec2 vUV;
out vec3 vPos;

void main()
{
    vec3 worldPos =
        uBillboardPos +
        uCameraRight * aPos.x * uSize.x +
        uCameraUp    * aPos.y * uSize.y;

    vColour = vec4(aColour, 1.0);
    vUV = aUV;
    vPos = aPos;
    gl_Position = uProj * uView * vec4(worldPos, 1.0);
}