#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform/common.h"
#include "engine/assetmanager.h"
#include "engine/hash.h"
#include "platform/stb_image.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <glad/glad.h>


const char *assetDir = ASSET_DIR;

#define ATLAS_SIZE 512

CBaseAssetManager *gAssetManager;

static inline void ResolveAssetPath(const char *path_in_assets, char *out, size_t outsize)
{
  snprintf(out, outsize, "%s/%s", assetDir, path_in_assets);
}

// Extracts filename from a path into out (min 256 bytes)
static inline int NameFromPath(const char *path, char *out)
{
  if (!path || !out)
    return 0;
  const char *name = strrchr(path, '/');
  name = name ? name + 1 : path;
  strncpy(out, name, 255);
  out[255] = '\0';
  return 1;
}

uint32_t AssetManager_AddShader(const char* vpath, const char* fpath)
{
  if (!vpath || !fpath) return (u32)-1;

  assetShader_t shaderasset;
  assethandle_t shaderhandle;
  char v_abspath[256];
  ResolveAssetPath(vpath, v_abspath, 256);
  Q_strncpy(shaderasset.path, vpath, 256);
  
  

  shaderasset.shader = CBaseShader_Create(vpath, fpath);
  shaderhandle.index = gAssetManager->shaders.count;
  shaderasset.handle = shaderhandle;

  gAssetManager->shaders.shaders[shaderasset.handle.index] = shaderasset;
  gAssetManager->shaders.count++;
  
  return shaderhandle.index;
}


uint32_t AssetManager_AddTexture(const char *path)
{
  if (gAssetManager->textures.count >= ASSETS_MAX_TEXTURES)
  {
    printf("[ASSETS][TEXTURE]: Registry full, cannot load %s\n", path);
    return (uint32_t)-1;
  }

  char abspath[256];
  ResolveAssetPath(path, abspath, 256);
  int width, height, channels;
  byte *pixels = stbi_load(abspath, &width, &height, &channels, 0);

  if (!pixels)
  {
    printf("[ASSETS][TEXTURE]: Failed to load texture\n");
    printf("[ASSETS][TEXTURE]: Path: %s\n", abspath);
    printf("[ASSETS][TEXTURE]: STB: %s\n", stbi_failure_reason());
    return (uint32_t)-1;
  }

  GLenum format, internal;
  switch (channels)
  {
  case 1:
    format = GL_RED;
    internal = GL_R8;
    break;
  case 3:
    format = GL_RGB;
    internal = GL_RGB8;
    break;
  case 4:
    format = GL_RGBA;
    internal = GL_RGBA8;
    break;
  default:
    printf("[ASSETS][TEXTURE]: Unsupported channel count (%d)\n", channels);
    stbi_image_free(pixels);
    return (uint32_t)-1;
  }

  GLuint texid;
  glGenTextures(1, &texid);
  glBindTexture(GL_TEXTURE_2D, texid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(pixels);

  uint32_t idx = gAssetManager->textures.count;
  assetTexture_t *tex = &gAssetManager->textures.tex[idx];
  memset(tex, 0, sizeof(*tex));

  tex->texture.gltexnum = texid;
  tex->texture.w = width;
  tex->texture.h = height;
  tex->state = ASSET_LOADED;
  tex->handle.index = idx;

  strncpy(tex->path, path, sizeof(tex->path) - 1);
  tex->path[sizeof(tex->path) - 1] = '\0';

  gAssetManager->textures.count++;
  return idx;
}

uint8_t AssetManager_AddFont(const char *path, int size)
{
  if (gAssetManager->fonts.count >= ASSETS_MAX_FONTS)
  {
    printf("[ASSETS][FONTS]: Registry full, cannot load %s\n", path);
    return 0;
  }

  char abspath[256];
  ResolveAssetPath(path, abspath, sizeof(abspath));

  TTF_Font *ttf_font = TTF_OpenFont(abspath, size);
  if (!ttf_font)
  {
    printf("[ASSETS][FONT]: Failed to load font, %s\n", SDL_GetError());
    return 0;
  }

  assetFont_t font;
  memset(&font, 0, sizeof(font));
  font.size = size;
  font.lineheight = TTF_GetFontHeight(ttf_font);
  font.ascent = TTF_GetFontAscent(ttf_font);

  SDL_Surface *atlas = SDL_CreateSurface(ATLAS_SIZE, ATLAS_SIZE, SDL_PIXELFORMAT_RGBA8888);
  if (!atlas)
  {
    printf("[ASSETS][FONT]: Failed to create font atlas, %s\n", SDL_GetError());
    TTF_CloseFont(ttf_font);
    return 0;
  }

  int penx = 0, peny = 0, maxrowheight = 0;

  for (int c = 32; c < 127; c++)
  {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *glyphsurf = TTF_RenderGlyph_Blended(ttf_font, (uint32_t)c, white);
    if (!glyphsurf)
      continue;

    int gw = glyphsurf->w, gh = glyphsurf->h;

    if (penx + gw + 1 >= ATLAS_SIZE)
    {
      penx = 0;
      peny += maxrowheight + 1;
      maxrowheight = 0;
    }
    if (peny + gh + 1 >= ATLAS_SIZE)
    {
      printf("[ASSETS][FONTS]: Atlas overflow at char '%c'\n", (char)c);
      SDL_DestroySurface(glyphsurf);
      break;
    }

    SDL_Rect dest = {penx, peny, gw, gh};
    SDL_BlitSurface(glyphsurf, NULL, atlas, &dest);

    int advance = 0, bearingx = 0, bearingy = 0;
    TTF_GetGlyphMetrics(ttf_font, (uint32_t)c, NULL, NULL, &bearingx, &bearingy, &advance);

    struct _fontglyph *glyph = &font.glyphs[c];
    glyph->uv0[0] = (float)penx / ATLAS_SIZE;
    glyph->uv0[1] = (float)peny / ATLAS_SIZE;
    glyph->uv1[0] = (float)(penx + gw) / ATLAS_SIZE;
    glyph->uv1[1] = (float)(peny + gh) / ATLAS_SIZE;
    glyph->w = gw;
    glyph->h = gh;
    glyph->bearing[0] = bearingx;
    glyph->bearing[1] = bearingy;
    glyph->advance = advance;

    penx += gw + 1;
    if (gh > maxrowheight)
      maxrowheight = gh;

    SDL_DestroySurface(glyphsurf);
  }

  char texpath[256];
  ResolveAssetPath("Fonts/font_atlas.bmp", texpath, sizeof(texpath));
  SDL_SaveBMP(atlas, texpath);

  uint32_t texhandle = AssetManager_AddTexture("Fonts/font_atlas.bmp");


  SDL_DestroySurface(atlas);
  TTF_CloseFont(ttf_font);

  if (texhandle == (uint32_t)-1)
    return 0;
  font.tex_index = (int)texhandle;

  strncpy(font.path, path, sizeof(font.path) - 1);
  font.path[sizeof(font.path) - 1] = '\0';

  char fontname[256];
  NameFromPath(path, fontname);

  uint32_t idx = gAssetManager->fonts.count;
  font.handle.index = idx;
  font.hash = Hash_String(fontname);

  uint32_t bucket = Hash_Bucket(font.hash, ASSETS_FONT_HASH_BUCKETS);

  gAssetManager->fonts.fonts[idx] = font;
  gAssetManager->fonts.fonts[idx].next = gAssetManager->fontHashTable[bucket];
  gAssetManager->fontHashTable[bucket] = &gAssetManager->fonts.fonts[idx];
  gAssetManager->fonts.count++;

  printf("[ASSETS][FONTS]: Loaded font (%d): %s\n", idx, path);
  return 1;
}



uint32_t AssetManger_GetFontID(const char *name)
{
  uint32_t bucket = Hash_Bucket(Hash_String(name), ASSETS_FONT_HASH_BUCKETS);

  for (assetFont_t *font = gAssetManager->fontHashTable[bucket]; font; font = font->next)
  {
    char fontname[256];
    NameFromPath(font->path, fontname);
    if (Q_stricmp(name, fontname) == 0)
      return font->handle.index;
  }

  printf("[ASSETS][FONTS]: Font not found: %s\n", name);
  return (uint32_t)-1;
}


uint8_t AssetManager_LoadModel(const char* path, model_t* out)
{
  mdlheader_t header;
  if (!ModelFile_GetModel(path, out, &header))
    return 0;

  out->texturehandle = AssetManager_AddTexture(header.texpath);
  return 1;
}



uint8_t AssetManager_Init()
{
  gAssetManager = malloc(sizeof(CBaseAssetManager));
  if (!gAssetManager)
    return 0;

  memset(gAssetManager, 0, sizeof(CBaseAssetManager));
  gAssetManager->textures.count = 0; // Reserve 0 for no texture

  TTF_Init();

  printf("DEV HANDLE = %d\n", AssetManager_AddTexture("Textures/dev.png"));
  AssetManager_AddFont("Fonts/Roboto.ttf", 20);

  ModelFile_Write("Models/obj/cube.obj", "Textures/dev.png", "Models/mdl/cube.mdl");
  //ModelFile_Write("Models/obj/isosphere.obj", "Textures/dev.png", "Models/mdl/isosphere.mdl");
  //ModelFile_Write("Models/obj/donut.obj", "Textures/dev.png", "Models/mdl/donut.mdl");
  AssetManager_LoadModel("Models/mdl/cube.mdl", &testmodel);

  return 1;
}
