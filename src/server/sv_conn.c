#include "server/server.h"

// Idea: upon connecetion, send the player index back to the client

int SV_ClientDrop(server_t* server, int client, const char* msg)
{
  if (!server || (client < 0 || client >= SERVER_MAX_CLIENTS))
    return SERVER_FAILURE;

  svclient_t* cl = &server->clients[client];
  if (cl->state != SVCLIENT_STATE_CONNECTED)
    return SERVER_FAILURE;

  // Send packet with the message
  netpacket_t packet = {0};
  packet.type = NET_PACKET_DISCONNECT;
  strncpy(packet.data, msg, strlen(msg));
  SV_SendPacketToClientUDP(server, client, &packet);
  

  close(cl->socket_tcp);
  memset(&cl->addr_udp, 0, sizeof(struct sockaddr_in));
  cl->state = SVCLIENT_STATE_CLOSED;
  cl->socket_tcp = -1;
  server->clientcount--;

  return SERVER_SUCCESS;
}


