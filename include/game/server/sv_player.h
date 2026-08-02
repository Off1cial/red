#pragma once

#include <stdint.h>

#include "shared/network/pframe.h"

// FOR EXAMPLE
typedef struct svplayer_s
{
  playercmd_t cmd; // This ticks input
  playerframe_t frame; // Output at the end of the tick
  int clientid;

  // Simulation state
  vec3_t origin;
  vec3_t velocity;
  int onground;
  int pm_flags;

  float respawn_time;
  uint32_t physbody;

} svplayer_t;

void SV_PlayerThink(svplayer_t* player, float dt);
