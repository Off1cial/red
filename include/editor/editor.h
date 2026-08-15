#pragma once

#include "corebase/mathlib.h"
#include "engine/camera.h"
#include "engine/ui/ui.h"
#include "engine/assetmanager.h"


#define TINY_LIMIT 0.01F
#define BRUSH_MINSIZE 1.0F // Minimum scale per axis

typedef float rectdef[4];

typedef enum paneltype_t
{
  PANEL_TOP,
  PANEL_FRONT,
  PANEL_SIDE,
  PANEL_3D,
  PANEL_CONTEXT, // KEEP THIS ORDER, ALL BRUSH-RELATED GO ABOVE THIS
  PANEL_TOOLS,
  PANEL_COUNT
} paneltype_t;

typedef struct panel_t
{
  const char* name;
  camera_t* camera;
  rectdef rect;
  u8 type;
  int axis_a, axis_b;
} panel_t;

typedef struct brushdraw_t
{
  vec3_t a, b;
} brushdraw_t;

typedef struct winding_s winding_t;
typedef struct facehighlight_t {
  CBaseMesh* mesh;
  winding_t* winding;
} facehighlight_t;

typedef struct face_s face_t;
typedef struct brush_s brush_t;
extern panel_t gPanels[6];

extern int8_t gHoveredPanel;
extern int gGridLevel;

extern brush_t* gBrushes;
extern u32 gBrushCount;
extern brush_t* gHoveredBrush;
extern int gHoveredBrushFace;
extern int gHoveredBrushFacePrevious;

extern u8 gHoveredPanelValid;
extern u8 gBrushDrawing;
extern brushdraw_t gBrushDraw;

extern facehighlight_t gFaceHighlight;

extern face_t* gFaceTarget;
extern brush_t* gBrushTarget;

typedef enum {ECMD_BRUSHCREATE, ECMD_BRUSHDELETE} ecmdtype_t;

typedef struct ecmd_t
{

  ecmdtype_t type;
  brush_t* brush;
  union
  {
    struct 
    {
      vec3_t mins, maxs;
    } brush_create;
    struct 
    {
      qangle newangles;
    } brush_rotate;
  };
} ecmd_t;

void PanelInput();

// brush.c
brush_t* Brush_Create(vec3_t mins, vec3_t maxs);
void Brush_Delete(brush_t** b);
void Brush_DeleteAll();
void Brush_SetFaceScale(brush_t* b, int face, float sx, float sy);
void Brush_BuildAllFaces(brush_t* b);

// Output brushes are allocated by this function, do not pre-allocate
void Brush_Splice(brush_t* in, const plane_t split, brush_t** front, brush_t** back);

// draw.c
void R_DrawBrush(brush_t* b);
void R_DrawBrushes();
void R_DrawFaceHighlight();


// ecmd.c
void ECMD_BrushCreate(vec3_t min, vec3_t max);

void ECMD_Init();
void ECMD_Flush();


// gui.c
void CalculatePanels();


#define WINDING_MAX_POINTS 64
#define BRUSH_MAX_PLANES 64

enum content_t
{
  BRUSH_SOLID,
  BRUSH_EMPTY,
  BRUSH_WATER,
};

// How many world units the space between grid lines represent

extern float gGridSizes[];

enum
{
  GRID_SMALL,
  GRID_MED,
  GRID_LARGE,
  GRID_HUGE,
};

typedef enum
{
  CONTEXT_FACE = 1,
  CONTEXT_ENTITY = 2,
} panelcontext_t;


extern panelcontext_t gPanelContext;

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

static float CurrentGridSize(float zoom)
{
  int level = gGridLevel;
  float grid = gGridSizes[level];
  while ((grid / zoom) < 16.0f && level < GRID_HUGE)
    grid = gGridSizes[++level];
  return grid;
}




void Camera_FarzInput();

void GUI_FaceContextMenu(face_t* f);
