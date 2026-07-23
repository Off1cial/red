#pragma once

#include "corebase/mathlib.h"



// Server side data, the client can just store positions?
typedef struct PHYS_SIMD_BODIES
{
  vec3_t* origin;
  vec3_t* velocity;
  vec3_t* acceleration;

  // Collision
  vec3_t* aabb_mins;
  vec3_t* aabb_maxs;
  // hulls?
  float* invmass;

  size_t capacity;
  size_t count;

} CBasePhysBodies;


int CBasePhysBodies_Init(CBasePhysBodies* array, size_t capacity);
void CBasePhysBodies_Free(CBasePhysBodies* array);




