#version 330 core

in vec2 vPos;
in vec2 vUV;
in vec4 vCol;

out vec4 FragColour;

uniform sampler2D uTexture;
uniform int uUseTexture; // 0 = solid colour (rects), 1 = sample atlas (text)

void main()
{
    if (uUseTexture == 1)
    {
        float coverage = texture(uTexture, vUV).a; // glyph coverage from alpha channel
        FragColour = vec4(vCol.rgb, vCol.a * coverage);
    }
    else
    {
        FragColour = vCol;
    }
}