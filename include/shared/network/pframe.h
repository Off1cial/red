#pragma once

#include "corebase/mathlib.h"

typedef struct playerframe_t
{
  vec3_t origin;
  vec3_t velocity;
  qangle viewangles;

  int pm_flags; // Player move flags - crouched, in air, etc

  // Not sent over network 
  int ping;

} playerframe_t;
