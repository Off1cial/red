#pragma once

#include <stdint.h>
#include "corebase/mathlib.h"

#define BRUSH_MAX_PLANES 64

typedef struct side_s
{
  int planenum;
  // material
} side_t;

typedef struct brush_s
{
  // all data
  // ....
  // Expandable
  side_t sides[];
} brush_t;