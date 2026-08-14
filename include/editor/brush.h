#ifndef BRUSH_H
#define BRUSH_H

#include "editor/brushmesh.h"
#include "corebase/mathlib.h"

#define MAX_WINDING_POINTS 64
#define MAX_BRUSH_FACES 64
#define MAX_BRUSHES 32768

typedef struct winding_s
{
  u16 pointcount;
  u16 pointcap;
  vec3_t points[]; 
} winding_t;

typedef struct facematerial_t
{
  u32 texhandle;
  vec2_t shift, scale;
  vec3_t uaxis, vaxis;
} facematerial_t;

typedef struct face_s
{
  winding_t* win;
  plane_t plane;
  facematerial_t material;
  u8 changed;
} face_t;



typedef struct brush_s
{
  brushmesh_t mesh;
  vec3_t mins;
  vec3_t maxs;
  struct brush_s* next;
  struct brush_s* prev;



  u32 numfaces;
  u8 changed;
  face_t* faces[];
} brush_t;

winding_t* AllocWinding(const int numpoints);
winding_t* HugeWinding(plane_t plane);
void       ClipWinding(winding_t* in, const plane_t p, winding_t** out);

face_t* AllocFace(plane_t p, const int numpoints);
void FreeFace(face_t** f);

brush_t* AllocBrush(const int facecount);
void FreeBrush(brush_t** b);


#endif
