#pragma once


#include "shared/network/pframe.h"
#include "platform/network/network.h"

typedef enum svclientstate_t
{
  SVCLIENT_STATE_CLOSED,
  SVCLIENT_STATE_CONNECTED,
  SVCLIENT_STATE_CONNECTING,
  SVCLIENT_STATE_SPAWNED,
  SVCLIENT_STATE_ZOMBIE, // Connected but not responding, allows reconnection


} svclientstate_t;

typedef struct svclient_t
{
  struct sockaddr_in addr_udp;
  netsocket_t socket_tcp;
  
  svclientstate_t state;

  double time_connected;
  double time_lastrecv;
  double time_elapsed;

  char name[64];
  uint16_t id;

} svclient_t;



