#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "platform/window.h"
#include "platform/cpu.h"
#include "platform/input.h"

#include "corebase/time.h"
#include "corebase/mathlib.h"

#include "engine/simd.h"
#include "engine/shader.h"
#include "engine/mesh.h"
#include "engine/camera.h"
#include "engine/ui/ui.h"
#include "engine/assetmanager.h"

#include "game/client/client.h"
#include "game/client/cl_console.h"
#include "shared/network/packet.h"

// cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
// ninja -C build
 
// https://www.reddit.com/r/playboicarti/s/lpdLQ9IYHt



// test triangle

void ASM_ADDTEST(float* dst, float* a, float* b, int count)
{
  for (int i = 0; i < count; i++)
  {
    dst[i] = a[i] + b[i];
  }
}

const gpuVertex v0 = {
  .xyz = {-0.5, -0.5, 0.5},
  .col = {1.0, 0.0, 0.0}
};

const gpuVertex v1 = {
  .xyz = {0.0, 0.5, 0.5},
  .col = {0.0, 1.0, 0.0}
};

const gpuVertex v2 = {
  .xyz = {0.5, -0.5, 0.5},
  .col = {0.0, 0.0, 1.0}
};

client_t* gClient;


void CL_Loop(client_t* client, double dt)
{
  
  if (client->socket_udp != -1)
  {
    netpacket_t newmsg;
    int res = CL_GetMessage(&newmsg);
    ///if (res == CLIENT_FAILURE)
    //return;
    CL_ParseMessage(&newmsg);

  }
  

  CL_Think(dt);
}


int main()
{
  printf("Hello world\n");
  cpufeatures_t cpufeat;
  gPltCPUFeatures = &cpufeat;
  pltCPU_GetFeatures();
  pltCPU_PrintFeatures();

  pltWindow* win = PlatformWindow_Create(854,480, "RED");
  gPltWindow = win; // clean this up later
  pltInput* input = PlatformInput_Create();
  gPltInput = input;
  

  pltTime_Init();

  AssetManager_Init();
  UI_Init();
  
  CBaseShader* shadertest = CBaseShader_Create("../Assets/Shaders/vert_test.vs", "../Assets/Shaders/frag_test.fs");
  CBaseShader* shader     = CBaseShader_Create("../Assets/Shaders/vert_unlit.vs", "../Assets/Shaders/frag_unlit.fs");
 
  Mat4Identity(MAT4_IDENTITY);

  CBaseMesh* mTriangle = CBaseMesh_Create(3, 3);
  CBaseMesh_PushTriangleVerts(mTriangle, v0, v1, v2);
  CBaseMesh_Upload(mTriangle, GL_STATIC_DRAW);

  camera_t* camera = Camera_Create(VEC_ZERO, VEC_AXIS_Z, (cViewport){0,0,1280,720});
  
  gCamera = camera;
  glViewport(0,0, win->winw, win->winh);

  char msg_disconnect[] = "Chode";
  size_t msg_disconnect_len = strlen(msg_disconnect);

  gClient = malloc(sizeof(client_t));
  CL_Init(gClient, "redw0od0");

  //CL_Connect(client, "127.0.0.1", SERVER_PORT, NET_PROTOCOL_UDP);
  
  
  //netpacket_t connectpacket = {0};
  //connectpacket.type = NET_PACKET_CONNECT;

  //CL_SendPacketUDP(client, &connectpacket);




  netpacket_t testpacket = {0};
  testpacket.type = 99;
  testpacket.sequence = 0;
  char* str = "Hello!";
  int len = strlen(str);
  testpacket.size = len;
  strcpy(testpacket.data, str);
 
  // ASM Testing
  float ra[14] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27};
  float rb[14] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24 ,26, 28};
  float rc[14];
  
  int res = AVX_AddArrays(rc, ra, rb, 14);
  //int a = AVX_AddValue(rc, ra, 1.0f, 14);
  printf("Res = %d\n", res);
  for (int i = 0; i < 14; i++)
  {
    printf("%0.2f ", rc[i]);
  }
  printf("\n");


  double timestamp = pltTime_Time();
  int quit = 0;
  int con_attempts = 0;
  SDL_GL_SetSwapInterval(1);
  while(!quit)
  {

    double time = pltTime_Time();
    double dt = time - timestamp;
    float fps = 1 /dt;



    timestamp = time;

    glClearColor(0.12f, 0.1f, 0.18f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
      
    PlatformInput_Poll(gPltWindow->window, input, &quit); 
    //CL_PMove(dt);
    float speed = VectorLength(gPlayer.velocity);
    // Run on other thread?
    CL_Loop(gClient, dt);
    UI_FrameBegin();
    // Check for window resize
    if (input->eventWindowResized)
    {
      PlatformWindow_Resize(
          win,
          input->eventWindowNewWidth,
          input->eventWindowNewHeight
      );
    }
    
    rectdef rect = {40,40,100,50};
    rectdef windowrect = {20, 20, 400, 200};
    rectdef win2; 
    UIRECT_NULL(win2);
    UIRECT_NULL(rect);
    rgba texcol = {255, 53, 180, 255};
    char speedbuff[32];
    snprintf(speedbuff, 32, "%0.1f", fps);
    UI_AddText(speedbuff, 0, 80, 30, UI_COLOR_GREEN);
    if (UI_Begin("Window", windowrect, 0))
    {
      if (UI_Button("Button", rect))
      {
        printf("Clicked\n");
        if (gClient->state != CSTATE_CONNECTED)
          CL_GameServerJoin(gClient, "127.0.0.1", SERVER_PORT, NET_PROTOCOL_UDP, 2);
      }
      if (UI_Button("Disconnect", rect))
      {
        CL_GameServerDisconnect(gClient, "Im leaving", 10);
      }
    }
    vec2_t p0, p1;
    p0[0] = 0.0f;
    p0[1] = 0.0f;
    p1[0] = 200;
    p1[1] = 200;
    UI_DrawLine(p0, p1, COL32(255, 200, 100, 255), 1.0f);
    UI_End();

    //Console_Draw();

    Camera_Look(camera, input->mxrel, input->myrel, 0.8f);
    Camera_Update(camera);
    CBaseShader_Use(shader);
    CBaseShader_SetMat4(shader, SH_UNIFORM_VIEW, camera->view);
    CBaseShader_SetMat4(shader, SH_UNIFORM_PROJECTION, camera->projection);
    CBaseShader_SetMat4(shader, SH_UNIFORM_MODEL, MAT4_IDENTITY);

    CBaseMesh_Draw(testmodel.mesh, GL_TRIANGLES);
    CBaseMesh_Draw(mTriangle, GL_TRIANGLES);
    UI_FrameEnd();

    while (glGetError() != GL_NO_ERROR)
      printf("GL error\n");

    SDL_GL_SwapWindow(win->window);

  }

  CBaseShader_Destroy(shadertest);
  CBaseShader_Destroy(shader);
  Camera_Destroy(camera);
  CBaseMesh_Destroy(mTriangle);

  PlatformInput_Destroy(input);
  PlatformWindow_Destroy(win);
 

  SDL_Quit();
  return 0;
}
