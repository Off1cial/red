#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include "platform/common.h"
#include "platform/network/network.h"
#include "shared/network/packet.h"
#include "shared/network/pframe.h"

#include "server/sv_client.h"


#define SERVER_MAX_CLIENTS 32
#define SERVER_TICKRATE 64


#define SERVER_FAILURE 0
#define SERVER_SUCCESS 1

typedef enum server_state_t
{
  SERVER_STATE_LOADING  = 0,
  SERVER_STATE_ACTIVE   = 1,
  SERVER_STATE_STOPPING = 2,

} server_state_t;


typedef struct server_t
{
  svclient_t clients[SERVER_MAX_CLIENTS];
  int clientcount;

  char name[STRING_MAX_LENGTH];

  unsigned long int seconds;
  server_state_t state;
  
  netsocket_t socket_tcp;
  netsocket_t socket_udp;

  struct sockaddr_in addr_tcp;
  struct sockaddr_in addr_udp;


} server_t;


int SV_Init(
    server_t* server, 
    const char* name, 
    netdomain_t domain
);

void SV_Close(server_t* server);

int SV_ClientAcceptTCP(server_t* server);
int SV_ClientConnectUDP(server_t* server, struct sockaddr_in* addr, netpacket_t* packet);

int SV_ClientDrop(server_t* server, int client, const char* msg);

int SV_SendPacketToClientUDP(server_t* server, int client, netpacket_t* packet);

int SV_ReceivePacketUDP(server_t* server);

svclient_t* SV_FindClientUDP(server_t* server, struct sockaddr_in* addr); 
