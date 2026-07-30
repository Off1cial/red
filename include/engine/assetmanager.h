#pragma once
#include <stdint.h>

#include <glad/glad.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "engine/shader.h"
#include "engine/mesh.h"

#define ASSETS_MAX_TEXTURES 256
#define ASSETS_MAX_MATERIALS 512
#define ASSETS_MAX_SHADERS 64
#define ASSETS_MAX_FONT_GLYPHS 128
#define ASSETS_MAX_FONTS 64
#define ASSETS_FONT_HASH_BUCKETS 64

typedef enum
{
  ASSET_UNLOADED,
  ASSET_LOADING,
  ASSET_LOADED,
  ASSET_FAILED
} assetstate_t;

typedef struct assethandle_t
{
  uint32_t index;
  uint32_t generation;
} assethandle_t;

typedef struct assetTexture_t
{
  char path[256];
  assethandle_t handle;
  GLuint texid;
  int width, height;
  assetstate_t state;
} assetTexture_t;

struct _texregistry
{
  assetTexture_t tex[ASSETS_MAX_TEXTURES];
  uint32_t count;
};

typedef struct assetMaterial_t
{
  char path[256];
  assethandle_t handle;
  assetstate_t state;

  assethandle_t shader;
  assethandle_t albedo;
  assethandle_t normal;
} assetMaterial_t;

struct _materialregistry
{
  assetMaterial_t mats[ASSETS_MAX_MATERIALS];
  uint32_t count;
};

typedef struct assetShader_t
{
  char path[256];
  assethandle_t handle;
  CBaseShader *shader;
  assetstate_t state;
} assetShader_t;

struct _shaderregistry
{
  assetShader_t shaders[ASSETS_MAX_SHADERS];
  uint32_t count;
};

struct _fontglyph
{
  float uv0[2];
  float uv1[2];
  float bearing[2];
  float advance;
  int w, h;
};

typedef struct assetFont_t
{
  char path[256];
  assetstate_t state;
  struct _fontglyph glyphs[ASSETS_MAX_FONT_GLYPHS];
  assethandle_t handle;
  int tex_index;
  int size;
  float lineheight;
  int ascent;

  uint32_t hash;
  struct assetFont_t *next; // hash bucket chain, see fontHashTable
} assetFont_t;

struct _fontregistry
{
  struct assetFont_t fonts[ASSETS_MAX_FONTS];
  uint32_t count;
};

typedef struct assetMesh_t
{
  gpuVertex *vertices;
  size_t vertexcount;
} assetMesh_t;

typedef struct AssetManager
{
  struct _texregistry textures;
  struct _shaderregistry shaders;
  struct _materialregistry materials;
  struct _fontregistry fonts;

  assetFont_t *fontHashTable[ASSETS_FONT_HASH_BUCKETS];
} CBaseAssetManager;

extern CBaseAssetManager *gAssetManager;

uint8_t AssetManager_AddFont(const char *path, int size);
uint32_t AssetManager_AddTexture(const char *path);
uint32_t AssetManager_GetFontID(const char *name);

uint8_t AssetManager_Init();

static inline assetFont_t *AssetManager_GetFont(uint32_t fontid)
{
  if (fontid >= gAssetManager->fonts.count)
    return NULL;
  return &gAssetManager->fonts.fonts[fontid];
}

static inline assetTexture_t *AssetManager_GetTexture(uint32_t textureid)
{
  if (textureid >= gAssetManager->textures.count)
    return NULL;
  return &gAssetManager->textures.tex[textureid];
}