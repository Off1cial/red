#include "game/server/sv_player.h"
#include "game/server/server.h"
#include "engine/physics.h"
#include "game/shared/cvar.h"

// Movement code that doesnt use the physics engine yet?
// No... get the wishdir from the pframe, and velocity etc from the physbody

float wishspeed = 320.0f;

/*
void SV_Accelerate(svplayer_t* player, float accel)
{
  vec3_t bodyvelocity;
  vec3_t forward;
  vec3_t right;

  VectorCrossNorm(forward, VEC_AXIS_Y, right);
  playercmd_t* cmd = &player->cmd;
  QAngleVector(player->cmd.viewangles, forward);
  vec3_t wishdir =
  {
    forward[0] * cmd->pm_forward + right[0] * cmd->pm_side,
    cmd->pm_up,
    forward[2] * cmd->pm_forward + right[2] * cmd->pm_side
  };
  uint32_t body = player->physbody;
  Physbody_GetVelocity(body, bodyvelocity);
  float speed_lookdir = DotProduct(bodyvelocity, wishdir);

  // addspeed
  float addspeed = wishspeed - speed_lookdir;
  float accelspeed = accel * SERVER_TICKRATE * wishspeed;
  if (accelspeed > addspeed)
    accelspeed = addspeed;

  vec3_t acceleration =
  {
    wishdir[0] * accelspeed,
    wishdir[1] * accelspeed,
    wishdir[2] * accelspeed
  };

  Physbody_AddAccel(body, acceleration);
}
*/

void SV_UserFriction(vec3_t velocity, float dt)
{
  vec3_t vel;
  VectorCopy(velocity, vel);
  float speed = sqrtf(vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2]);
  if (speed < 0.0001f)
    return;

  float control = speed < sv_stopspeed ? sv_stopspeed : speed;
  float drop = control * sv_friction * dt;

  float newspeed = speed - drop;
  if (newspeed < 0)
    newspeed = 0;
  newspeed /= speed;

  vel[0] *= newspeed;
  vel[1] *= newspeed;
  vel[2] *= newspeed;
  VectorCopy(vel, velocity);
}

void SV_Accelerate(vec3_t velocity, vec3_t wishdir, float wishspeed, float dt)
{
  float currentspeed = DotProduct(velocity, wishdir);
  float addspeed = wishspeed - currentspeed;
  if (addspeed <= 0)
    return;

  float accelspeed = sv_accelerate * dt * wishspeed;
  if (accelspeed > addspeed)
    accelspeed = addspeed;

  velocity[0] += accelspeed * wishdir[0];
  velocity[1] += accelspeed * wishdir[1];
  velocity[2] += accelspeed * wishdir[2];
}
