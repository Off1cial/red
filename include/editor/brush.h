#pragma once

#include <stdint.h>
#include "corebase/mathlib.h"
#include "editor/draw.h"

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
  int maxpoints;
  vec3_t points[];
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


typedef struct brush_s
{
  struct brush_s *next, *prev;
  u8 contents;
  vec3_t mins, maxs;
  u8 facecount;
  u8 facecapacity;
  face_t* faces;

  // Rendering data
  brushrender_t* renderable;
} brush_t;


u8 Brush_AddToList(brush_t* b, brush_t* list);
u8 Brush_RemoveFromList(brush_t* b, brush_t* list);

// Creates a 6-faced brush with the specified bounds
brush_t* Brush_Create(vec3_t mins, vec3_t maxs);


