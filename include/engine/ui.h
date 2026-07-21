#pragma once

#include <stdint.h>
#include "engine/shader.h"
#include "platform/input.h"

typedef struct uivertex_t
{
  float pos[2];
  float uv[2];
  float colour[4];
} uivertex_t;

/*
 *uint32_t ui_hash_id(const char* str, uint32_t seed) {
    uint32_t h = seed ? seed : 2166136261u;
    while (*str) { h ^= (uint8_t)*str++; h *= 16777619u; }
    return h;
}
 *
 *
 */

// XYWH, Defines the bounding box of a UI element
typedef float rectdef[4];

#define RECT_X 0
#define RECT_Y 1
#define RECT_W 2
#define RECT_H 3

typedef float rgb[3];
typedef float rgba[4];

#define RGBAfromRGB(dest, rgb, a) \
  dest[0]=rgb[0];dest[1]=rgb[1];dest[2]=rgb[2];dest[3]=a

#define RGBCopy(dest, rgb) \
  dest[0]=rgb[0];dest[1]=rgb[1];dest[2]=rgb[2]

// UI colouring
typedef struct uistyle_t
{
  rgb button_idle;
  rgb button_hovered;
  rgb button_clicked;
} uistyle_t;


#define UI_InputM1 0
#define UI_InputM2 1

typedef struct uicontext_t
{
  // Obtain from SDL each frame
  uint32_t mouse_x, mouse_y;
  char textinput[64]; // If the user can somehow enter 64 characters at once, kill them
  uint32_t keypress;  // SDL_Scancode
  uint8_t mpress; // -1 if none



  // State
  uint32_t hot_id; // Hovered;
  uint32_t active_id; // Held, being used (Scrollbars, text fields)

  // Draw buffer
  uivertex_t* vertices; // vbo
  uint32_t vertexcount, vertexcapacity;
  GLuint vao, vbo, ebo;
  CBaseShader* shader;

  uint32_t* indices; // vao
  uint32_t indexcount, indexcapacity;

  // Styling
  uistyle_t style;

} uicontext_t;

extern uicontext_t* gUIctx;

uint8_t UI_Init();

// ui_draw.c

// Upload data to GPU
void UI_DrawBatch(); 

void UI_FrameBegin();
void UI_FrameEnd();

// ui_widget.c
uint8_t UI_Button( const char* name, rectdef rect );
