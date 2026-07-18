#pragma once


#include <glad/glad.h>

typedef struct texture_t
{
  GLuint id;
  GLenum format, target;
  int w, h;
} texture_t;


