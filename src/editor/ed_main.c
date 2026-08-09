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
#include "editor/draw.h"

#include "editor/cammove.h"


int8_t gHoveredPanel = -1;

CBaseShader* gEditorShader_brush;

int main()
{
  printf("Hello World!\n");



  pltWindow* Window = PlatformWindow_Create(640, 480, "Editor");
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
  
  CBaseShader* gEditorShader_brush = CBaseShader_Create(
      "../Assets/Shaders/vert_unlit.vs", "../Assets/Shaders/frag_unlit.fs");
    
  Mat4Identity(MAT4_IDENTITY); // Move this to init
  ECMD_Init();
  UI_Init();
  GUI_Initialise();

  double timestamp = pltTime_Time();
  double accumulator = 0.0f;



int quit = 0;
  uint8_t sampletxtcol[4] = {180, 20 ,50, 255};
  while (!quit)
  {
    double time = pltTime_Time();
    double dt = time - timestamp;
    accumulator += dt;
    timestamp = time;
    gHoveredValid = (gHoveredPanel != -1);  


    PlatformInput_Poll(gPltWindow->window, gPltInput, &quit);
    Camcmd_prepare();

    if (accumulator >= 1.0f / 64)
    {
      Camcmd_act(camera, dt);
      accumulator -= (1.0 / 64);
    }

    
    PanelInput();
    
    ECMD_Flush();






    // Rendering
    glClearColor(0.16, 0.16, 0.16, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Camera_Look(camera, gPltInput->mxrel, gPltInput->myrel, 0.4f);
    Camera_Update(camera);
    UI_FrameBegin();

    GUI_Draw();
  
    //UI_AddText("Balls", 0, 0, 0, sampletxtcol); 
    
    
    UI_FrameEnd();

    CBaseShader_Use(gEditorShader_brush);
    CBaseShader_SetMat4(gEditorShader_brush, SH_UNIFORM_VIEW, camera->view);
    CBaseShader_SetMat4(gEditorShader_brush, SH_UNIFORM_PROJECTION, camera->projection);
    CBaseShader_SetMat4(gEditorShader_brush, SH_UNIFORM_MODEL,  MAT4_IDENTITY);
    //R_DrawBrushes(gBrushes);
    R_DrawSurfaces();


    while (glGetError() != GL_NO_ERROR)
      printf("GL error\n");

    SDL_GL_SwapWindow(gPltWindow->window);

  }
  PlatformInput_Destroy(gPltInput);
  PlatformWindow_Destroy(gPltWindow);
}
