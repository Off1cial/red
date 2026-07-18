#define STB_IMAGE_IMPLEMENTATION
#include "platform/stb_image.h"
#include "platform/common.h"
#include "corebase/texture.h"

#include <assert.h>

static texture_t* Texture_Alloc( void )
{
  texture_t* tex = malloc(sizeof(texture_t));
  if (!tex) return NULL;
  memset(tex, 0, sizeof(texture_t));
  return tex;
}


void CBaseAsset_TextureLoad(const char* filepath, texture_t* destination)
{
  assert(filepath);
  assert(destination);
  
  int w,h,ch;
  byte* pixels = stbi_load(filepath, &w, &h, &ch, 0);   
  
  if (!pixels)
  {
    printf("[TEXTURE]: Failed to load texture from \"%s\"\n", filepath);
    // Set texture to default missing
  }

  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE0, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
      GL_LINEAR_MIPMAP_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLenum format = GL_RGB;
  GLenum internal = GL_RGB8;

  switch (ch)
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
      printf("[TEXTURE]: Unsupported channel count (%d), quitting...\n", ch);
      stbi_image_free(pixels);
      exit(1);
      break;
  }
  // Enable dimensions that arent a power of 2
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      internal,
      w,
      h,
      0,
      format,
      GL_UNSIGNED_BYTE,
      pixels
  );

  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(pixels);

  memset(destination, 0, sizeof(texture_t));
  destination->target = GL_TEXTURE_2D;
  destination->w = w;
  destination->h = h;
  destination->id = id;
  destination->format = format;
}



void CBaseAssset_TextureDestroy(texture_t* texture)
{
  assert(texture);
  // do shit
  free(texture);
}
