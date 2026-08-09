#pragma once

#include <limits.h>
#include "corebase/mathlib.h"
#define BSP_MAX_PLANES 2048

#define MAX_MAP_VERTICES INT_MAX
#define MAX_MAP_EDGES INT_MAX


typedef struct edge_s
{
  u32 v[2];
} edge_t;

extern vec3_t bvertices[MAX_MAP_VERTICES];
extern edge_t bedges[MAX_MAP_EDGES];
extern plane_t bplanes[BSP_MAX_PLANES];

