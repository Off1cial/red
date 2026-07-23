#include "game/server/server.h"
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <assert.h>

static void setclients(server_t* server)
{
  for (int i = 0; i < SERVER_MAX_CLIENTS; i++)
  {
    svclient_t* c = &server->clients[i];
    memset(&c->addr_udp, 0, sizeof(struct sockaddr_in));
    c->state = SVCLIENT_STATE_CLOSED;
    c->socket_tcp = -1;
  }
}

int SV_Init(
    server_t* server,
    const char* name,
    netdomain_t domain
    )
{
  if (!server)
    return SERVER_FAILURE;
  
  memset(server, 0, sizeof(server_t));
  server->state = SERVER_STATE_LOADING;

  // TCP setup
  memset(
    &server->addr_tcp, 
    0, 
    sizeof(struct sockaddr_in)
  );
  
  setclients(server);

  server->socket_tcp = pltNetSocketCreate(domain, NET_PROTOCOL_TCP);
  if (server->socket_tcp == -1)
  {
    perror("[SERVER]: Socket (TCP)");
    return SERVER_FAILURE;
  }

  server->addr_tcp.sin_family = domain;
  server->addr_tcp.sin_port = htons(SERVER_PORT);
  server->addr_tcp.sin_addr.s_addr = INADDR_ANY;

  int bindtcp = 
  bind(
    server->socket_tcp,
    (struct sockaddr*)&server->addr_tcp,
    sizeof(server->addr_tcp)
  );

  if (bindtcp < 0)
  {
    perror("[SERVER]: TCP Bind");
    return SERVER_FAILURE;
  }

  listen(server->socket_tcp, SERVER_MAX_CLIENTS);


  // UDP setup
  
  memset(
    &server->addr_udp,
    0,
    sizeof(struct sockaddr_in)
  );
  
  server->socket_udp = pltNetSocketCreate(domain, NET_PROTOCOL_UDP);
  if (server->socket_udp == -1)
  {
    perror("[SERVER]: Socket (UDP)");
    return SERVER_FAILURE;
  }

  server->addr_udp.sin_family = domain;
  server->addr_udp.sin_port = htons(SERVER_PORT);
  server->addr_udp.sin_addr.s_addr = INADDR_ANY;
  
  int bindudp =
  bind(
    server->socket_udp,
    (struct sockaddr*)&server->addr_udp,
    sizeof(server->addr_udp)
  );
 
  if (bindudp < 0)
  {
    perror("[SERVER]: UDP Bind");
    return SERVER_FAILURE;
  }

  return SERVER_SUCCESS;
}
