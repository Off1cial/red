#pragma once

#include "corebase/mathlib.h"
#include "engine/camera.h"
#include "engine/assetmanager.h"

typedef float rectdef[4];

typedef enum paneltype_t
{
  PANEL_TOP,
  PANEL_FRONT,
  PANEL_SIDE,
  PANEL_TOOLS, // KEEP THIS ORDER
  PANEL_3D,
  PANEL_COUNT
} paneltype_t;

typedef struct panel_t
{
  // Each holds a vertex array?
  const char* name;
  camera_t* camera;
  rectdef rect;
  u8 type;
  int axis_a, axis_b;
} panel_t;

typedef struct face_s face_t;
typedef struct brush_s brush_t;
extern panel_t gPanels[5];

extern int8_t gHoveredPanel;
extern int gGridLevel;

extern brush_t* gBrushes;
extern u32 gBrushCount;

extern u8 gHoveredValid;

typedef enum {ECMD_BRUSHCREATE, ECMD_BRUSHDELETE} ecmdtype_t;

// Editor command
/*
typedef struct ecmd_t
{
  ecmdtype_t type;
  void* data; // yummy
  u64 datasize; // so shit, what the fuck am i doing

} ecmd_t;

*/
// Somewhat better...
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


void ECMD_BrushCreate(vec3_t min, vec3_t max);
void ECMD_BrushDelete(brush_t* b);

void ECMD_Init();
void ECMD_Flush();

#define WINDING_MAX_POINTS 64
#define BRUSH_MAX_PLANES 64

enum
{
  BRUSH_SOLID,
  BRUSH_EMPTY,
  BRUSH_WATER,
};

typedef struct winding_t
{
  int numpoints;
  vec3_t points[WINDING_MAX_POINTS];
} winding_t;

typedef struct brushmaterial_t
{
  u32 texhandle;
  vec3_t uaxis, vaxis;
  vec2_t shift;
  vec2_t scale;
} brushmaterial_t;



typedef struct face_s
{
  plane_t plane;
  winding_t* winding;
  brushmaterial_t material;

} face_t;


typedef struct brushrender_t
{
  CBaseMesh* mesh;
  int old; // Recompute mesh
} brushrender_t;





typedef struct brush_s
{
  struct brush_s *next, *prev;
  u8 contents;
  vec3_t mins, maxs;
  u32 facecount;
  face_t faces[BRUSH_MAX_PLANES];

  // Rendering data
  brushrender_t* renderable;
} brush_t;




// Render surface

typedef struct rsurfbatch_t
{
  u32 texhandle;
  CBaseMesh* mesh;
  int old;  

} rsurfbatch_t;

// Accessed by texture handles
extern rsurfbatch_t gSurfbatches[ASSETS_MAX_TEXTURES];
extern u32 gUniqueTexs; // Number of different textures being used 

void FaceUVpoint(face_t* f, vec3_t p, vec2_t out);

void rsurfbatch_addface(face_t f);
void rsurfbatch_reset(u32 texhandle);
