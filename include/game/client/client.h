#pragma once

#define CLIENT_FAILURE 0
#define CLIENT_SUCCESS 1

#define CLIENT_NAME_LENGTH 64

#define CLIENT_CMD_BACKUP 64

#include "shared/network/client.h"
#include "shared/network/packet.h"
#include "shared/network/pframe.h"

#include "game/client/cl_player.h"



typedef enum clientstate_t
{
  CSTATE_EMPTY,
  CSTATE_CONNECTING,
  CSTATE_CONNECTED, // Bound address/socket
  CSTATE_ACTIVE,
  CSTATE_ZOMBIE, // Not responding but dont close socket yet

} clientstate_t;



typedef struct client_s
{
  struct sockaddr_in addr_udp;
  struct sockaddr_in addr_tcp;
  netsocket_t socket_tcp;
  netsocket_t socket_udp;

  clientstate_t state;
  uint16_t serverslot;

  int connect_attempts;
  int connect_maxattempts;

  float time_lastconnectattempt;
  float time_lastcmdsent;


  char name[CLIENT_NAME_LENGTH];

  playercmd_t cmds[CLIENT_CMD_BACKUP];
  int cmdcount;

} client_t;

extern client_t* gClient;

int CL_Init(client_t* client, const char* name);


void CL_Loop(client_t* client, double dt);

// Network fundamentals

// Simply forms connection between client and server
int CL_Connect(
    client_t* client, 
    const char* ip, 
    short int port,
    netprotocol_t protocol
);

int CL_Disconnect( client_t* client );

int CL_SendPacketUDP(client_t* client, netpacket_t* packet);
int CL_SendPacketTCP(client_t* client, netpacket_t* packet);

int CL_SendChallengePacket(client_t* client);

int CL_ReceivePacketUDP(client_t* client);

// Game based


// Makes a certain number of attempts to join a server
int CL_GameServerJoin(client_t* client, const char* ip, short int port, netprotocol_t protocol, int retries);
int CL_GameServerDisconnect(
    client_t* client, 
    char* msg,
    size_t msglen
    );


// Send player information to the current server
int CL_SendPlayerFrame(client_t* client, playerframe_t* frame);
// Receive player information from the current server
int CL_ReceivePlayerFrame(client_t* client, playerframe_t* frame);


void CL_CreatePlayerCommand(float dt);
void CL_SendPlayerCommand(client_t* client);

uint8_t CL_GetMessage(netpacket_t* msgout);
uint8_t CL_ParseMessage(netpacket_t* packet);

void CL_Think(float dt);


void CL_ProcessPlayerFrame(playerframe_t frame);
void CL_PlayerThink();


void CL_PMove(float dt);