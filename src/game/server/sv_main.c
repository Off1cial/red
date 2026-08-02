#include <stdio.h>
#include <netdb.h>
#include <pthread.h>
#include "engine/physics.h"
#include "game/server/server.h"
#include "platform/cpu.h"
#include "corebase/time.h"

void SV_Close(server_t* server)
{
  if (!server)
    return;
  server->state = SERVER_STATE_STOPPING;
  for (int i = 0; i < server->clientcount; i++)
  {
    SV_ClientDrop(server, i, "Server is shutting down...");
  }
  
  free(server);
}


server_t* gServer = NULL;

void* thread_test()
{
  printf("Thread works\n");
  return NULL;
}

int main()
{
  gServer = malloc(sizeof(server_t));
  cpufeatures_t cpufeat;
  gPltCPUFeatures = &cpufeat;
  pltCPU_GetFeatures();
  pltTime_Init();
  
  int serverresult = SV_Init
    (
     gServer,
     "TestServer",
     NET_DOMAIN_IPV4
    );

  if (serverresult != SERVER_SUCCESS)
  {
    printf("[SERVER]: Failed to start\n");
    return 1;
  }

  gServer->state = SERVER_STATE_ACTIVE;

  char hostname[256];
  gethostname(hostname, sizeof(hostname));
  
  char hostip[256];
  struct hostent *host = gethostbyname(hostname);
  
  strcpy(hostip, inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));

  printf("[SERVER]: %dHz Server started on %s:%d\n", SERVER_TICKRATE, hostip, ntohs(gServer->addr_udp.sin_port));

  CBasePhysBodies_Init(gPhysBodies, 256);

  double previous = pltTime_Time(); 
  double accumulator = 0.0f;

  float host_frametime = 0.0f;

  while (gServer->state == SERVER_STATE_ACTIVE)
  {

    double time = pltTime_Time();
    accumulator += time - previous;
    host_frametime = time - previous;
    previous = time;

    if (accumulator >= 1.0f / SERVER_TICKRATE)
    {
      //SV_ClientAcceptTCP(server);
      SV_ReceivePacketUDP(gServer);
      for (int i = 0; i < gServer->clientcount; i++)
      {
        svclient_t* client = &gServer->clients[i];
        if (client->state != SVCLIENT_STATE_CONNECTED)
          continue;

        client->time_elapsed+=host_frametime;
        // Accelerate.. move etc
        svplayer_t* cplayer = &gServer->players[client->playerid];
        SV_PlayerThink(cplayer, (float)(1.0f/SERVER_TICKRATE));
      }
      accumulator -= (1.0 / SERVER_TICKRATE);
      // Update physics
      //CBasePhysBodies_Update(gPhysBodies, SERVER_TICKRATE);
    }
    // Post simulation
    for (int i = 0; i < gServer->clientcount; i++)
    { 
      svclient_t* client = &gServer->clients[i];
      if (client->state != SVCLIENT_STATE_CONNECTED)
        continue;
      //svplayer_t* cplayer = &gServer->players[client->playerid];
      SV_SendPlayerFrame(client);
    }
  }
  
  SV_Close(gServer);

  return 0;
}
