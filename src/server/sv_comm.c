#include "server/server.h"
#include "shared/network/packet.h"

int SV_SendPacketToClientUDP(
    server_t* server,
    int client,
    netpacket_t* packet
)
{
  int fail = (!server) || 
    (client < 0 || client >= SERVER_MAX_CLIENTS) ||
    (!packet);

  if (fail)
    return SERVER_FAILURE;

  sendto(
      server->socket_udp,
      packet,
      sizeof(*packet),
      0,
      (struct sockaddr*)&server->clients[client].addr_udp,
      sizeof(server->clients[client].addr_udp)
      );
  printf("[SERVER]: Send packet of size %zuB to client %d\n",
      sizeof(*packet), client);

  return SERVER_SUCCESS;
}


int SV_ReceivePacketUDP(server_t* server)
{
    netpacket_t packet;

    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);

    int size = recvfrom(
        server->socket_udp,
        &packet,
        sizeof(packet),
        0,
        (struct sockaddr*)&from,
        &fromlen
    );


    if (size <= 0)
        return SERVER_FAILURE;

  
    printf("[SERVER]: Packet received via UDP:\n");
    printf("  Size: %dB\n", size);
    printf("  Data: %s\n", packet.data);
  
    svclient_t* client = SV_FindClientUDP(server, &from);
    switch (packet.type)
    {
      case NET_PACKET_CONNECT:
        printf("[SERVER]: UDP Connection packet received\n");
        printf("  Client: %s\n", packet.data);
  
        int client_id;

        if (client)
        {
          printf("[SERVER]: Client is already present\n");
          client_id = client - server->clients;
        }
        else
        {
          client_id = SV_ClientConnectUDP(
            server,
            &from,
            &packet
            );
        }

        printf("[SERVER]: Client found: ID %d/%d\n", client_id, server->clientcount );


        if (client_id >= 0)
        {
          netpacket_t response = {0};
          response.type = NET_PACKET_ACCEPT;
          uint8_t index = client_id;
          memcpy(response.data, &index, sizeof(uint8_t));

          int send = sendto(
              server->socket_udp,
              &response,
              sizeof(response),
              0,
              (struct sockaddr*)&from,
              fromlen
            );

          if (send <= 0)
          {
            perror(
                "[SERVER][PACKET]: Failed to send acceptance packet\n");
            break;
          }
          
          printf("[SERVER][PACKET]: Sending acceptance packet...\n");
          printf(
    "From %s:%d\n",
    inet_ntoa(from.sin_addr),
    ntohs(from.sin_port)
);
        }
        break;
      case NET_PACKET_DISCONNECT:
        printf("[SERVER][PACKET]: UDP Disconnection packet received\n");
        printf("  Dropping client '%s'...\n", packet.data);
        // int client = get client by name...
        int cid = client - server->clients; 
        SV_ClientDrop(server, cid, packet.data);
        break;
  

    }

    // Find or create client using 'from'

    return SERVER_SUCCESS;
}
