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

typedef enum paneltype_t
{
  PANEL_TOP,
  PANEL_FRONT,
  PANEL_SIDE,
  PANEL_TOOLS, // KEEP THIS ORDER
  PANEL_3D,
  PANEL_COUNT
} paneltype_t;

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

static void DrawPanel_Tools()
{
  if (UI_Begin("Tools", gPanels[PANEL_TOOLS].rect, PanelFlags))
  {
    rectdef brect; UIRECT_NULL(brect);
    if (UI_Button("Button", brect))
    {
      printf("Clicked\n");
    }
  }
  UI_End();
}

static void DrawPanel_3D()
{
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

  u32 gridcol = 0xFFFFFF44; // fainter — see note below

  for (float x = startx; x <= maxx; x += grid)
  {
    float sx = p->rect[0] + (x - minx) / zoom;   // world -> screen

    vec2_t a, b;
    a[0] = sx; a[1] = p->rect[1];
    b[0] = sx; b[1] = p->rect[1] + p->rect[3];
    UI_DrawLine(a, b, (fabsf(x) < 0.01f) ? 0xFFFFFFFF : gridcol, 1.0f);
  }

  for (float y = starty; y <= maxy; y += grid)
  {
    // world Y increases "up", screen Y increases downward — flip
    float sy = p->rect[1] + p->rect[3] - (y - miny) / zoom;

    vec2_t a, b;
    a[0] = p->rect[0]; a[1] = sy;
    b[0] = p->rect[0] + p->rect[2]; b[1] = sy;
    UI_DrawLine(a, b, (fabsf(y) < 0.01f) ? 0xFFFFFFFF : gridcol, 1.0f);
  }
}



static void DrawPanel(panel_t* p)
{
  clampf(&p->camera->fov, 10.0f, 90.0f);
  if (UI_Begin(p->name, p->rect, PanelFlags))
  {
    u8 hovered = 
      ui_rectcontained(p->rect, gPltInput->mx, gPltInput->my);
    if (hovered) gHoveredPanel = p->type;

    DrawPanelGrid(p);
  }

  UI_End();

  // Draw top,side,front panels
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
  DrawPanel_3D();

}
