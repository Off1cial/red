#include "game/server/server.h"
#include "game/shared/cvar.h"

void SV_AddGravity(vec3_t velocity, float scale, float dt)
{
  velocity[1] -= scale * sv_gravity * dt;
}


