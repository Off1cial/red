#include "game/shared/pmove.h"
#include "game/shared/cvar.h"

#include "game/client/cl_player.h"
#include "game/client/client.h"

void PM_UserFriction(vec3_t velocity, float dt)
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

void PM_Accelerate(vec3_t velocity, vec3_t wishdir, float wishspeed, float dt)
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

void PM_AirAccelerate(vec3_t velocity, vec3_t wishdir, vec3_t wishspeed, float dt);

void PM_Move(playercmd_t *cmd, vec3_t origin, vec3_t velocity, vec3_t forwardout, float dt)
{
  // 1. friction acts on EXISTING velocity, read/write in place
  PM_UserFriction(velocity, dt);

  // 2. wishdir/wishspeed computed fresh from this tick's input
  vec3_t forward, right;
  QAngleVector(cmd->viewangles, forward);
  VectorCrossNorm(forward, VEC_AXIS_Y, right);

  /*
  if (cmd->pm_up >= 1.0f)
    cmd->buttons |= PM_BUTTON_JUMP;

  if (origin[1] > 0)
    cmd->onground = 0;
  else
    cmd->onground = 1;

  if (cmd->onground && ((cmd->buttons & PM_BUTTON_JUMP) != 0))
  {
    printf("Jump\n");
    velocity[1] += 270.0f;
  }
  */

  vec3_t wishvel, wishdir;

  wishvel[0] = forward[0] * cmd->pm_forward * sv_maxspeed + right[0] * cmd->pm_side * sv_maxspeed;
  wishvel[1] = 0;
  wishvel[2] = forward[2] * cmd->pm_forward * sv_maxspeed + right[2] * cmd->pm_side * sv_maxspeed;

  VectorCopy(wishvel, wishdir);
  float wishspeed = VectorNormalise(wishdir); // normalizes wishvel in place, returns its old length
  if (wishspeed > sv_maxspeed)
    wishspeed = sv_maxspeed;

  // 3. accelerate — reads player->velocity, writes player->velocity
  PM_Accelerate(velocity, wishdir, wishspeed, dt);
  // velocity[1] -= 400.0f * dt;

  // 4. integrate position from the (now updated) velocity
  origin[0] += velocity[0] * dt;
  origin[1] += velocity[1] * dt;
  origin[2] += velocity[2] * dt;

  if (origin[1] < 0)
    origin[1] = 0;

  VectorCopy(forward, forwardout);
}
