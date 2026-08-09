#pragma once


#include <glad/glad.h>

typedef struct surface_s surface_t;

typedef struct texture_s
{
  GLuint gltexnum;
  GLenum format, target;
  int w, h;

  surface_t* surfchain; // First surface of this texture
} texture_t;


