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

void PM_Move(playercmd_t* cmd, vec3_t origin, vec3_t velocity, vec3_t forwardout, float dt)
{
    // 1. friction acts on EXISTING velocity, read/write in place
    PM_UserFriction(velocity, dt);

    // 2. wishdir/wishspeed computed fresh from this tick's input
    vec3_t forward, right, wishdir;
    QAngleVector(cmd->viewangles, forward);
    VectorCrossNorm(forward, VEC_AXIS_Y, right);


    if (cmd->pm_up >= 1.0f)
      cmd->buttons |= PM_BUTTON_JUMP;
  
    if (!cmd->onground && ((cmd->buttons & PM_BUTTON_JUMP) != 0))
    {
      velocity[1] += 20.0f;
    }

    wishdir[0] = forward[0] * cmd->pm_forward + right[0] * cmd->pm_side;
    wishdir[1] = 0;
    wishdir[2] = forward[2] * cmd->pm_forward + right[2] * cmd->pm_side;

    float wishspeed = VectorNormalise(wishdir); // normalizes wishdir in place, returns its old length
    if (wishspeed > 1.0f)
      wishspeed = 1.0f;
    wishspeed *= 20;
    if (wishspeed > sv_maxspeed) wishspeed = sv_maxspeed;

    // 3. accelerate — reads player->velocity, writes player->velocity
    PM_Accelerate(velocity, wishdir, wishspeed, dt);

    // 4. integrate position from the (now updated) velocity
    origin[0] += velocity[0] * dt;
    origin[1] += velocity[1] * dt;
    origin[2] += velocity[2] * dt;

    VectorCopy(forward, forwardout);
}


