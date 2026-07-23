#include "game/server/server.h"
#include "shared/network/packet.h"
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

      client->time_connected = pltTime_Time();

      printf("[SERVER]: Client connected\n  Clients: (%d/%d)\n  Time: %lf\n",
          server->clientcount, SERVER_MAX_CLIENTS, client->time_connected);
      return SERVER_SUCCESS;

    }

  }
  close(client_socket);
  return SERVER_FAILURE;
}

int SV_ClientConnectUDP(
    server_t* server,
    struct sockaddr_in* addr,
    netpacket_t* packet
)
{
    for (int i = 0; i < SERVER_MAX_CLIENTS; i++)
    {
        svclient_t* client = &server->clients[i];

        if (client->state == SVCLIENT_STATE_CLOSED)
        {
            client->addr_udp = *addr;
            client->state = SVCLIENT_STATE_CONNECTED;

            strcpy(client->name, packet->data);

            client->time_connected = pltTime_Time();
            client->time_elapsed = 0.0f;
            server->clientcount++;

            printf(
                "[SERVER]: UDP client connected: %s\n",
                client->name
            );

            return i;
        }
    }
    return -1;
}


int SV_ClientUpdateTimeElapsed(svclient_t* client);



svclient_t* SV_FindClientUDP(server_t* server, struct sockaddr_in* addr)
{
  for (int i = 0; i < server->clientcount; i++)
  {
    svclient_t* client = &server->clients[i];
    if (client->state == SVCLIENT_STATE_CLOSED)
      continue;

    int match = (client->addr_udp.sin_port ==  addr->sin_port)
      && (client->addr_udp.sin_addr.s_addr == addr->sin_addr.s_addr);

    if (match)
      return client;
  }
  return NULL;
}
