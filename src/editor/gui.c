#include "engine/ui/ui.h"
#include "engine/camera.h"
#include "platform/window.h"
#include "platform/input.h"
#include "platform/common.h"

#include "editor/editor.h"
#include "editor/gui.h"



#include <float.h>
#include <stdio.h>

#define PANEL3D_STR "3D Panel"
#define PANELTOP_STR "Top (x/z)"
#define PANELFRONT_STR "Front (x/y)"
#define PANELSIDE_STR "Side (z/y)"
#define PANELTOOLS_STR "Tools"



// How many world units the space between grid lines represent
static float gGridSizes[] =
{
  1.0f,
  10.0f,
  100.0f,
  1000.0f,
};

enum
{
  GRID_SMALL,
  GRID_MED,
  GRID_LARGE,
  GRID_HUGE,
};

typedef enum { AXIS_X, AXIS_Y, AXIS_Z } PANEL_AXIS;

int gGridLevel = GRID_MED;



panel_t gPanels[5];
u64 PanelFlags =
  UIWindowFlag_NoCollapse |
  UIWindowFlag_NoMove |
  UIWindowFlag_NoResize |
  UIWindowFlag_NoTitleBar;

#define EDITORUI_TOOLSWIDTH 0.25  // The tools panel occupies 1/4 of the screen's width
                                  //
static void Vec2Clamp2Rect(vec2_t in, rectdef rect, vec2_t out)
{
  out[0] = fmaxf(rect[0], out[0]);
  out[1] = fmaxf(rect[1], out[1]);

  out[0] = fminf(rect[0] + rect[2], out[0]);
  out[1] = fminf(rect[1] + rect[3], out[1]);
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


static void PanelAxes(paneltype_t type, int* axis_a, int* axis_b)
{
  switch(type)
  {
    case PANEL_TOP:     *axis_a = 0; *axis_b = 2; break; // X/Z
    case PANEL_FRONT:   *axis_a = 0; *axis_b = 1; break; // X/Y
    case PANEL_SIDE:    *axis_a = 2; *axis_b = 1; break; // Z/Y
    default: break;
  }
}



static void CalculatePanels()
{

  int toolswidth = gPltWindow->winw * EDITORUI_TOOLSWIDTH;
  int halfwinh = gPltWindow->winh * 0.5;
  int toprowwidth = gPltWindow->winw - toolswidth;
  rectdef rect_3d = {0,0, toprowwidth, halfwinh};
  rectdef rect_tools = {gPltWindow->winw - toolswidth, 0, toolswidth, gPltWindow->winh};

  rectdef rect_top = {0, halfwinh, toprowwidth * 0.5, halfwinh};
  rectdef rect_front = {toprowwidth * 0.5, halfwinh, toprowwidth * 0.5, halfwinh};

  UIRect_Copy(rect_3d, gPanels[PANEL_3D].rect);
  UIRect_Copy(rect_top, gPanels[PANEL_TOP].rect);
  UIRect_Copy(rect_front, gPanels[PANEL_FRONT].rect);
  UIRect_Copy(rect_tools, gPanels[PANEL_TOOLS].rect);
}

void GUI_InitialisePanels()
{
  gPanels[PANEL_3D].type = PANEL_3D;
  gPanels[PANEL_TOP].type = PANEL_TOP;
  gPanels[PANEL_SIDE].type = PANEL_SIDE;
  gPanels[PANEL_FRONT].type = PANEL_FRONT;
  gPanels[PANEL_TOOLS].type = PANEL_TOOLS;

  gPanels[PANEL_3D].name = PANEL3D_STR;
  gPanels[PANEL_TOP].name = PANELTOP_STR;
  gPanels[PANEL_SIDE].name = PANELSIDE_STR;
  gPanels[PANEL_FRONT].name = PANELFRONT_STR;
  gPanels[PANEL_TOOLS].name = PANELTOOLS_STR;
  
  CalculatePanels();
  gPanels[PANEL_TOP].camera = Camera_Create(VEC_ZERO, VEC_AXIS_Y_NEG, (cViewport){0});

  gPanels[PANEL_SIDE].camera = Camera_Create(VEC_ZERO, VEC_AXIS_X, (cViewport){0});

  gPanels[PANEL_FRONT].camera = Camera_Create(VEC_ZERO, VEC_AXIS_Z_NEG, (cViewport){0});

  int i;
  for (i = PANEL_TOP; i <= PANEL_SIDE; i++)
  {
    PanelAxes(i, &gPanels[i].axis_a, &gPanels[i].axis_b);
    memcpy(gPanels[i].camera->viewport, gPanels[i].rect, sizeof(float) * 4 );
  }

}

void GUI_Initialise()
{
  RGBASet(gUIctx->style.window_bg_idle, 30, 30, 30, 255);
  GUI_InitialisePanels();
}



static void ViewportRect(rectdef rect)
{
  glViewport(rect[0], rect[1], rect[2], rect[3]);
}
// world -> screen, using the panel's 2D projection axes
static void Panel_WorldToScreen(panel_t* p, vec3_t world, vec2_t out)
{
  int ax = p->axis_a;
  int ay = p->axis_b;

  float zoom = p->camera->fov;          // world units per screen pixel
  float camx = p->camera->origin[ax];
  float camy = p->camera->origin[ay];

  float halfw = p->rect[RECT_W] * 0.5f;
  float halfh = p->rect[RECT_H] * 0.5f;

  out[0] = p->rect[RECT_X] + halfw + (world[ax] - camx) / zoom;
  out[1] = p->rect[RECT_Y] + halfh - (world[ay] - camy) / zoom;  // flip Y
}

// screen -> world, using the panel's 2D projection axes
static void Panel_ScreenToWorld(panel_t* p, float sx, float sy, vec3_t out)
{
  int ax = p->axis_a;
  int ay = p->axis_b;

  float zoom = p->camera->fov;
  float halfw = p->rect[RECT_W] * 0.5f;
  float halfh = p->rect[RECT_H] * 0.5f;

  out[ax] = p->camera->origin[ax] + (sx - p->rect[RECT_X] - halfw) * zoom;
  out[ay] = p->camera->origin[ay] - (sy - p->rect[RECT_Y] - halfh) * zoom;

  // the axis not covered by this panel (depth) has to come from
  // wherever the caller needs it — e.g. camera->origin[thirdaxis],
  // or a fixed grid plane, since orthographic panels drop it entirely
}

static void DrawPanel_Tools()
{
  if (UI_Begin("Tools", gPanels[PANEL_TOOLS].rect, PanelFlags))
  {
    rectdef brect; UIRECT_NULL(brect);
    if (UI_Button("Button", brect))
    {
      printf("Clicked\n");
      vec3_t min = {-4, -4, -4};
      vec3_t max = {4, 4, 4};
      ECMD_BrushCreate(min, max);
    }
  }
  UI_End();
}

// Iterate brushes, draw for each panel, or iterate panels and draw each brush
/*
static void DrawPanelBrushes()
{
  brush_t* blist;
  for (blist = gBrushes; blist; blist=blist->next)
  {
    
  }
}
*/

static void DrawPanelBrushes(panel_t* p)
{
  int i;
  
  brush_t* list = NULL;
  for (list = gBrushes; list; list=list->next)
  {
    brushrender_t* renderable = list->renderable;
    CBaseMesh* mesh = renderable->mesh;
    for (i = 0; i + 2 < mesh->indexcount; i += 3)
    {
      uint32_t i0 = mesh->indices[i];
      uint32_t i1 = mesh->indices[i + 1];
      uint32_t i2 = mesh->indices[i + 2];

      gpuVertex v0 = mesh->vertices[i0];
      gpuVertex v1 = mesh->vertices[i1];
      gpuVertex v2 = mesh->vertices[i2];
      
      vec2_t s0, s1, s2;
      Panel_WorldToScreen(p, v0.xyz, s0);
      Panel_WorldToScreen(p, v1.xyz, s1);
      Panel_WorldToScreen(p, v2.xyz, s2);
    
      //printf("s0: "); vec2print(s0);
      //printf("s1: "); vec2print(s1);
      //printf("s2: "); vec2print(s2);
      Vec2Clamp2Rect(s0, p->rect, s0);
      Vec2Clamp2Rect(s1, p->rect, s1);
      Vec2Clamp2Rect(s2, p->rect, s2);

      u32 linecol = COL32(20, 255, 20, 160);

      UI_DrawLine(s0, s1, linecol, 1.0f);
      UI_DrawLine(s0, s2, linecol, 1.0f);
      UI_DrawLine(s2, s1, linecol, 1.0f);
    }
  }
}

static void DrawPanelGrid(panel_t* p)
{
  int ax, ay;
  PanelAxes(p->type, &ax, &ay);

  float zoom = p->camera->fov;           // world units per screen pixel
  float camx = p->camera->origin[ax];
  float camy = p->camera->origin[ay];

  float halfw = (p->rect[2] * 0.5f) * zoom;
  float halfh = (p->rect[3] * 0.5f) * zoom;

  float minx = camx - halfw, maxx = camx + halfw;
  float miny = camy - halfh, maxy = camy + halfh;

  int level = gGridLevel;
  float grid = gGridSizes[level];
  while ((grid / zoom) < 16.0f && level < GRID_HUGE)
    grid = gGridSizes[++level];

  float startx = floorf(minx / grid) * grid;
  float starty = floorf(miny / grid) * grid;

  u32 gridcol = COL32(80, 80, 80, 255); // fainter — see note below

  for (float x = startx; x <= maxx; x += grid)
  {

    float sx = p->rect[0] + (x - minx) / zoom;   // world -> screen
    if (sx < p->rect[0] || sx >= p->rect[0] + p->rect[2]) continue;

    vec2_t a, b;
    a[0] = sx; a[1] = p->rect[1];
    b[0] = sx; b[1] = p->rect[1] + p->rect[3];
    UI_DrawLine(a, b, (fabsf(x) < 0.01f) ? 0xFFFFFFFF : gridcol, 1.0f);
  }

  for (float y = starty; y <= maxy; y += grid)
  {
    // world Y increases "up", screen Y increases downward — flip
    float sy = p->rect[1] + p->rect[3] - (y - miny) / zoom;
    if (sy < p->rect[1] || sy >= p->rect[1] + p->rect[3]) continue;

    vec2_t a, b;
    a[0] = p->rect[0]; a[1] = sy;
    b[0] = p->rect[0] + p->rect[2]; b[1] = sy;
    UI_DrawLine(a, b, (fabsf(y) < 0.01f) ? 0xFFFFFFFF : gridcol, 1.0f);
  }
}



static void DrawPanel(panel_t* p)
{
  clampf(&p->camera->fov, 0.1f, 10.0f);
  if (UI_Begin(p->name, p->rect, PanelFlags))
  {
    u8 hovered = 
      ui_rectcontained(p->rect, gPltInput->mx, gPltInput->my);
    if (hovered) gHoveredPanel = p->type;

    DrawPanelGrid(p);

    DrawPanelBrushes(p);
  }

  UI_End();
}

void GUI_Draw()
{
  gHoveredPanel = -1;
  glViewport(0, 0, gPltWindow->winw, gPltWindow->winh);
  DrawPanel_Tools();
  for (int i = 0; i < PANEL_TOOLS; i++)
  {
    DrawPanel(&gPanels[i]);
  }
}
