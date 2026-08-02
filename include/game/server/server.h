#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include "platform/common.h"
#include "platform/network/network.h"
#include "shared/network/packet.h"
#include "shared/network/pframe.h"

#include "game/server/sv_client.h"
#include "game/server/sv_player.h"


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
  svplayer_t players[SERVER_MAX_CLIENTS]; // More for bots?
  int clientcount;
  int playercount;

  char name[STRING_MAX_LENGTH];

  unsigned long int seconds;
  server_state_t state;
  
  netsocket_t socket_tcp;
  netsocket_t socket_udp;

  struct sockaddr_in addr_tcp;
  struct sockaddr_in addr_udp;


} server_t;

extern server_t* gServer;

int SV_Init(
    server_t* server, 
    const char* name, 
    netdomain_t domain
);

void SV_Close(server_t* server);

void SV_CreatePlayer(svclient_t* client);

int SV_ClientAcceptTCP(server_t* server);
int SV_ClientConnectUDP(server_t* server, struct sockaddr_in* addr, netpacket_t* packet);

int SV_ClientDrop(server_t* server, int client, const char* msg);

int SV_SendPacketToClientUDP(server_t* server, int client, netpacket_t* packet);

int SV_ReceivePacketUDP(server_t* server);

svclient_t* SV_FindClientUDP(server_t* server, struct sockaddr_in* addr); 


void SV_SendPlayerFrame(svclient_t* client);

extern float host_frametime;
