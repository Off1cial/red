#include "editor/cammove.h"
#include "platform/input.h"

camcmd_t gCamcmd = {0};

void Camcmd_prepare()
{
  gCamcmd.forward = 0.0f;
  gCamcmd.side = 0.0f;
  gCamcmd.up = 0.0f;

  if (pltInput_KeyboardDown(SDL_SCANCODE_W))
    gCamcmd.forward = 1.0f;
  if (pltInput_KeyboardDown(SDL_SCANCODE_S))
    gCamcmd.forward = -1.0f;

  if (pltInput_KeyboardDown(SDL_SCANCODE_D))
    gCamcmd.side = 1.0f;

  if (pltInput_KeyboardDown(SDL_SCANCODE_A))
    gCamcmd.side = -1.0f;

  if (pltInput_KeyboardDown(SDL_SCANCODE_SPACE))
    gCamcmd.up = 1.0f;

  if (pltInput_KeyboardDown(SDL_SCANCODE_LCTRL))
    gCamcmd.up = -1.0f;
}

float movspeed = 140.0f;
void Camcmd_act(camera_t* camera, float dt)
{
  vec3_t front, right, up; 
  float movdelta = movspeed * dt;
  float movfront = movdelta * gCamcmd.forward;
  float movside = movdelta * gCamcmd.side;
  float movup = movdelta * gCamcmd.up;

  VectorScale(camera->front, movfront, front);
  VectorScale(camera->right, movside, right);
  VectorScale(VEC_AXIS_Y, movup, up);

  VectorAdd(camera->origin, front, camera->origin);
  VectorAdd(camera->origin, right, camera->origin);
  VectorAdd(camera->origin, up, camera->origin);

  //vec3print(camera->origin);

}
