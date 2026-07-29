#pragma once

#include <SDL3_ttf/SDL_ttf.h>

#define UI_MAX_GLYPTHS 128
#define GLYPH_ATLAS_SIZE 512

typedef struct {
  float uv0[2]; // Top left UVs
  float uv1[2]; // Bottom right UVs
  int w, h;
  int advance;
  

} ui_glypth_t;


typedef struct 
{
  ui_glypth_t glyphs[UI_MAX_GLYPTHS];
  int size;
  float lineheight;
} ui_font_t;
