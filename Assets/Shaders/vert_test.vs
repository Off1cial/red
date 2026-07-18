#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColour;
layout (location = 3) in vec2 aUV;


out vec4 vColour;
out vec2 vUV;
out vec3 vNormal;
out vec3 vPos;

void main()
{
    gl_Position =
        vec4(aPos, 1.0);

    vColour = vec4(aColour, 1.0);
    vUV = aUV;
    vNormal = aNormal;
    vPos = aPos;
}
