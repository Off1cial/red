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
#include "editor/cammove.h"

float cl_updaterate = 1.0f / 128.0f;

int8_t gHoveredPanel = -1;

CBaseShader* gEditorShader_brush;

int main()
{
  printf("Hello World!\n");



  pltWindow* Window = PlatformWindow_Create(800, 600, "Editor");
  pltInput* PltInput   = PlatformInput_Create();
  pltMemArena* MemArena = PlatformMemArena_Create(PLT_MEM_ARENA_GSIZE);
  gPltWindow = Window; gPltInput = PltInput; gPltMem = MemArena;


  Mat4Identity(MAT4_IDENTITY); // Move this to init
  camera_t* camera = Camera_Create(
      VEC_ZERO, 
      VEC_AXIS_Z, 
      (cViewport){0,0,gPltWindow->winw, gPltWindow->winh});

  gCamera = camera;
  


  pltTime_Init();
  AssetManager_Init();
  
  gEditorShader_brush = CBaseShader_Create(
      "../Assets/Shaders/vert_unlit.vs", "../Assets/Shaders/frag_unlit.fs");
    
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
    gHoveredPanelValid = (gHoveredPanel != -1);  


    PlatformInput_Poll(gPltWindow->window, gPltInput, &quit);
    Camcmd_prepare();

    if (accumulator >= cl_updaterate)
    {
      Camcmd_act(camera, cl_updaterate);
      accumulator -= cl_updaterate;
    }
    if (gPltInput->eventWindowResized)
    {
      gPltWindow->winw = gPltInput->eventWindowNewWidth;
      gPltWindow->winh = gPltInput->eventWindowNewHeight;
      CalculatePanels();
    }
    gHoveredPanel = -1;
    
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
    if (gBrushCount > 0) {
      R_DrawBrushes();
    }


    while (glGetError() != GL_NO_ERROR)
      printf("GL error\n");

    SDL_GL_SwapWindow(gPltWindow->window);

  }
  Brush_DeleteAll();
  PlatformInput_Destroy(gPltInput);
  PlatformWindow_Destroy(gPltWindow);
}
