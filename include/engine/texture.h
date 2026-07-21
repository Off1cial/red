#pragma once


#include <glad/glad.h>

typedef struct texture_s
{
  GLuint gltexnum;
  GLenum format, target;
  int w, h;


} texture_t;


