#pragma once

#include "corebase/mathlib.h"

// Crucial information that the server and client may need
typedef struct playerinfo_t
{
  vec3_t origin;
  vec3_t velocity;
  qangle viewangles;

  // Animation frames?
} playerinfo_t;

typedef struct player_t
{  
  vec3_t origin;

} player_t;