#pragma once


#include "corebase/mathlib.h"

//void PM_Accelerate(vec3_t velocity, vec3_t wishdir, float wishspeed, float dt);
//void PM_UserFriction(vec3_t velocity, float dt);
typedef struct playercmd_t playercmd_t;

#define PM_BUTTON_JUMP 1
#define PM_BUTTON_CROUCH 2


void PM_Move(playercmd_t* cmd, vec3_t origin, vec3_t velocity, vec3_t forwardout, float dt);
