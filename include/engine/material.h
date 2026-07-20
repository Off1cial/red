#pragma once

#include "engine/texture.h"
#include "platform/common.h"

// Material flags
#define MATERIAL_USE_TEXTURE  1
#define MATERIAL_USE_COLBLEND 2
#define MATERIAL_USE_ALPHA    4
#define MATERIAL_USE_NORMAL   8
#define MATERIAL_USE_LIGHTING 16
#define MATERIAL_USE_SPECULAR 32

#define MATERIAL_FLAG_COUNT 5


typedef struct material_t
{
  float col[4]; 
  lbyte flags;

  texture_t* base;
  texture_t* normal;


  // Animation info
  struct texture_s *anim_next; // next texture in sequence
  int anim_count; // 0 if no anim


} material_t

