#include "platform/network/network.h"

netsocket_t pltNetSocketCreate(
    netdomain_t domain,
    netprotocol_t protocol
    )
{
  int type;
  switch (protocol)
  {
    case NET_PROTOCOL_TCP:
      type = SOCK_STREAM;
      break;
    case NET_PROTOCOL_UDP:
      type = SOCK_DGRAM;
      break;
    default:
      break;
  }

  return socket(domain, type, 0);
}
