#pragma once

#include "game/server/server.h"

#define PMF_ONGROUND 1
#define PMF_CROUCHED 2


void SV_Accelerate(vec3_t velocity, vec3_t wishdir, float wishspeed, float dt);
void SV_UserFriction(vec3_t velocity, float dt);