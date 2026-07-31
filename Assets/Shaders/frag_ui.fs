#version 330 core

in vec2 vPos;
in vec2 vUV;
flat in uint vCol;

out vec4 FragColour;
uniform sampler2D uTexture;
uniform int uUseTexture; // 0 = solid colour (rects), 1 = sample atlas (text)

vec4 UnpackRGBA(uint c)
{
    return vec4(
        float((c >> 0u) & 0xFFu),
        float((c >> 8u) & 0xFFu),
        float((c >> 16u) & 0xFFu),
        float((c >> 24u) & 0xFFu)
    ) / 255.0;
}

void main()
{
  vec4 unpacked = UnpackRGBA(vCol);
    if (uUseTexture == 1)
    {
        float coverage = texture(uTexture, vUV).a; // glyph coverage from alpha channel
        FragColour = vec4(unpacked.rgb, unpacked.a * coverage);
    }
    else
    {
        FragColour = unpacked;
    }
}
