#include "editor/editor.h"
#include "platform/input.h"
#include "platform/common.h"

#define SCROLL_DEADZONE 0.01f
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
  int movscale = gGridLevel + 1;
  if (ValueOverDeadzone(gPltInput->mscrl_x, SCROLL_DEADZONE))
    *camx = (*camx) + (gPltInput->mscrl_x * movscale);
  if (ValueOverDeadzone(gPltInput->mscrl_y, SCROLL_DEADZONE))
    *camy = (*camy) + (gPltInput->mscrl_y * movscale);

  if (pltInput_KeyboardPress(SDL_SCANCODE_RIGHTBRACKET))
    cam->fov -= 10.0f;

  if (pltInput_KeyboardPress(SDL_SCANCODE_LEFTBRACKET))
    cam->fov += 10.0f;

  clampf(&cam->fov, 10.0f, 90.0f);
}
