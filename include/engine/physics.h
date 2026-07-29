#pragma once

#include "corebase/mathlib.h"
#include "engine/simd.h"
#include <stdint.h>


// Server side data, the client can just store positions?
typedef struct PHYS_SIMD_BODIES
{
  simdarr_vec3_t origin;
  simdarr_vec3_t velocity;
  simdarr_vec3_t acceleration;

  // Collision
  simdarr_vec3_t aabb_min;
  simdarr_vec3_t aabb_max;
  // hulls?
  float* invmass;

  size_t capacity;
  size_t count;

} CBasePhysBodies;


int CBasePhysBodies_Init(CBasePhysBodies* array, size_t capacity);
void CBasePhysBodies_Free(CBasePhysBodies* array);


uint32_t CBasePhysBodies_Create(CBasePhysBodies* arr,
    vec3_t origin, 
    vec3_t velocity, 
    vec3_t acceleration,
    float mass,
    vec3_t aabb_min,
    vec3_t aabb_max
    );




