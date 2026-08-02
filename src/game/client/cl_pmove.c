#include "game/client/client.h"
#include "game/client/cl_player.h"
#include "platform/input.h"
#include "engine/camera.h"


void CL_CreatePlayerCommand(float dt)
{
  playercmd_t cmd = {0};
  float forward = 0.0f;
  float side = 0.0f;
  float up = 0.0f;
  if (gPltInput->kCurrent[SDL_SCANCODE_W])
    forward += 1.0f;
  if (gPltInput->kCurrent[SDL_SCANCODE_S])
    forward -= 1.0f;
  if (gPltInput->kCurrent[SDL_SCANCODE_D])
    side += 1.0f;
  if (gPltInput->kCurrent[SDL_SCANCODE_A])
    side -= 1.0f;
  if (gPltInput->kCurrent[SDL_SCANCODE_SPACE])
    up = 1.0f;
  cmd.sequence = gClient->cmdcount;
  
  cmd.pm_forward = forward;
  cmd.pm_side = side;
  cmd.pm_up = up;

  cmd.viewangles[PITCH] = gCamera->angles[PITCH];
  cmd.viewangles[YAW] = gCamera->angles[YAW];
  cmd.viewangles[ROLL] = gCamera->angles[ROLL];

  gClient->cmds[gClient->cmdcount % CLIENT_CMD_BACKUP] = cmd;
  gClient->cmdcount++;
}





#include "game/shared/pmove.h"
void CL_PMove(float dt)
{
  playercmd_t cmd = {0};
  float forward = 0.0f;
  float side = 0.0f;
  float up = 0.0f;
  if (gPltInput->kCurrent[SDL_SCANCODE_W])
    forward += 1.0f;
  if (gPltInput->kCurrent[SDL_SCANCODE_S])
    forward -= 1.0f;
  if (gPltInput->kCurrent[SDL_SCANCODE_D])
    side += 1.0f;
  if (gPltInput->kCurrent[SDL_SCANCODE_A])
    side -= 1.0f;

  cmd.sequence = gClient->cmdcount;
  
  cmd.pm_forward = forward;
  cmd.pm_side = side;
  cmd.pm_up = up;

  cmd.viewangles[PITCH] = gCamera->angles[PITCH];
  cmd.viewangles[YAW] = gCamera->angles[YAW];
  cmd.viewangles[ROLL] = gCamera->angles[ROLL];

  PM_Move(&cmd, gPlayer.origin, gPlayer.velocity, gPlayer.lookdir, dt);
  VectorCopy(gPlayer.origin, gCamera->origin);
}
