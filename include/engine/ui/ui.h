#pragma once

#include <stdint.h>
#include "engine/shader.h"
#include "engine/ui/ui_colours.h"
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




// Use to set a rect to null
#define UIRECT_NULL(rect) \
do { \
    (rect)[0] = -FLT_MAX; \
    (rect)[1] = -FLT_MAX; \
    (rect)[2] = -FLT_MAX; \
    (rect)[3] = -FLT_MAX; \
} while (0)

#define UIRect_Copy(src, dest) \
do { \
    (dest)[0] = (src)[0]; \
    (dest)[1] = (src)[1]; \
    (dest)[2] = (src)[2]; \
    (dest)[3] = (src)[3]; \
} while (0)

// UI colouring
typedef struct uistyle_t
{
  rgb button_idle;
  rgb button_hovered;
  rgb button_clicked;

  rgba window_bg_hovered;
  rgba window_bg_idle;
  rgba window_bg_title;
} uistyle_t;



typedef enum uiwindowflag_t
{
  UIWindowFlag_NoMove     = 0,
  UIWindowFlag_NoResize   = 1,
  UIWindowFlag_NoCollapse = 2,
  UIWindowFlag_NoTitleBar = 4,
} uiwindowflag_t;

#define UIWindowFlag_All ((uint64_t)-1)
#define UIWindowStackMax ((uint8_t)16)
#define UIWindowMax ((uint8_t)16)

typedef struct uiwindow_t
{
  char name[64];
  rectdef rect;
  float cursor_x, cursor_y;

  uint64_t flags; // When implemented
  uint32_t id;
  uint8_t collapsed;
} uiwindow_t;


typedef struct uicontext_t
{
  // State
  uint32_t hot_id; // Hovered;
  uint32_t active_id; // Held, being used (Scrollbars, text fields)
  
  // Used to store windows across frames
  uiwindow_t windowpersist[UIWindowMax];
  uint8_t windowpersistcount;

  uiwindow_t* windowdrag;

  // Used to find the current window, reset each frame (top = 0)
  uiwindow_t* windowstack[UIWindowStackMax]; // Pointers into persistence
  uint8_t windowstack_top; // 0-127
  uint8_t hotwindow_t;

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

// Windows
uint8_t UI_Begin(const char* name, rectdef rect, uint64_t flags);
void UI_End();

uint8_t UI_Button( const char* name, rectdef rect );




