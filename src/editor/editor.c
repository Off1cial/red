#include "editor/editor.h"
#include "platform/input.h"
#include "platform/common.h"

#define SCROLL_DEADZONE 0.05f
u8 gHoveredValid = 0;

static u8 ValueOverDeadzone(float val, float zone)
{
  return (fabsf(val) >= zone);
}

void PanelInput()
{
  if (!gHoveredValid)
    return;
  
  panel_t* p = &gPanels[gHoveredPanel];
  camera_t* cam = p->camera;
  float* camx = &cam->origin[p->axis_a];
  float* camy = &cam->origin[p->axis_b];
  float movscale = fmaxf(cam->fov, 0.1f);
  
  if (ValueOverDeadzone(gPltInput->mscrl_x, SCROLL_DEADZONE))
    *camx = (*camx) + (gPltInput->mscrl_x * movscale);
  if (ValueOverDeadzone(gPltInput->mscrl_y, SCROLL_DEADZONE))
    *camy = (*camy) + (gPltInput->mscrl_y * movscale);

  if (pltInput_KeyboardPress(SDL_SCANCODE_RIGHTBRACKET))
    cam->fov *= 0.9f;

  if (pltInput_KeyboardPress(SDL_SCANCODE_LEFTBRACKET))
    cam->fov *= 1.1f;


  //printf("CAM FOV PreC = %0.2f\n", cam->fov);
  clampf(&cam->fov, 0.01f, 1.0f);
  //printf("CAM FOV PstC = %0.2f\n", cam->fov);
}
