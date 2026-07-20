#include <stdio.h>
#include <netdb.h>
#include "server/server.h"
#include "corebase/time.h"

server_t* server = NULL;

int main()
{
  server = malloc(sizeof(server_t));
  
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

  printf("[SERVER]: Server started on %s:%d\n", hostip, ntohs(server->addr_udp.sin_port));


  double current_time = pltTime_Time(); 

  while (server->state == SERVER_STATE_ACTIVE)
  {
    double time = pltTime_Time();
    float dt = (time - current_time);
    current_time = time;
    
  

    SV_ClientAcceptTCP(server);
  }

  return 0;
}
