#pragma once


#include "platform/network/network.h"
#include "shared/network/client.h"

typedef struct svclient_t
{
  struct sockaddr_in addr_udp;
  netsocket_t socket_tcp;
  
  clientstate_t state;

  double time_connected;
  double time_lastrecv;
  double time_elapsed;

  char name[64];

} svclient_t;
