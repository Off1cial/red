#include "game/client/client.h"
#include "engine/camera.h"
#include <stdint.h>
#include <unistd.h>

int CL_SendPacketUDP(client_t* client, netpacket_t* packet)
{
  if (!client || !packet)
    return CLIENT_FAILURE;
  
  //printf("Client UDP socket: %d\n", client->socket_udp);
  size_t n = sizeof(packet->type) + 
             sizeof(packet->size) + packet->size;
  int res = sendto(
      client->socket_udp,
      packet,
      sizeof(*packet),
      0,
      (struct sockaddr*)&client->addr_udp,
      sizeof(client->addr_udp)
      );

  if (res < 0)
    return 0;

  printf("[CLIENT]: Packet sent of size %zuB\n", sizeof(*packet));

  return 1;
}

int CL_ReceivePacketUDP(client_t* client)
{
  //printf("Client UDP socket: %d\n", client->socket_udp);
  //printf(
    //"Local UDP port %d\n",
    //ntohs(client->addr_udp.sin_port)
// );
  netpacket_t packet;
  struct sockaddr_in from;
  socklen_t fromlen = sizeof(from);

  int size = recv(client->socket_udp, &packet, sizeof(packet), MSG_DONTWAIT);

  /*
  int size =
    recvfrom(
        client->socket_udp,
        &packet,
        sizeof(packet),
        MSG_DONTWAIT,
        (struct sockaddr*)&from,
        &fromlen
        );
  */
  if (size <= 0)
  {
    //perror("[CLIENT]: Recvfrom");
    return CLIENT_FAILURE;
  }
  printf("Received size = %d\n", size);

  switch (packet.type)
  {
    case NET_PACKET_ACCEPT:
      uint8_t index = -1;
      memcpy(&index, packet.data, sizeof(uint8_t));

      printf("[CLIENT]: Server accepted connection!\n  Index: %d\n", index);

      client->serverslot = index;
      client->state = CSTATE_ACTIVE;
      break;
    case NET_PACKET_DISCONNECT:
      // Server sent a DC packet
      CL_Disconnect(client);
      printf("Disconnected from server (%s)\n", packet.data);
      client->state = CSTATE_EMPTY;
      break;
    default:
      printf("[CLIENT][PACKET]: Generic packet received\n");
      break;

    case NET_PACKET_PFRAME:
      printf("Player frame received\n");
      playerframe_t* frame = (playerframe_t*)&packet.data;
      //printf("Origin: (%0.2f, %0.2f, %0.2f)\n", frame->origin[0], frame->origin[1], frame->origin[2]);
      //VectorCopy(frame->origin, gCamera->origin);
      CL_ProcessPlayerFrame(*frame);
      break;
  }

  return CLIENT_SUCCESS;
}



// NEW TIDIER FUNCTIONS


uint8_t CL_GetMessage(netpacket_t* msgout)
{
  netpacket_t inpacket = {0};
  struct sockaddr_in serveraddr;
  socklen_t fromlen = sizeof(serveraddr);

  int size = 
    recv(
      gClient->socket_udp, 
      &inpacket, 
      sizeof(inpacket), 
      MSG_DONTWAIT);

    
  if (size <= 0)
  {
    return CLIENT_FAILURE;
  }

  *msgout = inpacket;
  return CLIENT_SUCCESS;
}

uint8_t CL_ParseMessage(netpacket_t* packet)
{
  //printf("TYPE = %d\n", packet->type);
  switch(packet->type)
  {
    case NET_PACKET_ACCEPT:
      // Server allowed connection
      uint16_t clientindex = -1;
      memcpy(&clientindex, packet->data, sizeof(clientindex));
      printf("[CLIENT]: Server accepted connection!\n  Index: %d\n", clientindex);

      gClient->serverslot = clientindex;
      gClient->state = CSTATE_ACTIVE;
      break;
    
    case NET_PACKET_DISCONNECT:
      // Server told us to disconnect
      CL_Disconnect(gClient);
      gClient->state = CSTATE_EMPTY;
      break;

    case NET_PACKET_PFRAME:
      // Received a player frame
      playerframe_t* frame = (playerframe_t*)&packet->data;
      CL_ProcessPlayerFrame(*frame);
      break;
    default:
      return CLIENT_FAILURE;
      break;
  }
  return CLIENT_SUCCESS;
}



void CL_SendPlayerCommand(client_t* client)
{
  netpacket_t movpacket = {0};
  movpacket.type = NET_PACKET_CLCMD;

  int max = NET_PACKET_SIZE / sizeof(playercmd_t);
  int backup = (max > 3) ? 3 : max;
  byte* ptr = &movpacket.data[0];
  //printf("Preparing player commands\n");
  for (int i = 0; i < backup; i++)
  {
    int idx = (gClient->cmdcount - 1 - i) % CLIENT_CMD_BACKUP;
    if (idx < 0)
      break;
    playercmd_t* cmd = &gClient->cmds[idx];
    memcpy(ptr, cmd, sizeof(playercmd_t));
    ptr+=sizeof(playercmd_t);
  }
  CL_SendPacketUDP(gClient, &movpacket);
}
