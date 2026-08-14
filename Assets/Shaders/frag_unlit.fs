#version 330 core

in vec4 vColour;
in vec2 vUV;
in vec3 vPos;

out vec4 FragColour;

uniform vec4 uColour;
uniform bool uUseTexture;
uniform bool uUseVertexColour;
uniform sampler2D uTexture;

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
    vec4 finalColour = uColour;

    //if (uUseVertexColour){
        //finalColour *= vColour;
    //}

 
    vec4 texColour = vec4(0.5);

    if (uUseTexture){
        texColour = texture(uTexture, vUV);
    }

    //FragColour = finalColour * texColour;
    //FragColour = texColour;


    //FragColour = uColour * texColour;
    FragColour = texture(uTexture, vUV) * vColour;
    

    //FragColour = vec4(vUV, 0.0, 1.0);

    //FragColour = vColour;
}
