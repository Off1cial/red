#include "server/server.h"
#include "corebase/time.h"
#include <unistd.h>

int SV_ClientAcceptTCP(server_t* server)
{
  if (server->clientcount >= SERVER_MAX_CLIENTS)
    return SERVER_FAILURE;

  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);

  int client_socket =
    accept(
        server->socket_tcp,
        (struct sockaddr*)&addr,
        &len
        );

  if (client_socket == -1)
    return SERVER_FAILURE;

  for (int i = 0; i < SERVER_MAX_CLIENTS; i++)
  {
    svclient_t* client = &server->clients[i];
    if (client->socket_tcp == -1)
    {
      client->socket_tcp = client_socket;
  
      strcpy(client->name, "Unknown");
      server->clientcount++;

      client->time_connected = pltTime_Time() * 1000.0f;

      printf("[SERVER]: Client connected\n  Clients: (%d/%d)\n  Time: %lf\n",
          server->clientcount, SERVER_MAX_CLIENTS, client->time_connected);
      return SERVER_SUCCESS;

    }

  }
  close(client_socket);
  return SERVER_FAILURE;
}


int SV_ClientUpdateTimeElapsed(svclient_t* client);
