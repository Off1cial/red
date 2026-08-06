#include <stdio.h>

#include "platform/window.h"
#include "platform/input.h"
#include "platform/memarena.h"
#include "corebase/time.h"
#include "corebase/mathlib.h"
#include "engine/ui/ui.h"
#include "engine/camera.h"

#include "engine/assetmanager.h"

#include "editor/editor.h"
#include "editor/gui.h"


int8_t gHoveredPanel = -1;

int main()
{
  printf("Hello World!\n");



  pltWindow* Window = PlatformWindow_Create(1280, 720, "Editor");
  pltInput* PltInput   = PlatformInput_Create();
  pltMemArena* MemArena = PlatformMemArena_Create(PLT_MEM_ARENA_GSIZE);
  gPltWindow = Window; gPltInput = PltInput; gPltMem = MemArena;


  camera_t* camera = Camera_Create(
      VEC_ZERO, 
      VEC_AXIS_Z, 
      (cViewport){0,0,gPltWindow->winw, gPltWindow->winh});

  gCamera = camera;

  pltTime_Init();
  AssetManager_Init();
  ECMD_Init();
  UI_Init();
  GUI_Initialise();
  double timestamp = pltTime_Time();
  int quit = 0;
  uint8_t sampletxtcol[4] = {180, 20 ,50, 255};
  while (!quit)
  {
    double time = pltTime_Time();
    double dt = time - timestamp;
    timestamp = time;
    gHoveredValid = (gHoveredPanel != -1);  

    PlatformInput_Poll(gPltWindow->window, gPltInput, &quit);
    
    /*
    if (gHoveredPanel != -1 && (fabsf(gPltInput->mscrl_y) > 0.0f))
      gPanels[gHoveredPanel].camera->fov += gPltInput->mscrl_y * 0.1f;
    */

    PanelInput();
    
    ECMD_Flush();






    // Rendering
    glClearColor(0.16, 0.16, 0.16, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    UI_FrameBegin();

    GUI_Draw();
  
    UI_AddText("Balls", 0, 0, 0, sampletxtcol); 

    UI_FrameEnd();
    SDL_GL_SwapWindow(gPltWindow->window);

  }
  PlatformInput_Destroy(gPltInput);
  PlatformWindow_Destroy(gPltWindow);
}
