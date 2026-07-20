#pragma once

#define CLIENT_FAILURE 0
#define CLIENT_SUCCESS 1

#include "shared/network/client.h"
#include "shared/network/packet.h"

typedef struct client_s
{
  struct sockaddr_in addr_udp;
  struct sockaddr_in addr_tcp;
  netsocket_t socket_tcp;
  netsocket_t socket_udp;

  clientstate_t state;

  char name[64];

} client_t;


void CL_Init(client_t* client, const char* name);

int CL_Connect(
    client_t* client, 
    const char* ip, 
    short int port,
    netprotocol_t protocol
);

int CL_SendPacketUDP(client_t* client, netpacket_t* packet);
int CL_SendPacketTCP(client_t* client, netpacket_t* packet);


