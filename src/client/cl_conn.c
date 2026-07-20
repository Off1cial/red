#include "client/client.h"
#include "platform/network/network.h"
#include <stdio.h>

static void clientsetaddr(
    client_t* client,
    netdomain_t domain,
    const char* ip,
    short int port
    )
{
  if (!client)
  {
    return;
  }

  client->addr_tcp.sin_family = domain;
  client->addr_tcp.sin_port = htons(port);
  client->addr_tcp.sin_addr.s_addr = inet_addr(ip);
}


int CL_Connect(client_t* client, const char* ip, short int port, netprotocol_t protocol)
{
  if (client->state == CSTATE_CONNECTED)
    return CLIENT_FAILURE;
  
  netdomain_t domain = NET_DOMAIN_IPV4;
  
  struct sockaddr_in* addr;
  netsocket_t* pSocket;
  pSocket = (protocol == NET_PROTOCOL_TCP) ?  
    &client->socket_tcp : 
    &client->socket_udp;

  addr = (protocol == NET_PROTOCOL_TCP) ? 
    &client->addr_tcp :
    &client->addr_udp;

  int type = (protocol == NET_PROTOCOL_TCP) ?
    SOCK_STREAM :
    SOCK_DGRAM;

  *pSocket = socket(NET_DOMAIN_IPV4, type, 0);

  if ((*pSocket) == -1)
  {
    perror("[CLIENT]: Socket");
    return CLIENT_FAILURE;
  }
  
  clientsetaddr(client, domain, ip, port);

  int conresult = connect
    (
     *pSocket,
     (struct sockaddr*)addr,
     sizeof((*addr))
    );

  if (conresult == -1)
  {
    perror("[CLIENT]: Connection");
    close(*pSocket);
    return CLIENT_FAILURE;
  }
  
  client->state = CSTATE_CONNECTED;
  printf("[CLIENT]: Connected to %s:%d\n", ip, port);

  return CLIENT_SUCCESS;
}

int CL_Disconnect(client_t* client)
{
  if (client->state != CSTATE_CONNECTED)
    return CLIENT_FAILURE;

  // Send message to server to notify

  // Close

  // return success
}
