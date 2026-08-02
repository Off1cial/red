#pragma once

// Only the necessary information shared between client and server
#include <stdint.h>

#include "corebase/mathlib.h"


// THIS GOES TO THE SERVER, FROM THE CLIENT
typedef struct playercmd_t
{
  uint32_t sequence;

  float pm_forward;
  float pm_side;
  float pm_up;
  int onground;
  qangle viewangles;

  uint32_t buttons;

} playercmd_t;


// THIS COMES BACK FROM THE SERVER
typedef struct playerframe_t
{
  vec3_t origin;
  vec3_t lookdir;
  vec3_t velocity;

  uint32_t pm_flags; // Player move flags - crouched, in air, etc

  int ping; 

} playerframe_t;


