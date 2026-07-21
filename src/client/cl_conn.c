#include "client/client.h"
#include "platform/network/network.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int CL_SendConnectPacket(client_t* client)
{
  netpacket_t packet = {0};

  packet.type = NET_PACKET_CONNECT;

  strncpy(
        packet.data,
        client->name,
        sizeof(packet.data) - 1
  );

  return CL_SendPacketUDP(client, &packet);
}

static void clientsetaddr(
    struct sockaddr_in* addr,
    netdomain_t domain,
    const char* ip,
    short int port
)
{
  memset(addr, 0, sizeof(struct sockaddr_in));

  addr->sin_family = domain;
  addr->sin_port = htons(port);
  addr->sin_addr.s_addr = inet_addr(ip);
}


int CL_Connect(
    client_t* client,
    const char* ip,
    short int port,
    netprotocol_t protocol
)
{
  if (!client)
    return CLIENT_FAILURE;

  if (client->state == CSTATE_CONNECTED)
    return CLIENT_FAILURE;


  netdomain_t domain = NET_DOMAIN_IPV4;

  struct sockaddr_in* addr;
  netsocket_t* socket;


  /*
   * Select which socket and address to use.
   * TCP uses a real connection.
   * UDP uses connect() only to set the default destination.
   */
  if (protocol == NET_PROTOCOL_TCP)
  {
    socket = &client->socket_tcp;
    addr = &client->addr_tcp;
  }
  else
  {
    socket = &client->socket_udp;
    addr = &client->addr_udp;
  }


  if (*socket == -1)
  {

    *socket = pltNetSocketCreate(
                domain,
                protocol
    );
  }


  if (*socket == -1)
  {
    perror("[CLIENT]: Socket");
    return CLIENT_FAILURE;
  }


  /*
   * Fill in the destination address.
   */
  clientsetaddr(
      addr,
      domain,
      ip,
      port
  );


  /*
   * For TCP:
   *   Performs the connection handshake.
   *
   * For UDP:
   *   Does not create a connection.
   *   It only stores the destination address so
   *   send()/recv() can be used instead of
   *   sendto()/recvfrom().
   */
  int conresult = connect(
      *socket,
      (struct sockaddr*)addr,
      sizeof(*addr)
  );


  if (conresult == -1)
  {
    perror("[CLIENT]: Connection");
    close(*socket);
    *socket = -1;
    return CLIENT_FAILURE;
  }

  printf(
      "[CLIENT]: Connecting to %s:%d (%s)\n",
      ip,
      port,
      protocol == NET_PROTOCOL_TCP ? "TCP" : "UDP"
  );
  
  client->state = CSTATE_CONNECTING;
  CL_SendConnectPacket(client);

  return CLIENT_SUCCESS;
}


int CL_Disconnect(client_t* client)
{
  if (!client)
    return CLIENT_FAILURE;

  if (client->state != CSTATE_CONNECTED)
    return CLIENT_FAILURE;


  /*
   * Later:
   * Send a disconnect packet before closing.
   */

  netpacket_t pDisconnect = {0};
  pDisconnect.type = NET_PACKET_DISCONNECT;
  strncpy(pDisconnect.data, client->name, strlen(client->name));
  pDisconnect.data[strlen(client->name)] = '\0';
  CL_SendPacketUDP(client, &pDisconnect);
  

  if (client->socket_tcp != -1)
  {
    close(client->socket_tcp);
    client->socket_tcp = -1;
  }


  if (client->socket_udp != -1)
  {
    close(client->socket_udp);
    client->socket_udp = -1;
  }


  client->state = CSTATE_EMPTY;

  return CLIENT_SUCCESS;
}
