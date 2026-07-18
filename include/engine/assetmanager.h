#pragma once

#include "platform/common.h"
// Asset manager
//

#include "corebase/texture.h"

typedef enum {
  ASSET_TEXTURE = 0,
  ASSET_MATERIAL,
  ASSET_SHADER,
  ASSET_MODEL,
  ASSET_AUDIO
} asset_type_t;
  
typedef struct CBASE_ASSET_T
{
  byte type; 
  string fpath;

} CBaseAsset;


// Texture.c

void CBaseAsset_TextureLoad(const char* filepath, texture_t* destination);
void CBaseAssset_TextureDestroy(texture_t* texture);

// Material.c



