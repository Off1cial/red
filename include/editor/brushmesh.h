#pragma once

// Specialised renderer for the editor that is shit and unoptimised
#include "engine/mesh.h"
#include "corebase/mathlib.h"

#define BRUSHMESH_TEXHASH_BUCKETS 96

typedef struct brushsurface_t
{
  int texhandle;
  u32 firstindex; // Index into the brush mesh
  u32 indexcount;
} brushsurface_t;

typedef struct texbucketent_t
{
  int texhandle;
  int surface_index;
  struct texbucketent_t* next;
} texbucketent_t;

typedef struct panelbounds_t
{ 
  int paneltype;
  vec2_t mins, maxs;
} panelbounds_t;

typedef struct brushmesh_s
{
  CBaseMesh* mesh;
  
  // Approximate bounds for fast hover discard
  panelbounds_t topbounds, frontbounds, sidebounds;

  brushsurface_t* surfaces;
  int surfacecount;
  int surfacecapacity;
} brushmesh_t;
