#version 330 core

in vec2 vPos;
in vec2 vUV;
in vec4 vCol;

out vec4 FragColour;

uniform float uRounding;
uniform vec2 uRectPos;
uniform vec2 uRectSize;

float roundingSDF(vec2 centre, vec2 size, float radius)
{
  return length(max( abs(centre)-size+radius, 0.0)) - radius;
}

void main()
{


  FragColour = vCol;
}

