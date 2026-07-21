#include "engine/ui.h"
#include "engine/ui_draw.h"
#include <stdio.h>

#define SEED_PLACEHOLDER 0

static uint32_t ui_hash_id(const char* str, uint32_t seed) {
    uint32_t h = seed ? seed : 2166136261u;
    while (*str) { h ^= (uint8_t)*str++; h *= 16777619u; }
    return h;
}

static uint8_t ui_rectcontained(rectdef rect, float x, float y)
{
  float xfar = rect[RECT_X] + rect[RECT_W];
  float yfar = rect[RECT_Y] + rect[RECT_H];
  
  return (
      x >= rect[RECT_X] && x <= xfar && 
      y >= rect[RECT_Y] && y <= yfar
      );
}

uint8_t UI_Button(const char* name, rectdef rect)
{
  uint32_t id = ui_hash_id(name, SEED_PLACEHOLDER);
  uint8_t hovered = ui_rectcontained(rect, gUIctx->mouse_x, gUIctx->mouse_y);
  int clicked = 0;
  if (hovered)
    gUIctx->hot_id = id;
  

  // Input and other shit
  if (gUIctx->hot_id == id)
  {
    if (gUIctx->mpress == UI_InputM1)
    {
      gUIctx->active_id = id;
    }
  }
   
  // Determine colour
  rgba colour;
  if (gUIctx->active_id == id)
  {
    // Active
    RGBAfromRGB(colour, gUIctx->style.button_clicked, 255); 
  }else if (gUIctx->hot_id == id)
  {
    RGBAfromRGB(colour, gUIctx->style.button_hovered, 255);
  }
  else
  {
    RGBAfromRGB(colour, gUIctx->style.button_idle, 255);  
  }
  //printf("Colour determined\n\t(%0.2f, %0.2f, %0.2f, %0.2f)\n",
    //  colour[0], colour[1], colour[2], colour[3]);
  UI_DrawRect(rect, colour);

  return clicked;
}
