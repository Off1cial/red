#include "engine/ui/ui.h"
#include "engine/ui/ui_draw.h"
#include "platform/common.h"
#include "corebase/mathlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>

// ui_widget.c

#define SEED_PLACEHOLDER 0

#define WINDOW_TITLEBAR_HEIGHT 16
#define WINDOW_RESIZEHANDLE_SIZE 12

static inline uint8_t ui_windowhasflag(uiwindow_t* win, uiwindowflag_t flag)
{
  return (win->flags & flag) != 0;
}

static uint32_t ui_hash_id(const char* str, uint32_t seed) {
    uint32_t h = seed ? seed : 2166136261u;
    while (*str) { h ^= (uint8_t)*str++; h *= 16777619u; }
    return h;
}


static void ui_rectcorner(rectdef rect, int i, float out[2])
{
  switch(i)
  {
    case 0:
      out[0] = rect[RECT_X];
      out[1] = rect[RECT_Y];
      break;
    case 1:
      out[0] = rect[RECT_X];
      out[1] = rect[RECT_Y] + rect[RECT_H];
      break;
    case 2:
      out[0] = rect[RECT_X] + rect[RECT_W];
      out[1] = rect[RECT_Y] + rect[RECT_H];
      break;
    case 3:
      out[0] = rect[RECT_X] + rect[RECT_W];
      out[1] = rect[RECT_Y];
      break;
    default:
      break;
  }
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


static inline uiwindow_t* ui_windowcurrent()
{
  return gUIctx->windowstack[gUIctx->windowstack_top - 1];
}

  

static inline void ui_windowcheckexpand(uiwindow_t* win, rectdef widget)
{
  float left   = widget[RECT_X];
  float right  = widget[RECT_X] + widget[RECT_W];
  float top    = widget[RECT_Y];
  float bottom = widget[RECT_Y] + widget[RECT_H];

  if (left < win->rect[RECT_X])
  {
    float diff = win->rect[RECT_X] - left;
    win->rect[RECT_X] = left;
    win->rect[RECT_W] += diff;
  }

  if (top < win->rect[RECT_Y])
  {
    float diff = win->rect[RECT_Y] - top;
    win->rect[RECT_Y] = top;
    win->rect[RECT_H] += diff;
  }

  if (right > win->rect[RECT_X] + win->rect[RECT_W])
    win->rect[RECT_W] = right - win->rect[RECT_X];

  if (bottom > win->rect[RECT_Y] + win->rect[RECT_H])
    win->rect[RECT_H] = bottom - win->rect[RECT_Y];
}

static inline uint8_t ui_rectnull(rectdef rect)
{
  return rect[0] == -FLT_MAX ||
         rect[1] == -FLT_MAX ||
         rect[2] == -FLT_MAX ||
         rect[3] == -FLT_MAX;
}

// Note Button1
// Upgrade this function to automatically 
// adjust the rect w/h based on the text

static inline void button_rect(
    uiwindow_t* win, 
    const char* text, 
    rectdef in, rectdef out)
{
  if (!win) exit(1); 
  if (ui_rectnull(in))
  {
    // Determine rect
    out[RECT_X] = win->cursor_x;
    out[RECT_Y] = win->cursor_y;
    // Temporary values until text is implemented
    out[RECT_W] = 120;
    out[RECT_H] = 80;
  }
  else
  {
    out[RECT_X] = win->rect[RECT_X] + in[RECT_X];
    out[RECT_Y] = win->rect[RECT_Y] + in[RECT_Y];
    // Change later to use text to determine w/h
    // For now just copy
    out[RECT_W] = in[RECT_W]; 
    out[RECT_H] = in[RECT_H];
  }
}


uint8_t UI_Button(const char* name, rectdef rect)
{
  // Window
  uiwindow_t* win = ui_windowcurrent();
  if (!win)
  {
    printf("[UI][BUTTON]: Current window is null\n");
    exit(1);
  }

  if (win->collapsed)
    return 0;
    
  // Note Button1
  rectdef buttonrect;
  button_rect(win, name, rect, buttonrect);
  ui_windowcheckexpand(win, buttonrect);


    
  // IMPORTANT
  // check that the window cursor hasnt exceeded the bounds,
  // GROW THE WINDOW??

  // Button
  uint32_t id = ui_hash_id(name, SEED_PLACEHOLDER);
  uint8_t hovered = ui_rectcontained(buttonrect, gPltInput->mx, gPltInput->my);
  int clicked = 0;
  if (hovered)
    gUIctx->hot_id = id;
  

  // Input and other shit
  if (gUIctx->hot_id == id)
  {
    if (pltInput_MouseClick(0))
    {
      clicked = 1;
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
     //colour[0], colour[1], colour[2], colour[3]);
  UI_DrawRect(buttonrect, 0, 0, 1, 1, colour, -1);
  win->cursor_y += buttonrect[RECT_H] + 8.0f;

  return clicked;
}

// Finds the window of matching name, otherwise makes a new one
static uiwindow_t* ui_windownew(const char* name, rectdef rect, uint64_t flags)
{
  uint32_t id = ui_hash_id(name, SEED_PLACEHOLDER);
  for (int i = 0; i < gUIctx->windowpersistcount; i++)
  { 
    if (id == gUIctx->windowpersist[i].id)
      return &gUIctx->windowpersist[i];
  }
  
  // Check if there is room for another window
  if (gUIctx->windowpersistcount >= UIWindowMax)
  {
    printf("[UI][WINDOW]: Too many persistent windows, cannot create another\n");
    return NULL;
  }

  // No window found, create one
  printf("[UI][WINDOW]: Creating a window\n");
  uiwindow_t* win = &gUIctx->windowpersist[gUIctx->windowpersistcount++];
  memset(win, 0, sizeof(uiwindow_t));

  // Copy name
  size_t len = strlen(name);
  // 64 Bytes
  if (len > sizeof(win->name))
    len = sizeof(win->name) - 1;
  memcpy(win->name, name, len);
  win->name[len] = '\0';

  win->id = id;
  win->flags = flags;

  // Determine rect
  if (!ui_rectnull(rect))
  {
    UIRect_Copy(rect, win->rect);
  }
  else
  {
    // Generate rect
    float cascade = 30.0f * (gUIctx->windowpersistcount - 1);
    win->rect[RECT_X] = cascade + 30;
    win->rect[RECT_Y] = cascade + 30;
    win->rect[RECT_W] = 100;
    win->rect[RECT_H] = 40;
  }

  return win;
}

static inline void windowtitlebar(uiwindow_t* win, rectdef out_titlebar)
{
  out_titlebar[RECT_X] = win->rect[RECT_X];
  out_titlebar[RECT_Y] = win->rect[RECT_Y];
  out_titlebar[RECT_W] = win->rect[RECT_W];
  out_titlebar[RECT_H] = WINDOW_TITLEBAR_HEIGHT;
}

static inline uint8_t ui_windowresizehover(uiwindow_t* win)
{
  if ((win->flags & UIWindowFlag_NoResize) != 0)
    return 0;
  rectdef rect;
  rect[RECT_X] = win->rect[RECT_X] + win->rect[RECT_W] - WINDOW_RESIZEHANDLE_SIZE;
  rect[RECT_Y] = win->rect[RECT_Y] + win->rect[RECT_H] - WINDOW_RESIZEHANDLE_SIZE;
  rect[RECT_W] = WINDOW_RESIZEHANDLE_SIZE;
  rect[RECT_H] = WINDOW_RESIZEHANDLE_SIZE;
  return ui_rectcontained(rect, gPltInput->mx, gPltInput->my);
}

uint8_t UI_Begin(const char* name, rectdef rect, uint64_t flags)
{
  uiwindow_t* win = ui_windownew(name, rect, flags);
  if (!win) return 0;

  rectdef titlebar;
  float titlebar_h = (flags & UIWindowFlag_NoTitleBar) ? 0.0f : WINDOW_TITLEBAR_HEIGHT;
  if (titlebar_h > 0)
    windowtitlebar(win, titlebar);
  

  uint8_t hovered = ui_rectcontained(win->rect, gPltInput->mx, gPltInput->my);
  uint8_t titlebar_hovered = (titlebar_h > 0) && 
    ui_rectcontained(titlebar, gPltInput->mx, gPltInput->my);

  uint8_t resize_hovered = ui_windowresizehover(win);
  
  // Start a drag
  if (titlebar_hovered && !gUIctx->windowdrag && pltInput_MouseClick(0))
  {
    gUIctx->windowdrag = win;
    gUIctx->windowdrag_offset[0] = gPltInput->mx - win->rect[RECT_X];
    gUIctx->windowdrag_offset[1] = gPltInput->my - win->rect[RECT_Y];
      //printf("DRAG START mx=%.2f my=%.2f rectX=%.2f rectY=%.2f offX=%.2f offY=%.2f\n",
    //gPltInput->mx, gPltInput->my, win->rect[RECT_X], win->rect[RECT_Y],
    //gUIctx->windowdrag_offset[0], gUIctx->windowdrag_offset[1]);
  }

  if (gUIctx->windowdrag == win)
  {
    if (pltInput_MouseDown(0))
    {
      //printf("Dragging..\n");
      //printf("Win(%0.2f, %0.2f), M(%0.2f, %0.2f)\n", 
      //  win->rect[RECT_X], win->rect[RECT_Y],
      //  gPltInput->mx, gPltInput->my
      //);
      win->rect[RECT_X] = gPltInput->mx - gUIctx->windowdrag_offset[0];
      win->rect[RECT_Y] = gPltInput->my - gUIctx->windowdrag_offset[1];
    }
    else // Mouse release
    {
      //printf("Mouse released\n");
      gUIctx->windowdrag = NULL;
    }
  }
  

  // Start a resize
  if (resize_hovered && !gUIctx->windowresize && pltInput_MouseClick(0))
    gUIctx->windowresize = win;

  if (gUIctx->windowresize)
  {
    if (pltInput_MouseDown(0))
    {
      // Resize window
      win->rect[RECT_W] += gPltInput->mx - (win->rect[RECT_X] + win->rect[RECT_W]);
      win->rect[RECT_H] += gPltInput->my - (win->rect[RECT_Y] + win->rect[RECT_H]);
    }
    else
    {
      gUIctx->windowresize = NULL;  
    }
  }
  

  // adjust cursor for padding
  win->cursor_x = win->rect[RECT_X] + 8.0f;
  win->cursor_y = win->rect[RECT_Y] + titlebar_h + 8.0f;

  if (gUIctx->windowstack_top >= UIWindowStackMax)
      EXIT_ERROR("[UI][WINDOW]: Window stack overflow");
  gUIctx->windowstack[gUIctx->windowstack_top++] = win;
  
  // Early exit if collapsed
  if (win->collapsed)
  {
    UI_DrawRect(
        titlebar,
        0, 0, 1, 1,
        gUIctx->style.window_bg_title,
        -1);
    return 0;
  }


  // Draw window
  
  if (!win->collapsed)
  {

    rgba wincol;
    if (hovered)
      RGBACopy(wincol, gUIctx->style.window_bg_hovered);
    else
      RGBACopy(wincol, gUIctx->style.window_bg_idle);

    UI_DrawRect(
        win->rect,
        0, 0, 1, 1,
        wincol,
        -1);
    
  }

  if (titlebar_h > 0.0f)
  {
    UI_DrawRect(
        titlebar, 
        0, 0, 1, 1,
        gUIctx->style.window_bg_title,
        -1);
  }
  if (!win->collapsed)
  {
    rgba border;
    UI_SETCOLRGBA_WHITE(border);
    border[3]=170;
    UI_DrawRectOutline(win->rect, border, 1.0f);
  }

  if (!ui_windowhasflag(win, UIWindowFlag_NoResize))
  {
    vec2_t verts[3];
    ui_rectcorner(win->rect, UI_RECTCORNER_BR, verts[0]);
    ui_rectcorner(win->rect, UI_RECTCORNER_BR, verts[1]);
    ui_rectcorner(win->rect, UI_RECTCORNER_BR, verts[2]);
    verts[1][1] -= WINDOW_RESIZEHANDLE_SIZE;
    verts[2][0] -= WINDOW_RESIZEHANDLE_SIZE;
    UI_DrawTriangle(verts[0], verts[1], verts[2], gUIctx->style.window_bg_title);
  }

  return 1;
}


void UI_End(void)
{
  if (gUIctx->windowstack_top == 0)
    EXIT_ERROR("[UI][WINDOW]: UI_End() called with empty stack");
  gUIctx->windowstack_top--;
}
