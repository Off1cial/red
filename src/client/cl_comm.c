#include "client/client.h"
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
      client->state = CSTATE_CONNECTED;
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
  }

  return CLIENT_SUCCESS;
}
