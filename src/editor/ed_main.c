#include <stdio.h>

#include "platform/window.h"
#include "platform/input.h"
#include "corebase/time.h"
#include "corebase/mathlib.h"
#include "engine/ui/ui.h"

#include "engine/assetmanager.h"

int main()
{
  printf("Hello World!\n");


  pltWindow* gPltWindow = PlatformWindow_Create(1280, 720, "Editor");
  pltInput* gPltInput   = PlatformInput_Create();
  
  AssetManager_Init();
  UI_Init();
  pltTime_Init();
  double timestamp = pltTime_Time();
  int quit = 0;

  uint8_t sampletxtcol[4] = {180, 20 ,50, 255};
  while (!quit)
  {
    double time = pltTime_Time();
    double dt = time - timestamp;
    timestamp = time;

    PlatformInput_Poll(gPltWindow->window, gPltInput, &quit);
    //UI_FrameBegin();
    glClearColor(0.1, 0.5, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //UI_AddText("Balls", 0, 0, 0, sampletxtcol); 

    //UI_FrameEnd();
    SDL_GL_SwapWindow(gPltWindow->window);
  }
  PlatformInput_Destroy(gPltInput);
  PlatformWindow_Destroy(gPltWindow);
}
