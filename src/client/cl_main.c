#include <stdio.h>
#include <stdlib.h>

#include "platform/window.h"
#include "platform/input.h"
#include "corebase/time.h"

#include "engine/shader.h"
#include "engine/mesh.h"
#include "engine/camera.h"

#include "client/client.h"
#include "shared/network/packet.h"

// cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
// ninja -C build
 
// https://www.reddit.com/r/playboicarti/s/lpdLQ9IYHt


// test triangle

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

client_t* client;

int main()
{

  client = malloc(sizeof(client_t));
  CL_Init(client, "redw0od0");
  printf("Hello world\n");

  pltWindow* win = PlatformWindow_Create(640,480, "RED");
  pltInput* input = PlatformInput_Create();

  
  CBaseShader* shadertest = CBaseShader_Create("../Assets/Shaders/vert_test.vs", "../Assets/Shaders/frag_test.fs");
  CBaseShader* shader     = CBaseShader_Create("../Assets/Shaders/vert_unlit.vs", "../Assets/Shaders/frag_unlit.fs");
  
  CBaseMesh* mTriangle = CBaseMesh_Create(3, 3);
  CBaseMesh_PushTriangleVerts(mTriangle, v0, v1, v2);
  CBaseMesh_Upload(mTriangle, GL_STATIC_DRAW);

  camera_t* camera = Camera_Create(VECTOR_ZERO, VECTOR_AXIS_Z_NEG, (cViewport){0,0,1280,720});

  glViewport(0,0, win->winw, win->winh);



  CL_Connect(client, "127.0.0.1", SERVER_PORT, NET_PROTOCOL_UDP);
  
  
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


  pltTime_Init();
  double timestamp = pltTime_Time();
  int quit = 0;
  while(!quit)
  {

    double time = pltTime_Time();
    double dt = time - timestamp;
    if (client->socket_udp != -1)
      CL_ReceivePacketUDP(client);

    if (dt >= 3.0f)
    {
      if (client->state != CSTATE_CONNECTED)
      {
        printf("Attempting connection...\n");
        if (client->socket_udp == -1)
          CL_Connect(client, "127.0.0.1", SERVER_PORT, NET_PROTOCOL_UDP);
        else
        CL_SendConnectPacket(client);
      }

      if (client->state == CSTATE_CONNECTED)
      {
        CL_SendPacketUDP(client, &testpacket);
        printf("Packet sent\n");
      }
      timestamp = time;
    }

    glClearColor(0.12f, 0.7f, 0.7f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    PlatformInput_Poll(input, &quit); 
    // Check for window resize
    if (input->eventWindowResized)
    {
      PlatformWindow_Resize(
          win,
          input->eventWindowNewWidth,
          input->eventWindowNewHeight
      );
    }
    
    Camera_Look(camera, input->mxrel, input->myrel, 0.2f);

    if (input->pressed[SDL_SCANCODE_S])
    {
      printf("Move\n");
      camera->origin = VectorSub(camera->origin, camera->front);
    }


    Camera_Update(camera);
    CBaseShader_Use(shader);
    CBaseShader_SetMat4(shader, SH_UNIFORM_VIEW, camera->view);
    CBaseShader_SetMat4(shader, SH_UNIFORM_PROJECTION, camera->projection);
    CBaseShader_SetMat4(shader, SH_UNIFORM_MODEL, Mat4Identity());

    CBaseMesh_Draw(mTriangle, GL_TRIANGLES);

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
