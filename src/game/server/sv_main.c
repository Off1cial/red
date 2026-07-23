#include <stdio.h>
#include <netdb.h>
#include <pthread.h>
#include "game/server/server.h"
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


server_t* server = NULL;

void* thread_test()
{
  printf("Thread works\n");
  return NULL;
}

int main()
{
  server = malloc(sizeof(server_t));
  pltTime_Init();
  
  int serverresult = SV_Init
    (
     server,
     "TestServer",
     NET_DOMAIN_IPV4
    );

  if (serverresult != SERVER_SUCCESS)
  {
    printf("[SERVER]: Failed to start\n");
    return 1;
  }

  server->state = SERVER_STATE_ACTIVE;

  char hostname[256];
  gethostname(hostname, sizeof(hostname));
  
  char hostip[256];
  struct hostent *host = gethostbyname(hostname);
  
  strcpy(hostip, inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));

  printf("[SERVER]: %dHz Server started on %s:%d\n", SERVER_TICKRATE, hostip, ntohs(server->addr_udp.sin_port));


  double previous = pltTime_Time(); 
  double accumulator = 0.0f;


  while (server->state == SERVER_STATE_ACTIVE)
  {

    double time = pltTime_Time();
    accumulator += time - previous;
    previous = time;

    if (accumulator >= 1.0f / SERVER_TICKRATE)
    {
      //SV_ClientAcceptTCP(server);
      SV_ReceivePacketUDP(server);
      for (int i = 0; i < server->clientcount; i++)
      {
        if (server->clients[i].state != SVCLIENT_STATE_CONNECTED)
          continue;
        server->clients[i].time_elapsed+=time-previous;
      }
      accumulator -= (1.0 / SERVER_TICKRATE);
    }
  }
  
  SV_Close(server);

  return 0;
}
