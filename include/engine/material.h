#pragma once

#include "corebase/texture.h"

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
  texture_t* texture;

} material_t;
