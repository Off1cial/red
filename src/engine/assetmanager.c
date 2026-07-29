
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "engine/assetmanager.h"
#include "platform/common.h"
#include "platform/stb_image.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

#define SEED_PLACEHOLDER 0
#define ATLAS_SIZE 512

CBaseAssetManager* gAssetManager;

static inline uint64_t str_hash(const char* str, uint64_t seed)
{
  uint32_t h = seed ? seed : 2166136261u;
  while (*str) { h ^= (uint8_t)*str++; h *= 16777619u; }
  return h;
}

static inline int NameFromPath(const char* path, char* out)
{
    if (!path || !out)
        return 0;

    const char* name = strrchr(path, '/');

    if (name)
        name++;
    else
        name = path;

    strcpy(out, name);

    return 1;
}

int AssetManager_Init()
{
  gAssetManager = malloc(sizeof(CBaseAssetManager));
  if (!gAssetManager)
    return 0;
  
  memset(&gAssetManager->textures, 0, sizeof(struct _texregistry));
  memset(&gAssetManager->shaders, 0, sizeof(struct _shaderregistry));
  memset(&gAssetManager->fonts, 0, sizeof(struct _fontregistry));



  return 1;
}

uint8_t AssetManager_AddTexture(const char* path)
{
  assetTexture_t texture;
  texture.state = ASSET_LOADING;

  int width;
  int height;
  int channels;

  byte* pixels = stbi_load(
      path, &width, &height, &channels, 0
      );

  if (!pixels)
  {
    printf("[ASSETS][TEXTURE]: Failed to load texture data, %s\n", path);
    return 0;
  }

  // OpenGL 
  GLuint texid;

  glGenTextures(1, &texid);
  glBindTexture(GL_TEXTURE_2D, texid);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLenum format = GL_RGB;
  GLenum internal = GL_RGB8;

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
      printf("[ASSETS][TEXTURE]: Unsupported image channel count\n");
      stbi_image_free(pixels);
      return 0;
      break;
  }
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      internal,
      width,
      height,
      0,
      format,
      GL_UNSIGNED_BYTE,
      pixels
      );

  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(pixels);

  texture = (assetTexture_t){0};

  texture.texid = texid;
  texture.width = width;
  texture.height = height;
  
  texture.state = ASSET_LOADED;
  Q_strncpy(texture.path, path, 256);
  texture.handle.index = gAssetManager->textures.count;
  
  gAssetManager->textures.tex[gAssetManager->textures.count++] = texture;
  
  return 1;
}

uint8_t AssetManager_AddFont(const char* path, int size)
{

  SDL_Surface* atlas;
  TTF_Font* ttf_font;
  struct assetFont_t font;
  

  memset(&font, 0, sizeof(struct assetFont_t));
  ttf_font = TTF_OpenFont(path, size);

  if (!ttf_font)
  {
    printf("[ASSETS][FONT]: Failed to load font, %s\n", SDL_GetError());
    return 0; 
  }

  font.size = size;
  font.lineheight = TTF_GetFontHeight(ttf_font);

  atlas = SDL_CreateSurface(
      ATLAS_SIZE,
      ATLAS_SIZE,
      SDL_PIXELFORMAT_RGBA8888
      );
  if (!atlas)
  {
    printf("[ASSETS][FONT]: Failed to create font atlas, %s\n", SDL_GetError());
    TTF_CloseFont(ttf_font);
    return 0;
  }

  int penx = 0, peny = 0;
  int maxrowheight = 0;

  for (int c = 32; c < 127; c++)
  {
    SDL_Color white = {255,255,255,255}; 
    SDL_Surface* glyphsurf = TTF_RenderGlyph_Blended(
        ttf_font,
        (uint32_t)c,
        white);
    if (!glyphsurf)
      continue;

    int gw = glyphsurf->w;
    int gh = glyphsurf->h;

    if (penx + gw + 1 >= ATLAS_SIZE)
    {
      penx = 0;
      peny += maxrowheight + 1;
      maxrowheight = 0;
    }

    if (peny + gh + 1 >= ATLAS_SIZE)
    {
      printf("[ASSETS][FONTS]: Font atlas overflow, character = %c", (char)c);
      SDL_DestroySurface(glyphsurf);
      break;
    }


    // Copy glyph to atlas (Grep this in old CProjects/Engine)

   // ...
    SDL_Rect dest = {penx, peny, gw, gh};
    SDL_BlitSurface(glyphsurf, NULL, atlas, &dest);

    int advance = 0;
    int bearingx = 0, bearingy = 0;
    TTF_GetGlyphMetrics(ttf_font, (uint32_t)c, NULL, NULL, &bearingx, &bearingy, &advance);
    font.ascent = TTF_GetFontAscent(ttf_font);

    struct _fontglyph* glyph = &font.glyphs[c];
    glyph->uv0[0] = (float)penx / (float)ATLAS_SIZE;
    glyph->uv0[1] = (float)peny/ (float)ATLAS_SIZE;
    glyph->uv1[0] = (float)(penx + gw) / (float)ATLAS_SIZE;
    glyph->uv1[1] = (float)(peny + gh) / (float)ATLAS_SIZE;

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

  // Create OpenGL texture from atlas
  SDL_SaveBMP(atlas, "../../Assets/Fonts/font_atlas.bmp");
  if (!AssetManager_AddTexture("../../Assets/Fonts/font_atlas.bmp"))
    return 0;

  SDL_DestroySurface(atlas);
  TTF_CloseFont(ttf_font);

  // Add font to the manager
  uint32_t i = gAssetManager->fonts.count;
  font.next = &gAssetManager->fonts.fonts[i];
  font.handle.index = i;
  gAssetManager->fonts.fonts[i] = font;


  return 1;
}

uint32_t AssetManger_GetFontID(const char* name)
{
  uint32_t hash = str_hash(name, SEED_PLACEHOLDER); 
  struct assetFont_t* font = &gAssetManager->fonts.fonts[hash]; 
  // Extract name from path
  char fontname[256];
  while (Q_stricmp(name, fontname) != 0)
  {
    font = font->next;
    NameFromPath(font->path, fontname);
  }

  return font->handle.index;

  

}

