#include <stdio.h>
#include <netdb.h>
#include "server/server.h"
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


  double current_time = pltTime_Time(); 
  while (server->state == SERVER_STATE_ACTIVE)
  {
    double time = pltTime_Time();
    float dt = (time - current_time);
    server->seconds+=dt;

    if (server->seconds >= 30.0f)
    {
      goto shutdown;
      break;
    }

    if (dt >= 1.0f / SERVER_TICKRATE)
    {
      current_time = time;
      //SV_ClientAcceptTCP(server);
      SV_ReceivePacketUDP(server);
      for (int i = 0; i < server->clientcount; i++)
      {
        if (server->clients[i].state != SVCLIENT_STATE_CONNECTED)
          continue;
        server->clients[i].time_elapsed+=dt;
      }
    }
  }
  
shutdown:
  SV_Close(server);

  return 0;
}
