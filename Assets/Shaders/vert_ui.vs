#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in uint aCol;

out vec2 vPos;
out vec2 vUV;
flat out uint vCol;

uniform vec2 uScreenSize;

vec2 ScreenToNDC(vec2 p)
{
    return vec2(
        (p.x / uScreenSize.x) * 2.0 - 1.0,
        1.0 - (p.y / uScreenSize.y) * 2.0
    );
}



void main()
{
  vec2 ndcpos = ScreenToNDC(aPos);
    gl_Position =
        vec4(ndcpos, 0.0, 1.0);

    vCol = aCol;
    vUV = aUV;
    vPos = ndcpos;
}
