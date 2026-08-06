#pragma once

#include "corebase/mathlib.h"
#include "engine/camera.h"
#include "editor/brush.h"

typedef float rectdef[4];


typedef struct panel_t
{
  // Each holds a vertex array?
  const char* name;
  camera_t* camera;
  rectdef rect;
  u8 type;
  int axis_a, axis_b;
} panel_t;

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
typedef struct brush_s brush_t;
// Somewhat better...
typedef union ecmd_t
{
  ecmdtype_t type;
  brush_t* brush;
  struct 
  {
    vec3_t mins, maxs;
  } brush_create;
  struct 
  {
    qangle newangles;
  } brush_rotate;

} ecmd_t;

void PanelInput();


void ECMD_BrushCreate(vec3_t min, vec3_t max);
void ECMD_BrushDelete(brush_t* b);

void ECMD_Init();
void ECMD_Flush();
