#pragma once

#define CLIENT_FAILURE 0
#define CLIENT_SUCCESS 1

#define CLIENT_NAME_LENGTH 64

#include "shared/network/client.h"
#include "shared/network/packet.h"

typedef struct client_s
{
  struct sockaddr_in addr_udp;
  struct sockaddr_in addr_tcp;
  netsocket_t socket_tcp;
  netsocket_t socket_udp;

  clientstate_t state;
  uint8_t serverslot;

  char name[CLIENT_NAME_LENGTH];

} client_t;


int CL_Init(client_t* client, const char* name);

int CL_Connect(
    client_t* client, 
    const char* ip, 
    short int port,
    netprotocol_t protocol
);

int CL_Disconnect( client_t* client );

int CL_SendPacketUDP(client_t* client, netpacket_t* packet);
int CL_SendPacketTCP(client_t* client, netpacket_t* packet);

int CL_SendConnectPacket(client_t* client);

int CL_ReceivePacketUDP(client_t* client);

// Makes a certain number of attempts to join a server
int CL_GameServerJoin(client_t* client, const char* ip, short int port, netprotocol_t protocol, int retries);
int CL_GameServerDisconnect(
    client_t* client, 
    char* msg,
    size_t msglen
    );
