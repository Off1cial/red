#pragma once

#include "shared/network/pframe.h"



typedef struct clplayer_s
{
  playerframe_t pframes[3];
  float interpolation;

  // Predictions
  vec3_t origin;
  vec3_t velocity;
  vec3_t lookdir;
  int onground;
  // Animations, models, particles

} clplayer_t;

extern clplayer_t gPlayer;
